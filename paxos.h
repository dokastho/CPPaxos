#ifndef PAXOS_H
#define PAXOS_H

#include <string>
#include <map>
#include <vector>
#include <mutex>

#include "rpcs.h"
#include "Logger.h"
#include "drpc.h"

#define nil ""
#define prepare "Prepare"
#define accept "Accept"
#define learn "Learn"

#define RPC_TIMEOUT 200 // in ms

const Fate Decided = 0;
const Fate Pending = 1;
const Fate Forgotten = 2;

struct instance_t
{
    Fate status;
    int n_a;
    int n_p;
    PaxosOp v_a;
};

class Paxos
{
private:
    Logger *logger;
    std::map<int, instance_t> log;
    std::map<std::string, int> peer_max_done;
    std::vector<drpc_host> peers;
    drpc_server *drpc_engine;
    drpc_client *drpc_agent;
    int me; // index in peers
    int max_seq;
    int max_done;
    int max_n;
    int paxos_min;
    std::mutex set_sync, mu;

public:
    Paxos(int, std::string, std::vector<drpc_host> &);
    ~Paxos();

    //
    // the application wants paxos to start agreement on
    // instance seq, with proposed value v.
    // Start() returns right away; the application will
    // call Status() to find out if/when agreement
    // is reached.
    //
    // Comment credit to harshavm@umich.edu
    void Start(int, PaxosOp);

    //
    // the application on this machine is done with
    // all instances <= seq.
    //
    // see the comments for Min() for more explanation.
    //
    // Comment credit to harshavm@umich.edu
    void Done(int);

    //
    // the application wants to know the
    // highest instance sequence known to
    // this peer.
    //
    // Comment credit to harshavm@umich.edu
    int Max();

    //
    // Min() should return one more than the minimum among z_i,
    // where z_i is the highest number ever passed
    // to Done() on peer i. A peer's z_i is -1 if it has
    // never called Done().
    //
    // Paxos is required to have forgotten all information
    // about any instances it knows that are < Min().
    // The point is to free up memory in long-running
    // Paxos-based servers.
    //
    // Paxos peers need to exchange their highest Done()
    // arguments in order to implement Min(). These
    // exchanges can be piggybacked on ordinary Paxos
    // agreement protocol messages, so it is OK if one
    // peer's Min does not reflect another peer's Done()
    // until after the next instance is agreed to.
    //
    // The fact that Min() is defined as a minimum over
    // *all* Paxos peers means that Min() cannot increase until
    // all peers have been heard from. So if a peer is dead
    // or unreachable, other peers' Min()s will not increase
    // even if all reachable peers call Done(). The reason for
    // this is that when the unreachable peer comes back to
    // life, it will need to catch up on instances that it
    // missed -- the other peers therefore cannot forget these
    // instances.
    //
    // Comment credit to harshavm@umich.edu
    int Min();

    //
    // the application wants to know whether this
    // peer thinks an instance has been decided,
    // and if so, what the agreed value is. Status()
    // should just inspect the local peer state;
    // it should not contact other Paxos peers.
    //
    // Comment credit to harshavm@umich.edu
    std::pair<Fate, PaxosOp*> Status(int);

    // for testing only
    void Deafen();

    // RPC for invoking the paxos process
    static void paxos_rpc(Paxos *, drpc_msg &);

    std::string whoami();
    static void Prepare(Paxos *, drpc_msg &);
    static void Accept(Paxos *, drpc_msg &);
    static void Learn(Paxos *, drpc_msg &);

private:
    std::vector<PrepareReply> prepare_phase(int, int, PaxosOp);
    std::pair<std::vector<AcceptReply>, PaxosOp> accept_phase(int, int, PaxosOp, std::vector<PrepareReply> &);
    std::vector<DecidedReply> learn_phase(int, int, PaxosOp);
    void update_min();
    void update_peer_max(int, int);
    int get_max_n();
    int get_paxos_min();
    int get_max_seq();
    void set_max_seq(int);
    int get_max_done();
    void set_max_done(int);
    bool did_majority_accept(std::vector<int> &);
    std::pair<instance_t, bool> read_slot(int);
    void update_max_seq(int);
    bool do_accept_phase(int, std::vector<PrepareReply> &);
    instance_t rpc_inst_init(int);
    std::string peer_name(int);
};

#endif