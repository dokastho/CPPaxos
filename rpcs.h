#ifndef PAXOS_RPCS_H
#define PAXOS_RPCS_H

#include <cstring>
#include <iostream>
#include <string>

#define OK 0
#define Reject 1
#define Err 2

#define PAXOS_OP_SIZE 512

typedef int Fate;

struct PaxosOp
{
    int seed;
    int seq;
    int err;
    uint8_t data[PAXOS_OP_SIZE];

    PaxosOp()
    {
        memset(this->data, '\0', PAXOS_OP_SIZE);
    }

    template <typename T>
    PaxosOp(int seq, T datum, size_t len, int seed)
    {
        memset(this->data, '\0', PAXOS_OP_SIZE);
        memcpy(this->data, datum, len);
        this->seed = seed;
        this->seq = seq;
    }

    bool operator==(PaxosOp &rhs)
    {
        return this->seed == rhs.seed;
    }

    bool operator!=(PaxosOp &rhs)
    {
        return this->seed != rhs.seed;
    }
};

struct PrepareArgs
{
    PaxosOp v;
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
};

struct PrepareReply
{
    PaxosOp v_a;
    int res;
    int seq;
    int n_a;
    int max_seq;
    int id_index;
    int max_done;
    int err;
};

struct AcceptArgs
{
    PaxosOp v;
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
};

struct AcceptReply
{
    int res;
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
    int err;
};

struct DecidedArgs
{
    PaxosOp v;
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
};

struct DecidedReply
{
    PaxosOp v;
    int res;
    int n;
    int max_seq;
    int id_index;
    int max_done;
    int err;
};

#endif