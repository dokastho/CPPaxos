#include <iostream>
#include <cassert>
#include <string>
#include <mutex>
#include <vector>
#include "paxos.h"
#include "test_helpers.h"
#include "drpc.h"

const int npaxos = 3;
Testing t(npaxos);
drpc_client c;
int SEQ = 0;
std::mutex print_lock;

void print_atomic(std::string s)
{
    print_lock.lock();
    std::cout << s << std::endl;
    print_lock.unlock();
}

void make_paxos_request(int seq)
{
    drpc_host dh{
        "localhost",
        TESTING_START_PORT + 2};

    // advance seq until req is returned
    while (true)
    {
        PaxosOp preq(seq, "foobar", 6, rand());
        PaxosOp prep;
        prep.err = Err;

        rpc_arg_wrapper req{(void *)&preq, sizeof(PaxosOp)};
        rpc_arg_wrapper rep{(void *)&prep, sizeof(PaxosOp)};

        while (prep.err == Err)
        {
            int err = c.Call(dh, "Paxos", &req, &rep);
            if (err == -1)
            {
                break;
            }
        }

        if (prep.seed == preq.seed)
        {

            break;
        }
        // if (prep.seed == 0)
        // {
        //     print_atomic("seq was forgotten:" + std::to_string(seq));
        // }
        seq++;
    }
}

int main()
{
    {
        std::cout << "Test: Paxos from endpoint ..." << std::endl;

        drpc_host dh{
            "localhost",
            TESTING_START_PORT};

        PaxosOp preq(SEQ, "foobar", 6, rand());
        PaxosOp prep;
        prep.err = Err;

        rpc_arg_wrapper req{(void *)&preq, sizeof(PaxosOp)};
        rpc_arg_wrapper rep{(void *)&prep, sizeof(PaxosOp)};

        while (prep.err == Err)
        {
            int err = c.Call(dh, "Paxos", &req, &rep);
            if (err == -1)
            {
                break;
            }
        }

        t.waitmajority(SEQ, {preq});
        SEQ++;
    }
    std::cout << "... Passed" << std::endl;

    {
        std::cout << "Test: Lots paxos requests from endpoint ..." << std::endl;

        size_t count = 100;

        for (size_t i = 1; i < count; i++)
        {
            drpc_host dh{
                "localhost",
                (short)(TESTING_START_PORT + 1)};

            PaxosOp preq((int)i, "foobar", 6, rand());
            PaxosOp prep;
            prep.err = Err;

            rpc_arg_wrapper req{(void *)&preq, sizeof(PaxosOp)};
            rpc_arg_wrapper rep{(void *)&prep, sizeof(PaxosOp)};

            while (prep.err == Err)
            {
                int err = c.Call(dh, "Paxos", &req, &rep);
                if (err == -1)
                {
                    break;
                }
            }

            t.waitmajority((int)i, {preq});
        }
        SEQ = 100;
    }

    std::cout << "... Passed" << std::endl;

    {
        std::cout << "Test: Paxos from behind ..." << std::endl;

        make_paxos_request(0);
        SEQ++;
    }
    std::cout << "... Passed" << std::endl;

    return 0;
}
