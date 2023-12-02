# rpl-udp

A simple RPL network with UDP communication. This is a self-contained example:
it includes a DAG root (`udp-server.c`) and DAG nodes (`udp-clients.c`).
This example runs without a border router -- this is a stand-alone RPL network.

A simple string containing some sensor data is transmitted from the client to the server.