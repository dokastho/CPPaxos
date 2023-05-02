#ifndef PAXOS_H
#define PAXOS_H

#include <string>

#include "rpcs.h"
#include "Logger.h"

#define nil ""

typedef std::string error;

class Paxos
{
private:
    Logger *logger;

public:
    Paxos(std::string);
    error Prepare(PrepareArgs *, PrepareReply *);
    error Accept(AcceptArgs *, AcceptReply *);
    error Learn(DecidedArgs *, DecidedReply *);
};

#endif