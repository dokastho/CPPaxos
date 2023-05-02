#include "paxos.h"

Paxos::Paxos(std::string log_filename)
{
    logger = new Logger(log_filename);
}
