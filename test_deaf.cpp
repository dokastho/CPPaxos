#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    const int npaxos = 5;
    Testing t(npaxos);

    std::cout << "Test: Deaf proposer ..." << std::endl;

    PaxosOp val1 = PaxosOp(1, "hello", 5, rand());
    PaxosOp val2 = PaxosOp(2, "goodbye", 7, rand());
    PaxosOp val3 = PaxosOp(3, "xxx", 3, rand());
    PaxosOp val4 = PaxosOp(4, "yyy", 3, rand());
    
    t.pxa[0]->Start(0, val1);
    t.waitn(0, npaxos, {val1});

    t.pxa[0]->Deafen();
    t.pxa[npaxos - 1]->Deafen();

    t.Sleep(MS(300));

    t.pxa[1]->Start(1, val2);
    t.waitmajority(1, {val2});
    if (t.ndecided(1, {val2}) != npaxos - 2)
    {
        std::cout << "a deaf peer heard about a decision" << std::endl;
        return 1;
    }
    
    t.pxa[0]->Start(1, val3);
    t.waitn(1, npaxos-1, {val2});
    if (t.ndecided(1, {val2}) != npaxos - 1)
    {
        std::cout << "a deaf peer heard about a decision" << std::endl;
        return 1;
    }

    t.pxa[npaxos-1]->Start(1, val4);
    t.waitn(1, npaxos, {val2});

    std::cout << "... Passed" << std::endl;
    return 0;
}
