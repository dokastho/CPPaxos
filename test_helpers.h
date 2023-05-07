#ifndef TEST_HELPERS
#define TEST_HELPERS

#include <chrono>
#include <vector>
#include <iostream>
#include <thread>
#include <sstream>
#include "paxos.h"

typedef std::chrono::milliseconds MS;
typedef std::chrono::seconds S;

class Testing
{
private:
    std::vector<drpc_host> hosts;
public:
    std::vector<Paxos *> pxa;
    Testing(const int npaxos)
    {
        for (int i = 0; i < npaxos; i++)
        {
            drpc_host h{"localhost", (short)(8024 + i)};
            hosts.push_back(h);
        }
        for (int i = 0; i < npaxos; i++)
        {
            std::stringstream ss;
            ss << "output" << i << ".out";
            Paxos *p = new Paxos(i, ss.str(), hosts);
            pxa.push_back(p);
        }
    }

    ~Testing()
    {
        for (size_t i = 0; i < pxa.size(); i++)
        {
            delete pxa[i];
        }
    }

    template<typename T>
    void Sleep(T d)
    {
        std::this_thread::sleep_for(d);
    }

    int ndecided(int seq, std::vector<interface> wantedvals)
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

    void waitn(int seq, int wanted, std::vector<interface> wantedvals)
    {
        MS to(10);
        for (size_t i = 0; i < 30; i++)
        {
            if (ndecided(seq, wantedvals) >= wanted)
            {
                break;
            }
            Sleep(to);

            // exponential backoff <1s
            if (to < S(1))
            {
                to *= 2;
            }
        }
        int nd = ndecided(seq, wantedvals);
        if (nd < wanted)
        {
            std::cout << "too few decided; seq=" << seq << " ndecided=" << nd << " wanted=" << wanted << std::endl;
            exit(1);
        }
    }

    void waitmajority(int seq, std::vector<interface> wantedvals)
    {
        waitn(seq, ((int)pxa.size() / 2) + 1, wantedvals);
    }
};

#endif