#include <iostream>
#include <cassert>
#include <vector>
#include <thread>
#include <mutex>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

#define maxseq 100
#define npaxos 3

Testing t(npaxos);
bool done = false;
std::mutex m;

void starter()
{
    int seq, j, r;
    PaxosOp v;
    for (size_t i = 0; i < maxseq; i++)
    {
        r = rand();
        seq = r % maxseq;
        j = r % npaxos;
        std::string rs = std::to_string(r);
        v = PaxosOp((int)i, rs.c_str(), rs.size(), rand());
        t.pxa[j]->Start(seq, v);
    }
    {
        m.lock();
        done = true;
        m.unlock();
    }
}

void finisher()
{
    bool is_done = false;
    int seq, i;
    Fate decided;
    do
    {
        seq = rand() % maxseq;
        i = rand() % npaxos;
        if (seq >= t.pxa[i]->Min())
        {
            auto ret = t.pxa[i]->Status(seq);
            decided = ret.first;
            if (decided == Decided)
            {
                t.pxa[i]->Done(seq);
            }
        }

        {
            m.lock();
            is_done = done;
            m.unlock();
        }
    } while (!is_done);
}

int main()
{
    std::cout << "Test: Lots of forgetting ..." << std::endl;

    std::thread s(starter);
    std::thread f(finisher);

    s.join();
    f.join();

    for (int seq = 0; seq < maxseq; seq++)
    {
        for (int i = 0; i < npaxos; i++)
        {
            int minimum = t.pxa[i]->Min();
            int maximum = t.pxa[i]->Max();
            auto val = t.pxa[i]->Status(seq);
            Fate stat = val.first;
            if (seq < minimum)
            {
                assert(stat == Forgotten);
            }

            else if (seq > maximum)
            {
                assert(stat == Pending);
            }

            else
            {
                assert(stat != Forgotten);
            }
        }
    }

    std::cout << "... Passed" << std::endl;

    return 0;
}
