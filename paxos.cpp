#include <thread>

#include "paxos.h"

Paxos::Paxos(int my_index, std::string log_filename) : me(my_index)
{
    logger = new Logger(log_filename);

    drpc_host my_host{
        "localhost",
        8022
    };
    drpc_engine = new drpc_server(my_host, this);

    max_done = -1;
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
