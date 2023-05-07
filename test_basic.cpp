#include <iostream>
#include <cassert>
#include <vector>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    const int npaxos = 3;
    Testing t(3);

    std::cout << "Test: Single proposer ..." << std::endl;

    interface val1 = (interface)"hello";
    t.pxa[0]->Start(0, val1);
    t.waitn(0, npaxos, {val1});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Many proposers, same value ..." << std::endl;

    interface val2 = (interface)77;
    for (int i = 0; i < npaxos; i++)
    {
        t.pxa[i]->Start(1, val2);
    }
    t.waitn(1, npaxos, {val2});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Out-of-order instances ..." << std::endl;

    t.pxa[0]->Start(7, (interface)700);
	t.pxa[0]->Start(6, (interface)600);
	t.pxa[1]->Start(5, (interface)500);
	t.waitn(7, npaxos, {(interface)700});
	t.pxa[0]->Start(4, (interface)400);
	t.pxa[1]->Start(3, (interface)300);
	t.waitn(6, npaxos, {(interface)600});
	t.waitn(5, npaxos, {(interface)500});
	t.waitn(4, npaxos, {(interface)400});
	t.waitn(3, npaxos, {(interface)300});

    int max_seq = t.pxa[0]->Max();
    if (max_seq != 7) {
		std::cout << "wrong Max()" << std::endl;
        return 1;
	}

    std::cout << "... Passed" << std::endl;

    return 0;
}
