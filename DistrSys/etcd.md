# etcd

https://www.ibm.com/cloud/learn/etcd

- etcd = /etc + distributed
- key-value database
- open source
- Raft consensus algorithm
  - elected leader,, followers.

## etcd with k8s

serves as the primary data backbone for **Kubernetes** and other distributed platforms.

## etcd vs. ZooKeeper vs. Consul

- ZooKeeper was originally created to coordinate configuration data and metadata across Apache Hadoop clusters.
- ZK older than etcd
- Consul is a service networking solution for distributed systems, the capabilities of which sit somewhere between those of etcd and the Istio service mesh for Kubernetes. (服务发现)

## etcd vs. Redis

basic functionalities are different.

**redis**:
- Redis is an in-memory data store and can function as a database, cache, or message broker. 
- Redis supports a wider variety of data types and structures than etcd and 
- Redis has much faster read/write performance.

**etcd**:
- etcd has superior fault tolerance, stronger failover and continuous data availability capabilities, 
- and, most importantly, etcd persists all stored data to disk, essentially sacrificing speed for greater reliability and guaranteed consistency. 
  
For these reasons: 
- **Redis is better suited for serving as a distributed memory caching system**
- **etcd is better suited for storing distributed system configuration information.**