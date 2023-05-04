#include "paxos.h"

error Paxos::Prepare(Paxos *px, rpc_arg_wrapper *args, rpc_arg_wrapper *reply)
{
    PrepareArgs *p = (PrepareArgs *)args->args;
    PrepareReply *r = (PrepareReply *)reply->args;

    px->set_sync.lock();

    px->update_peer_max(p->id_index, p->max_done);
    px->update_max_seq(p->seq);

    // default to reject
    r->res = Reject;
    r->max_done = px->get_max_done();
    r->max_seq = px->get_max_seq();
    r->n_a = -1;
    r->id_index = px->me;
    r->valid = true;
    r->seq = p->seq;

    px->mu.lock();

    instance_t datum = px->rpc_inst_init(p->seq, true);

    if (p->n > datum.n_p)
    {
        if (p->n > px->max_n)
        {
            px->max_n = p->n;
        }
        // reply with accept
        r->res = OK;
        datum.n_p = p->n;

        r->n_a = datum.n_a;
        r->v_a = datum.v_a;
    }

    px->log[p->seq] = datum;
    px->mu.unlock();

    px->set_sync.unlock();
    return nil;
}

error Paxos::Accept(Paxos *px, rpc_arg_wrapper *args, rpc_arg_wrapper *reply)
{
    AcceptArgs *p = (AcceptArgs *)args->args;
    AcceptReply *r = (AcceptReply *)reply->args;

    px->set_sync.lock();

    px->update_peer_max(p->id_index, p->max_done);
    px->update_max_seq(p->seq);

    r->res = Reject;
    r->max_done = px->get_max_done();
    r->max_seq = px->get_max_seq();
    r->id_index = px->me;

    px->mu.lock();
    instance_t datum = px->rpc_inst_init(p->seq, false);

    if (p->n > datum.n_p)
    {
        // update state
        datum.n_a = p->n;
        datum.n_p = p->n;

        // set values in reply
        r->n = p->n;
        r->res = OK;
    }

    px->log[p->seq] = datum;
    px->mu.unlock();

    px->set_sync.unlock();
    return nil;
}

error Paxos::Learn(Paxos *px, rpc_arg_wrapper *args, rpc_arg_wrapper *reply)
{
    DecidedArgs *p = (DecidedArgs *)args->args;
    DecidedReply *r = (DecidedReply *)reply->args;

    px->set_sync.lock();

    px->update_peer_max(p->id_index, p->max_done);
    px->update_max_seq(p->seq);

    px->mu.lock();

    instance_t datum = px->rpc_inst_init(p->seq, false);

    // update log
    datum.status = Decided;
    datum.v_a = p->v;
    datum.n_a = p->n;

    datum.n_a = p->n;

    r->res = OK;
    r->max_done = px->max_done;
    r->max_seq = px->max_seq;

    px->log[p->seq] = datum;
    px->mu.unlock();

    px->set_sync.unlock();
    return nil;
}

instance_t Paxos::rpc_inst_init(int seq, bool p_phase)
{
    instance_t datum{
        Pending,
        -1,
        -1,
        nullptr,
    };

    bool exists = log.find(seq) != log.end();

    if (exists)
    {
        datum = log[seq];
    }
    return datum;
}
