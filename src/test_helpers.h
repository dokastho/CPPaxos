#ifndef TEST_HELPERS
#define TEST_HELPERS

#include <chrono>
#include <vector>
#include <iostream>
#include <thread>
#include <sstream>
#include <string>
#include <mutex>
#include "paxos.h"

typedef std::chrono::milliseconds MS;
typedef std::chrono::seconds S;

#define TESTING_START_PORT 8024

// print data as concatenated numbers
std::string PaxosOp_to_string(const PaxosOp &p)
{
    std::string string_data;
    for (size_t i = 0; i < PAXOS_OP_SIZE; i++)
    {
        string_data += std::to_string(p.data[i]);
    }
    return string_data;
}

class Testing
{
private:
    std::mutex l;
public:
    std::vector<drpc_host> hosts;
    std::vector<Paxos *> pxa;
    Testing(const int npaxos)
    {
        for (int i = 0; i < npaxos; i++)
        {
            drpc_host h{"localhost", (short)(TESTING_START_PORT + i)};
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

    void print(std::string s)
    {
        l.lock();
        std::cout << s << std::endl;
        l.unlock();
    }

    template <typename T>
    void Sleep(T d)
    {
        std::this_thread::sleep_for(d);
    }

    int ndecided(int seq, std::vector<PaxosOp> wantedvals)
    {
        int count = 0;
        PaxosOp v;
        for (size_t i = 0; i < pxa.size(); i++)
        {
            auto p = pxa[i]->Status(seq);
            int decided = p.first;
            PaxosOp v1 = p.second;
            if (decided == Decided)
            {
                if (count > 0 && v != v1)
                {
                    std::cout << "decided value " << PaxosOp_to_string(v1) << " for seq=" << seq << " at peer " << pxa[i]->whoami() << " does not match previously seen decided value " << PaxosOp_to_string(v) << std::endl;
                    exit(1);
                }
                if (!wantedvals.empty())
                {
                    bool match = false;
                    for (PaxosOp val : wantedvals)
                    {
                        if (val == v1)
                        {
                            match = true;
                            break;
                        }
                    }
                    if (!match)
                    {
                        std::cout << "decided value " << PaxosOp_to_string(v1) << " for seq=" << seq << " at peer " << pxa[i]->whoami() << " does not match any expected value" << std::endl;
                        exit(1);
                    }
                }
                count++;
                v = v1;
            }
        }
        return count;
    }

    void waitn(int seq, int wanted, std::vector<PaxosOp> wantedvals)
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

    void waitmajority(int seq, std::vector<PaxosOp> wantedvals)
    {
        waitn(seq, ((int)pxa.size() / 2) + 1, wantedvals);
    }
};

#endif