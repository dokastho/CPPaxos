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

    PaxosOp val1 = PaxosOp("hello", strlen("hello"));
    t.pxa[0]->Start(0, val1);
    t.waitn(0, npaxos, {val1});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Many proposers, same value ..." << std::endl;

    PaxosOp val2 = PaxosOp("077", 3);
    for (int i = 0; i < npaxos; i++)
    {
        t.pxa[i]->Start(1, val2);
    }
    t.waitn(1, npaxos, {val2});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Out-of-order instances ..." << std::endl;

    t.pxa[0]->Start(7, PaxosOp("700", 3));
	t.pxa[0]->Start(6, PaxosOp("600", 3));
	t.pxa[1]->Start(5, PaxosOp("500", 3));
	t.waitn(7, npaxos, {PaxosOp("700", 3)});
	t.pxa[0]->Start(4, PaxosOp("400", 3));
	t.pxa[1]->Start(3, PaxosOp("300", 3));
	t.waitn(6, npaxos, {PaxosOp("600", 3)});
	t.waitn(5, npaxos, {PaxosOp("500", 3)});
	t.waitn(4, npaxos, {PaxosOp("400", 3)});
	t.waitn(3, npaxos, {PaxosOp("300", 3)});

    int max_seq = t.pxa[0]->Max();
    if (max_seq != 7) {
		std::cout << "wrong Max()" << std::endl;
        return 1;
	}

    std::cout << "... Passed" << std::endl;

    return 0;
}
