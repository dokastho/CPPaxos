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
        seed = 0;
        seq = 0;
        err = OK;
        memset(this, 0, sizeof(PaxosOp));
    }

    template <typename T>
    PaxosOp(int seq, T datum, size_t len, int seed)
    {
        memset(this, 0, sizeof(PaxosOp));
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
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
    PaxosOp v;
};

struct PrepareReply
{
    int res;
    int seq;
    int n_a;
    int max_seq;
    int id_index;
    int max_done;
    int err;
    PaxosOp v_a;
};

struct AcceptArgs
{
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
    PaxosOp v;
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
    int seq;
    int n;
    int max_seq;
    int id_index;
    int max_done;
    PaxosOp v;
};

struct DecidedReply
{
    int res;
    int n;
    int max_seq;
    int id_index;
    int max_done;
    int err;
    PaxosOp v;
};

#endif