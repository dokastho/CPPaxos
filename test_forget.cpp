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

    std::cout << "Test: Forgetting ..." << std::endl;

    Testing t(npaxos);

    interface val0 = (interface)0x0;
    interface val1 = (interface)0x1;
    interface val2 = (interface)0x2;
    interface val6 = (interface)0x6;
    interface val7 = (interface)0x7;

    t.pxa[0]->Start(0, val0);
    t.pxa[1]->Start(1, val1);
    t.pxa[2]->Start(2, val2);
    t.pxa[0]->Start(0, val6);
    t.pxa[1]->Start(1, val7);

    t.waitn(0, npaxos, {val0});

    // Min() correct?
    for (int i = 0; i < npaxos; i++)
    {
        int minimum = t.pxa[i]->Min();
        if (minimum != 0)
        {
            std::cout << "wrong Min() " << minimum << "; expected 0" << std::endl;
            return 1;
        }
    }

    t.waitn(1, npaxos, {val1});

    // Min() correct?
    for (int i = 0; i < npaxos; i++)
    {
        int minimum = t.pxa[i]->Min();
        if (minimum != 0)
        {
            std::cout << "wrong Min() " << minimum << "; expected 0" << std::endl;
            return 1;
        }
    }

    // everyone Done() -> Min() changes?
    for (int i = 0; i < npaxos; i++)
    {
        t.pxa[i]->Done(0);
    }
    for (int i = 0; i < npaxos; i++)
    {
        t.pxa[i]->Done(1);
    }
    for (int i = 0; i < npaxos; i++)
    {
        t.pxa[i]->Start(8 + i, (interface)0x9);
    }

    bool all_ok = false;
    for (size_t i = 0; i < 5; i++)
    {
        all_ok = true;
        for (int j = 0; j < npaxos; j++)
        {
            int minimum = t.pxa[j]->Min();
            if (minimum != 2)
            {
                all_ok = false;
            }
        }
        if (all_ok)
        {
            break;
        }
        t.Sleep(S(1));
    }
    if (!all_ok)
    {
        std::cout << "Min() did not advance after Done()" << std::endl;
        return 1;
    }

    std::cout << "... Passed" << std::endl;
    return 0;
}
