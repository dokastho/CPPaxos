#ifndef PAXOS_RPCS_H
#define PAXOS_RPCS_H

#include <cstring>
#include <iostream>
#include <string>

#define OK 0
#define Reject 1
#define Err 2

#define PAXOS_OP_SIZE 256

typedef int Fate;

struct PaxosOp
{
    int seed;
    uint8_t data[PAXOS_OP_SIZE];

    PaxosOp()
    {
        memset(this->data, '\0', PAXOS_OP_SIZE);
    }

    template <typename T>
    PaxosOp(T datum, size_t len)
    {
        memset(this->data, '\0', PAXOS_OP_SIZE);
        memcpy(this->data, datum, len);
    }

    bool operator==(PaxosOp &rhs)
    {
        return memcmp(this->data, rhs.data, PAXOS_OP_SIZE) == 0;
    }

    bool operator!=(PaxosOp &rhs)
    {
        return memcmp(this->data, rhs.data, PAXOS_OP_SIZE) != 0;
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