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

typedef int Fate;
typedef void *interface;

const Fate Decided = 0;
const Fate Pending = 1;
const Fate Forgotten = 2;

struct instance_t
{
    Fate status;
    int n_a;
    int n_p;
    interface v_a;
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
    void Start(int, interface);
    void Done(int);
    int Max();
    int Min();
    // for testing only
    void Deafen();
    std::pair<Fate, interface> Status(int);
    std::string whoami();
    static void Prepare(Paxos *, drpc_msg &);
    static void Accept(Paxos *, drpc_msg &);
    static void Learn(Paxos *, drpc_msg &);

private:
    std::vector<PrepareReply> prepare_phase(int, int, interface);
    std::pair<std::vector<AcceptReply>, interface> accept_phase(int, int, interface, std::vector<PrepareReply> &);
    std::vector<DecidedReply> learn_phase(int, int, interface);
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
};

#endif