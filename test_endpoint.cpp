#include <iostream>
#include <cassert>
#include <vector>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    const int npaxos = 3;
    Testing t(npaxos);
    drpc_client c;

    {
        std::cout << "Test: Paxos from endpoint ..." << std::endl;

        drpc_host dh{
            "localhost",
            TESTING_START_PORT + 1};

        PaxosOp preq(0, "foobar", 6, rand());
        PaxosOp prep;

        rpc_arg_wrapper req{(void *)&preq, sizeof(PaxosOp)};
        rpc_arg_wrapper rep{(void *)&prep, sizeof(PaxosOp)};

        c.Call(dh, "Paxos", &req, &rep);

        t.waitmajority(0, {preq});
    }
    std::cout << "... Passed" << std::endl;

    {
        std::cout << "Test: Lots of paxos from endpoint ..." << std::endl;

        size_t count = 100;

        for (size_t i = 1; i < count; i++)
        {
            drpc_host dh{
                "localhost",
                (short)(TESTING_START_PORT + (rand() % npaxos))};

            PaxosOp preq((int)i, "foobar", 6, rand());
            PaxosOp prep;

            rpc_arg_wrapper req{(void *)&preq, sizeof(PaxosOp)};
            rpc_arg_wrapper rep{(void *)&prep, sizeof(PaxosOp)};

            c.Call(dh, "Paxos", &req, &rep);

            t.waitmajority((int)i, {preq});
        }
    }

    std::cout << "... Passed" << std::endl;

    return 0;
}
