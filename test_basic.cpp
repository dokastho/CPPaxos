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
    p2.Start(1, (void*)&p2);
    p3.Start(2, (void*)&p3);
    {
        auto x = p1.Status(0);
        Fate paxos_status = x.first;
        assert(paxos_status == Decided);
    }
    {
        auto x = p2.Status(0);
        Fate paxos_status = x.first;
        assert(paxos_status == Decided);
    }
    {
        auto x = p2.Status(1);
        Fate paxos_status = x.first;
        assert(paxos_status == Decided);
    }
    {
        auto x = p3.Status(0);
        Fate paxos_status = x.first;
        assert(paxos_status == Decided);
    }
    {
        auto x = p3.Status(1);
        Fate paxos_status = x.first;
        assert(paxos_status == Decided);
    }
    {
        auto x = p3.Status(2);
        Fate paxos_status = x.first;
        assert(paxos_status == Decided);
    }
    std::cout << "passed" << std::endl;
    return 0;
}
