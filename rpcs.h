#ifndef PAXOS_RPCS_H
#define PAXOS_RPCS_H

#define OK 0
#define Reject 1
#define Err 2

struct OpArgs
{
    int seq;
    int seed;
    interface val;
};

struct OpReply
{
    OpArgs op;
};

struct PrepareArgs
{
    int seq;
    int n;
    void *v;
    int max_seq;
    int id_index;
    int max_done;
};

struct PrepareReply
{
    int res;
    int seq;
    int n_a;
    void* v_a;
    int max_seq;
    int id_index;
    int max_done;
    int err;
};

struct AcceptArgs
{
    int seq;
    int n;
    void *v;
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
    int seq;
    int n;
    void *v;
    int max_seq;
    int id_index;
    int max_done;
};

struct DecidedReply
{
    int res;
    int n;
    void *v;
    int max_seq;
    int id_index;
    int max_done;
    int err;
};

#endif