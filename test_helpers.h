#ifndef TEST_HELPERS
#define TEST_HELPERS

#include <chrono>
#include <vector>
#include <iostream>
#include <thread>
#include "paxos.h"

typedef std::chrono::milliseconds ms;
typedef std::chrono::seconds s;

class Testing
{
private:
public:
    int ndecided(std::vector<Paxos*> &pxa, int seq, std::vector<interface> wantedvals)
    {
        int count = 0;
        interface v;
        for (size_t i = 0; i < pxa.size(); i++)
        {
            auto p = pxa[i]->Status(seq);
            bool decided = p.first;
            interface v1 = p.second;
            if (decided == Decided)
            {
                if (count > 0 && v != v1)
                {
                    std::cout << "decided value" << v1 << " for seq= " << seq << " at peer " << pxa[i]->whoami() << " does not match previously seen decided value " << v << std::endl;
                    exit(1);
                }
                if (!wantedvals.empty())
                {
                    bool match = false;
                    for (interface val : wantedvals)
                    {
                        if (val == v1)
                        {
                            match = true;
                            break;
                        }
                    }
                    if (!match)
                    {
                        std::cout << "decided value" << v1 << " for seq= " << seq << " at peer " << pxa[i]->whoami() << " does not match any expected value" << std::endl;
                        exit(1);
                    }
                }
                count++;
                v = v1;
            }
        }
        return count;
    }

    void waitn(std::vector<Paxos*> &pxa, int seq, int wanted, std::vector<interface> wantedvals)
    {
        ms to(10);
        for (size_t i = 0; i < 30; i++)
        {
            if (ndecided(pxa, seq, wantedvals) >= wanted)
            {
                break;
            }
            std::this_thread::sleep_for(to);

            // exponential backoff <1s
            if (to < s(1))
            {
                to *= 2;
            }
        }
        int nd = ndecided(pxa, seq, wantedvals);
        if (nd < wanted)
        {
            std::cout << "too few decided; seq=" << seq << " ndecided=" << nd << " wanted=" << wanted << std::endl;
            exit(1);
        }
    }

    void waitmajority(std::vector<Paxos*> &pxa, int seq, std::vector<interface> wantedvals)
    {
        waitn(pxa, seq, ((int)pxa.size() / 2) + 1, wantedvals);
    }
};

#endif