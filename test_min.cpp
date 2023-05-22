#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    const int npaxos = 6;

    std::cout << "Test: min ..." << std::endl;

    Testing t(npaxos);

    PaxosOp val0 = PaxosOp("0x0", 3, rand());
    PaxosOp val1 = PaxosOp("0x1", 3, rand());
    PaxosOp val2 = PaxosOp("0x2", 3, rand());
    PaxosOp val6 = PaxosOp("0x6", 3, rand());
    PaxosOp val7 = PaxosOp("0x7", 3, rand());

    t.pxa[0]->Start(0, val0);
    t.pxa[1]->Start(1, val1);
    t.pxa[2]->Start(2, val2);
    t.pxa[0]->Start(0, val6);
    t.pxa[1]->Start(1, val7);

    t.waitn(0, npaxos, {val0});

    for (int i = 0; i < npaxos; i++)
    {
        int minimum = t.pxa[i]->Min();
        if (minimum != 0)
        {
            std::cout << "wrong Min() " << minimum << "; expected 0" << std::endl;
            return 1;
        }
    }

    std::cout << "... Passed" << std::endl;
    return 0;
}
