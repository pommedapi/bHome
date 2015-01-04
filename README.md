bHome
=====
bHome is a home automation project, using a Raspberry Pi as master (central server, orchestrator), and multiple Arduinos or DIO by Chacon power plugs as nodes.
For now, this project is more a proof of concept than a usable system.
The reason I started this project is because I wanted to have a cheap, simple yet configurable home automation system.
The commercial systems are way to expensive, or consists of simple on/off systems.

Communication
=====
bHome relies on radio (433.92MHz), and uses the VirtualWire library to communicate.
It also uses the Home Easy protocol to send data to the Chacon plugs.

Architecture
=====
bHome is architectured like this:
 * A Raspberry Pi, acting as central server.
 * An Arduino linked to the Pi through I2C (the Pi is the master), and has 433.92MHz transmitters and receivers.
 * Multiple (253 max) Arduinos having 433.92MHz transmitters and receivers, and anything else you want!
 * Multiple Chacon plugs.

Protocol
=====
The master is the only one which can start a communication, the nodes can only answer (ACK or "real" answer).
The master can send packets to a single receiver, or to everyone (broadcast).
As previously said, the nodes can send an ACK to the master, or answer back with real data (like sensor status).
I intend to add the group-broadcasting feature in the future, allowing to send packets to all the nodes of a room for instance.
