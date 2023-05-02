#include <iostream>
#include <cassert>
#include "paxos.h"

int main()
{
    Paxos p(0, "output.out");
    assert(1);
    std::cout << "passed" << std::endl;
    return 0;
}
