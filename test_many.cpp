#include <iostream>
#include <cassert>
#include <vector>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

int main()
{
    const int npaxos = 5;
    const int niter = 500;
    Testing t(npaxos);

    std::cout << "Test: Many instances ..." << std::endl;

    for (int seq = 0; seq < niter; seq++)
    {
        t.pxa[rand() % npaxos]->Start(seq, (interface)(&npaxos + seq));
    }
    
    for (int seq = 0; seq < niter; seq++)
    {
        interface wanted = (interface)(&npaxos + seq);
        int count = 0;
        while (t.ndecided(seq, {wanted}) < npaxos)
        {
            if (count == 10)
            {
                std::cout << "value was not decided for seq=" << seq << std::endl;
                return 1;
            }
            
            t.Sleep(MS(100));
            count++;
        }
    }
    

    std::cout << "... Passed" << std::endl;

    return 0;
}
