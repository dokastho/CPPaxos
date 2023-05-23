#include <vector>
#include <string>
#include <iostream>

#include "drpc.h"
#include "paxos.h"

#define PAXOS_START_PORT 8124
#define NPAXOS 3

class PaxosGroup
{
public:
    std::vector<drpc_host> hosts;
    std::vector<Paxos *> pxa;
    PaxosGroup(const int npaxos)
    {
        for (int i = 0; i < npaxos; i++)
        {
            drpc_host h{"localhost", (short)(PAXOS_START_PORT + i)};
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

    ~PaxosGroup()
    {
        for (size_t i = 0; i < pxa.size(); i++)
        {
            delete pxa[i];
        }
    }
};

int main()
{
    PaxosGroup servers(NPAXOS);
    std::string cmd = "";
    do
    {
        std::cout << "enter 'q' to quit\n$ ";
        std::cin >> cmd;
    } while (cmd[0] != 'q');
}
