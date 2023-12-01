#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "drpc.h"
#include "paxos.h"
#include "test_helpers.h"

#define nclients 2
#define npaxos 3
#define nrequests 250

Testing t(npaxos);

void run_client(int me)
{
    int seq = 0;
    for (size_t i = 0; i < nrequests; i++)
    {
        std::stringstream ss;
        PaxosOp v{};
        v.seq = seq;
        v.seed = rand();
        v.err = 0;
        memset(v.data, 0xff, PAXOS_OP_SIZE);

        PaxosOp reply{};
        reply.err = Err;
        rpc_arg_wrapper req;
        rpc_arg_wrapper rep;

        req.args = &v;
        req.len = sizeof(PaxosOp);
        rep.args = &reply;
        rep.len = sizeof(PaxosOp);

        drpc_host h = t.hosts[me];

        while (reply.seed != v.seed)
        {
            v.seq = seq;
            reply.err = Err;
            while (reply.err == Err)
            {
                drpc_client drpc_agent;
                int err = drpc_agent.Call(h, "Paxos", &req, &rep);
                if (err == -1)
                {
                    break;
                }
            }
            seq++;
        }
    }
}

int main()
{
    std::cout << "emulated client service" << std::endl;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < nclients; i++)
    {
        std::thread t(&run_client, i);
        threads.push_back(std::move(t));
    }
    for (size_t i = 0; i < nclients; i++)
    {
        threads[i].join();
    }
    std::cout << "passed" << std::endl;
}