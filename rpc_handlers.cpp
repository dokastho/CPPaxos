#include "paxos.h"
#include <thread>

void Paxos::paxos_rpc(Paxos *px, drpc_msg &m)
{
    OpArgs *p = (OpArgs *)m.req->args;
    OpReply *r = (OpReply *)m.rep->args;

    auto val = px->Status(p->seq);
    Fate stat = val.first;

    if (stat != Decided)
    {
        px->Start(p->seq, p->val);
        r->op = *(OpArgs *)val.second;
        return;
    }
    val = px->Status(p->seq);
    stat = val.first;
    int exp_back_ctr = 1;
    while (stat != Decided)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10 * exp_back_ctr));
        exp_back_ctr *= 2;
        val = px->Status(p->seq);
        stat = val.first;
    }

    px->Done(p->seq);

    r->op = *(OpArgs *)val.second;
}

void Paxos::Prepare(Paxos *px, drpc_msg &m)
{
    PrepareArgs *p = (PrepareArgs *)m.req->args;
    PrepareReply *r = (PrepareReply *)m.rep->args;

    px->set_sync.lock();

    px->update_peer_max(p->id_index, p->max_done);
    px->update_max_seq(p->seq);

    // default to reject
    r->res = Reject;
    r->err = OK;
    r->max_done = px->get_max_done();
    r->max_seq = px->get_max_seq();
    r->n_a = -1;
    r->id_index = px->me;
    r->seq = p->seq;

    px->mu.lock();

    instance_t datum = px->rpc_inst_init(p->seq);

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
}

void Paxos::Accept(Paxos *px, drpc_msg &m)
{
    AcceptArgs *p = (AcceptArgs *)m.req->args;
    AcceptReply *r = (AcceptReply *)m.rep->args;

    px->set_sync.lock();

    px->update_peer_max(p->id_index, p->max_done);
    px->update_max_seq(p->seq);

    r->res = Reject;
    r->err = OK;
    r->max_done = px->get_max_done();
    r->max_seq = px->get_max_seq();
    r->id_index = px->me;

    px->mu.lock();
    instance_t datum = px->rpc_inst_init(p->seq);

    if (p->n >= datum.n_p)
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
}

void Paxos::Learn(Paxos *px, drpc_msg &m)
{
    DecidedArgs *p = (DecidedArgs *)m.req->args;
    DecidedReply *r = (DecidedReply *)m.rep->args;

    px->set_sync.lock();

    px->update_peer_max(p->id_index, p->max_done);
    px->update_max_seq(p->seq);

    px->mu.lock();

    instance_t datum = px->rpc_inst_init(p->seq);

    // update log
    datum.status = Decided;
    datum.v_a = p->v;
    datum.n_a = p->n;

    datum.n_a = p->n;

    r->res = OK;
    r->err = OK;
    r->max_done = px->max_done;
    r->max_seq = px->max_seq;

    px->log[p->seq] = datum;
    px->mu.unlock();

    px->set_sync.unlock();
}

instance_t Paxos::rpc_inst_init(int seq)
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
