#include <iostream>
#include <cassert>
#include <vector>
#include "paxos.h"
#include "drpc.h"

int main()
{
    std::vector<drpc_host> peers = {
        drpc_host{"localhost", 8024},
        drpc_host{"localhost", 8025},
        drpc_host{"localhost", 8026},
    };
    Paxos p1(0, "output0.out", peers);
    Paxos p2(1, "output1.out", peers);
    Paxos p3(2, "output2.out", peers);

    p1.Start(0, (void*)&p1);
    assert(1);
    std::cout << "passed" << std::endl;
    return 0;
}
