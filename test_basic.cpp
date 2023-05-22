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

    PaxosOp val1 = PaxosOp("hello", strlen("hello"), rand());
    t.pxa[0]->Start(0, val1);
    t.waitn(0, npaxos, {val1});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Many proposers, same value ..." << std::endl;

    PaxosOp val2 = PaxosOp("077", 3, rand());
    for (int i = 0; i < npaxos; i++)
    {
        t.pxa[i]->Start(1, val2);
    }
    t.waitn(1, npaxos, {val2});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Out-of-order instances ..." << std::endl;
    
    PaxosOp val3 = PaxosOp("300", 3, rand());
    PaxosOp val4 = PaxosOp("400", 3, rand());
    PaxosOp val5 = PaxosOp("500", 3, rand());
    PaxosOp val6 = PaxosOp("600", 3, rand());
    PaxosOp val7 = PaxosOp("700", 3, rand());

    t.pxa[0]->Start(7, val7);
	t.pxa[0]->Start(6, val6);
	t.pxa[1]->Start(5, val5);
	t.waitn(7, npaxos, {val7});
	t.pxa[0]->Start(4, val4);
	t.pxa[1]->Start(3, val3);
	t.waitn(6, npaxos, {val6});
	t.waitn(5, npaxos, {val5});
	t.waitn(4, npaxos, {val4});
	t.waitn(3, npaxos, {val3});

    int max_seq = t.pxa[0]->Max();
    if (max_seq != 7) {
		std::cout << "wrong Max()" << std::endl;
        return 1;
	}

    std::cout << "... Passed" << std::endl;

    return 0;
}
