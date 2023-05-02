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

typedef std::string error;
typedef int Fate;
typedef void *interface;

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
    std::vector<std::string> peers;
    drpc_server *drpc_engine;
    int me; // index in peers
    int max_seq;
    int max_done;
    int max_n;
    int paxos_min;
    std::mutex set_sync;

public:
    Paxos(int, std::string);
    ~Paxos();
    void Start(int, interface);
    void Done(int);
    int Max();
    int Min();
    std::pair<Fate, interface> Status(int);
    error Prepare(PrepareArgs *, PrepareReply *);
    error Accept(AcceptArgs *, AcceptReply *);
    error Learn(DecidedArgs *, DecidedReply *);

private:
    std::vector<PrepareReply> prepare_phase(int, int, interface);
    std::pair<std::vector<AcceptReply>, interface> accept_phase(int, int, interface, std::vector<PrepareReply> &);
    std::vector<DecidedReply> learn_phase(int, int, interface);
    void update_min();
    void update_peer_max(std::string, int);
    int get_max_n();
    std::vector<std::string> get_peers();
    int get_paxos_min();
    int get_max_seq();
    void set_max_seq(int);
    int get_max_done();
    void set_max_done(int);
    bool did_majority_accept(std::vector<std::string>&);
    std::pair<instance_t, bool> read_slot(int);
    std::string whoami();
    void update_max_seq(int);
    bool do_accept_phase(int, std::vector<PrepareReply>&);
};

#endif