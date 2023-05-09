#include <iostream>
#include <cassert>
#include <vector>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    const int npaxos = 5;
    Testing t(npaxos);

    std::cout << "Test: Minority proposal ignored ..." << std::endl;

    // make one die before we start
    t.pxa[0]->~Paxos();

    t.Sleep(MS(200));

    interface v = (interface)0x1;
    t.pxa[1]->Start(1, v);
    t.waitmajority(1, {v});

    Paxos *late = new Paxos(0, "output0.out", t.hosts);
    t.pxa[0] = late;
    t.pxa[0]->Start(1, (interface)0x2);

    t.waitn(1, 4, {v});

    std::cout << "... Passed" << std::endl;

    return 0;
}
