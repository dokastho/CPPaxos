#include <thread>
#include <string.h>

#include "paxos.h"

Paxos::Paxos(int my_index, std::string log_filename) : me(my_index)
{
    logger = new Logger(log_filename);

    drpc_host my_host{
        "localhost",
        PAXOS_PORT};
    drpc_engine = new drpc_server(my_host, this);

    // register RPC functions
    drpc_engine->publish_endpoint("Prepare", (void *)this->Prepare);
    drpc_engine->publish_endpoint("Accept", (void *)this->Accept);
    drpc_engine->publish_endpoint("Learn", (void *)this->Learn);

    std::thread t(&drpc_server::run_server, drpc_engine);
    t.detach();

    max_done = -1;
    for (size_t i = 0; i < peers.size(); i++)
    {
        peer_max_done[peers[i]] = max_done;
    }
    max_seq = -1;
    max_n = 0;
    paxos_min = -1;
}

Paxos::~Paxos()
{
    drpc_engine->kill();
}

void Paxos::Start(int seq, interface v) {}

void Paxos::Done(int seq)
{
    if (seq > get_max_done())
    {
        set_max_done(seq);
    }
    update_min();
}

int Paxos::Max()
{
    return get_max_done();
}

int Paxos::Min()
{
    update_min();
    return get_paxos_min() + 1;
}

std::pair<Fate, interface> Paxos::Status(int seq)
{
    if (seq < get_paxos_min())
    {
        return {Forgotten, nullptr};
    }
    else if (seq > get_max_seq())
    {
        return {Forgotten, nullptr};
    }
    else
    {
        auto item = read_slot(seq);
        bool ok = item.second;
        instance_t val = item.first;
        if (ok)
        {
            return {val.status, val.v_a};
        }
        else
        {
            return {Forgotten, nullptr};
        }
    }
}

std::vector<PrepareReply> Paxos::prepare_phase(int seq, int n, interface v)
{
    // get peers from state
    std::vector<std::string> peers_l = get_peers();
    size_t n_peers = peers_l.size();

    int goal = (int)n_peers / 2 + 1;
    int affirms = 0;

    // request content & replies vector
    std::vector<PrepareReply> replies;

    for (size_t p = 0; p < n_peers; p++)
    {
        if ((goal - affirms) > (int)(n_peers - p))
        {
            break;
        }

        PrepareArgs args{
            seq,
            n,
            v,
            get_max_seq(),
            me,
            get_max_done()};
        PrepareReply reply;
        rpc_arg_wrapper req;
        rpc_arg_wrapper rep;
        req.args = &args;
        req.len = sizeof(PrepareArgs);
        rep.args = &rep;
        rep.len = sizeof(PrepareReply);

        if ((int)p == me)
        {
            Prepare(this, &req, &rep);
        }
        else
        {
            drpc_client c;
            drpc_host h{peers_l[p], PAXOS_PORT};
            c.Call(h, "Prepare", &req, &rep);
        }
        // save reply
        replies.push_back(reply);
        if (affirms == goal)
        {
            break;
        }
    }

    return replies;
}

std::pair<std::vector<AcceptReply>, interface> Paxos::accept_phase(int seq, int n, interface v, std::vector<PrepareReply> &p_replies)
{
    int max_n_a = 0;
    interface v_prime = v;

    std::string whoiam = whoami();

    // find max n_a, set v_prime if one found
    for (PrepareReply reply : p_replies)
    {
        if (!reply.valid)
        {
            continue;
        }
        if (reply.n_a >= max_n_a)
        {
            max_n_a = reply.n_a;
            v_prime = reply.v_a;
        }
    }

    // get peers from state
    std::vector<std::string> peers_l = get_peers();
    size_t n_peers = peers_l.size();

    int goal = (int)n_peers / 2 + 1;
    int affirms = 0;

    // request content & replies vector
    std::vector<AcceptReply> replies;

    for (size_t p = 0; p < n_peers; p++)
    {
        if ((goal - affirms) > (int)(n_peers - p))
        {
            break;
        }
        AcceptArgs args{
            seq,
            n,
            v_prime,
            get_max_seq(),
            me,
            get_max_done()};
        AcceptReply reply;
        rpc_arg_wrapper req;
        rpc_arg_wrapper rep;
        req.args = &args;
        req.len = sizeof(AcceptArgs);
        rep.args = &rep;
        rep.len = sizeof(AcceptReply);

        if ((int)p == me)
        {
            Accept(this, &req, &rep);
        }
        else
        {
            drpc_client c;
            drpc_host h{peers_l[p], PAXOS_PORT};
            c.Call(h, "Accept", &req, &rep);
        }

        // efficiency
        if (reply.res == OK)
        {
            affirms++;
        }

        // save reply
        replies.push_back(reply);
        if (affirms == goal)
        {
            break;
        }
    }

    return {replies, v_prime};
}

std::vector<DecidedReply> Paxos::learn_phase(int seq, int n, interface v)
{
    // get peers from state
    std::vector<std::string> peers_l = get_peers();
    size_t n_peers = peers_l.size();

    // request content & replies vector
    std::vector<DecidedReply> replies;

    for (size_t p = 0; p < n_peers; p++)
    {
        DecidedArgs args{
            seq,
            n,
            v,
            get_max_seq(),
            me,
            get_max_done()};
        DecidedReply reply;
        rpc_arg_wrapper req;
        rpc_arg_wrapper rep;
        req.args = &args;
        req.len = sizeof(DecidedArgs);
        rep.args = &rep;
        rep.len = sizeof(DecidedReply);

        if ((int)p == me)
        {
            Learn(this, &req, &rep);
        }
        else
        {
            drpc_client c;
            drpc_host h{peers_l[p], PAXOS_PORT};
            c.Call(h, "Learn", &req, &rep);
        }
        // save reply
        replies.push_back(reply);
    }
    return replies;
}

void Paxos::update_min()
{
    mu.lock();
    int minimum = max_done;

    // want minimum in network
    for (auto it = peer_max_done.begin(); it != peer_max_done.end(); it++)
    {
        minimum = std::min(it->second, minimum);
    }
    paxos_min = minimum;
    for (auto it = log.begin(); it != log.end(); it++)
    {
        if (it->first <= minimum)
        {
            log.erase(it->first);
        }
    }

    mu.unlock();
}

void Paxos::update_peer_max(int peer_idx, int max_done)
{
    mu.lock();
    std::string peer = peers[peer_idx];
    if (peer_max_done[peer] < max_done)
    {
        peer_max_done[peer] = max_done;
    }
    mu.unlock();
    update_min();
}

int Paxos::get_max_n()
{
    mu.lock();
    int m = max_n;
    mu.unlock();
    return m;
}

std::vector<std::string> Paxos::get_peers()
{
    mu.lock();
    std::vector<std::string> p = peers;
    mu.unlock();
    return p;
}

int Paxos::get_paxos_min()
{
    mu.lock();
    int m = paxos_min;
    mu.unlock();
    return m;
}

int Paxos::get_max_seq()
{
    mu.lock();
    int m = max_seq;
    mu.unlock();
    return m;
}

void Paxos::set_max_seq(int val)
{
    mu.lock();
    max_seq = val;
    mu.unlock();
}

int Paxos::get_max_done()
{
    mu.lock();
    int m = max_done;
    mu.unlock();
    return m;
}

void Paxos::set_max_done(int val)
{
    mu.lock();
    max_done = val;
    for (auto it = log.begin(); it != log.end(); it++)
    {
        int seq = it->first;
        if (seq <= val)
        {
            // it->second.status = Forgotten
            log[seq] = it->second;
        }
    }
    // print_log();
    mu.unlock();
}

bool Paxos::did_majority_accept(std::vector<int> &replies)
{
    std::vector<std::string> peers_l = get_peers();
    int goal = (int)peers_l.size() / 2 + 1;
    int affirms = 0;

    for (int rep : replies)
    {
        if (rep == OK)
        {
            affirms++;
        }
    }
    return affirms >= goal;
}

std::pair<instance_t, bool> Paxos::read_slot(int key)
{
    mu.lock();
    bool found = log.find(key) != log.end();
    instance_t instance = log[key];
    mu.unlock();
    return {instance, found};
}

std::string Paxos::whoami()
{
    mu.lock();
    std::string whoiam = peers[me];
    mu.unlock();
    return whoiam;
}

void Paxos::update_max_seq(int seq)
{
    if (seq > get_max_seq())
    {
        set_max_seq(seq);
    }
}

bool Paxos::do_accept_phase(int n_p, std::vector<PrepareReply> &replies)
{
    std::vector<int> n_as;
    for (auto it = replies.begin(); it != replies.end(); it++)
    {
        if (it->res == OK)
        {
            n_as.push_back(it->n_a);
        }
    }
    int maximum = n_as.front();
    for (int n : n_as)
    {
        if (n != maximum)
        {
            return true;
        }
        maximum = std::max(maximum, n);
    }
    return maximum < n_p;
}