# SimpleKVServer
A Simple Key-Value Server Based on Thread Pool

## Introduction


## Compile
g++ simplekvsvr.cpp socket.cpp threadpool.cpp -pthread -o simplekvsvr

## Usage
./simplekvsvr storefile

The server will be listening on port 7777.

P.S. The default directory is /home/ianhua/homework which can be modified in simplekvsvr.cpp.