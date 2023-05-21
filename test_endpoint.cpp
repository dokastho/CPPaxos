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

    std::cout << "Test: Single proposer ..." << std::endl;

    drpc_client c;
    drpc_host dh{
        "localhost",
        TESTING_START_PORT + 1
    };

    PaxosOp preq("foobar", 6);
    PaxosOp prep;

    rpc_arg_wrapper req{(void*)&preq, sizeof(PaxosOp)};
    rpc_arg_wrapper rep{(void*)&prep, sizeof(PaxosOp)};

    c.Call(dh, "Paxos", &req, &rep);

    t.waitmajority(0, {preq});

    std::cout << "... Passed" << std::endl;

    return 0;
}
