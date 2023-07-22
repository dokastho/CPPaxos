# CPPaxos
Paxos, rewritten in C++

## Optimizations

This Paxos API utilizes many optimizations. In no particular order:

- Skips redundant Prepare & Accept Phase RPC's once quorum is either established or ruled out
- Skips Accept Phase if all prepare phase responses are default
- Distributes requests concurrently
- Calls RPC's locally when a Paxos peer interacts with itself

Thanks to these optimizations this protocol is significantly more efficient

## Purpose

Paxos provides a linearized log of operations. Specifically, Paxos provides task scheduling as a service to distributed server applications without requiring a heirarchical server topology. In other words, the Paxos process will keep a group of identical server processes organized. 

## How it Works

It goes without saying that Paxos is not intuitive. In a nutshell, distributed server processes send requests to an assigned Paxos peer, which then "reaches out" to at least half of the peers in the Paxos group. If a simple majority of those peers "accept" the operation, then it is committed to the log. If not, they will reply with an operation that they either propose or have already accepted, and the original proposer must then proceed with that operation. Once the original proposer has committed the operation, it can then re-propose the requested operation and repeat the process. Therefore, the two possible outcomes are either a) the requested operation is commited or b) the proposing peer becomes more up-to-date with the group of Paxos peers.

For more in-depth detail on Paxos, check out the Paxos [wikipedia page](https://en.wikipedia.org/wiki/Paxos_(computer_science)).
