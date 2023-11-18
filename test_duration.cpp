#include <iostream>
#include <cassert>
#include <vector>
#include <thread>
#include <chrono>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

#define npaxos 3

drpc_client c;

void make_paxos_request(int seq, int host)
{
    drpc_host dh{
        "localhost",
        (short)(TESTING_START_PORT + host)};
    
    // advance seq until req is returned
    while (true)
    {
        PaxosOp preq(seq, "foobar", 6, rand());
        PaxosOp prep;
        prep.err = Err;

        rpc_arg_wrapper req{(void *)&preq, sizeof(PaxosOp)};
        rpc_arg_wrapper rep{(void *)&prep, sizeof(PaxosOp)};

        while (prep.err == Err)
        {
            int err = c.Call(dh, "Paxos", &req, &rep);
            if (err == -1)
            {
                break;
            }
        }

        if (prep.seed == preq.seed)
        {
            break;
        }
        
        seq++;
    }
}

int main()
{
    Testing t(npaxos);

    std::cout << "Test: Many proposers over time ..." << std::endl;

    int count = 64;
    int nthread = 8;

    for (int i = 0; i < count * nthread; i += (int)nthread)
    {
        std::vector<std::thread> threads;
        for (int j = 0; j < nthread; j++)
        {
            int host = j % npaxos;
            std::thread t(make_paxos_request, i, host);
            threads.push_back(std::move(t));
        }
        for (int j = 0; j < nthread; j++)
        {
            threads[j].join();
        }
        if (i % nthread == 0)
        {
            // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    std::cout << "... Passed" << std::endl;

    return 0;
}
