#include <iostream>
#include <cassert>
#include <vector>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    Testing t;
    std::vector<drpc_host> peers = {
        drpc_host{"localhost", 8024},
        drpc_host{"localhost", 8025},
        drpc_host{"localhost", 8026},
    };
    Paxos p1(0, "output0.out", peers);
    Paxos p2(1, "output1.out", peers);
    Paxos p3(2, "output2.out", peers);

    std::vector<Paxos*> pxa = {
        &p1,
        &p2,
        &p3
    };
    int npaxos = 3;

    std::cout << "Test: Single proposer ..." << std::endl;

    interface val1 = (interface)"hello";
    pxa[0]->Start(0, val1);
    t.waitn(pxa, 0, npaxos, {val1});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Many proposers, same value ..." << std::endl;

    interface val2 = (interface)77;
    for (int i = 0; i < npaxos; i++)
    {
        pxa[i]->Start(1, val2);
    }
    t.waitn(pxa, 1, npaxos, {val2});

    std::cout << "... Passed" << std::endl;

    std::cout << "Test: Out-of-order instances ..." << std::endl;

    pxa[0]->Start(7, (interface)700);
	pxa[0]->Start(6, (interface)600);
	pxa[1]->Start(5, (interface)500);
	t.waitn(pxa, 7, npaxos, {(interface)700});
	pxa[0]->Start(4, (interface)400);
	pxa[1]->Start(3, (interface)300);
	t.waitn(pxa, 6, npaxos, {(interface)600});
	t.waitn(pxa, 5, npaxos, {(interface)500});
	t.waitn(pxa, 4, npaxos, {(interface)400});
	t.waitn(pxa, 3, npaxos, {(interface)300});

    int max_seq = pxa[0]->Max();
    if (max_seq != 7) {
		std::cout << "wrong Max()" << std::endl;
        return 1;
	}

    std::cout << "... Passed" << std::endl;

    return 0;
}
