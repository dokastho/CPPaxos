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
    const int start = rand() % 0xffff;
    Testing t(npaxos);
    std::vector<int> seeds;

    std::cout << "Test: Many instances ..." << std::endl;

    for (int seq = 0; seq < niter; seq++)
    {
        std::string val = std::to_string((start + seq));
        PaxosOp po = PaxosOp(val.c_str(), val.size());
        t.pxa[rand() % npaxos]->Start(seq, po);
        seeds.push_back(po.seed);
    }
    
    for (int seq = 0; seq < niter; seq++)
    {
        std::string val = std::to_string((start + seq));
        PaxosOp wanted = PaxosOp(val.c_str(), val.size());
        wanted.seed = seeds[seq];
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
