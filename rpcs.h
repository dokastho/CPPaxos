#ifndef PAXOS_RPCS_H
#define PAXOS_RPCS_H

#define OK "OK"
#define Reject "Reject"
#define ID_LEN 16
#define RES_LEN 16

struct PrepareArgs {
    int seq;
    int n;
    void* v;
    int max_seq;
    char identity[ID_LEN];
    int max_done;
    bool valid;
};

struct AcceptArgs {
    int seq;
    int n;
    void* v;
    int max_seq;
    char identity[ID_LEN];
    int max_done;
};

struct DecidedArgs {
    int seq;
    int n;
    void* v;
    int max_seq;
    char identity[ID_LEN];
    int max_done;
};

struct DecidedReply {
    char res[RES_LEN];
    int n;
    void* v;
    int max_seq;
    char identity[ID_LEN];
    int max_done;
};

#endif