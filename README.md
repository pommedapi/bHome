bHome
=====
bHome is a home automation project, using a Raspberry Pi as master (central server, orchestrator), and multiple Arduinos or DIO by Chacon power plugs as nodes.

For now, this project is more a proof of concept than a usable system.

The reason I started this project is because I wanted to have a cheap, simple yet configurable home automation system.
The commercial systems are way to expensive, or consists of simple on/off systems.


Why
======
I wanted to have a home automation fora while, and after some researches on the Internet, I couldn't find something that _really_ suits my needs.

Communication
=====
bHome relies on radio (433MHz), and uses the VirtualWire library to communicate.

It also uses the Home Easy protocol to send data to the Chacon plugs (special thanks to Idleman [check his blog on http://blog.idleman.fr] who, with his work, made that possible).


Architecture
=====
bHome is architectured like this:
 * A Raspberry Pi, acting as central server.
 * An Arduino linked to the Pi through I2C (the Pi is the master), and has 433MHz transmitters and receivers.
 * Multiple (253 max) Arduinos having 433MHz transmitters and receivers, and anything else you want!
 * Multiple Chacon plugs.

Protocol
=====
The master is the only one which can start a communication, the nodes can only answer (ACK or "real" answer).

The master can send packets to a single receiver, or to everyone (broadcast).

As previously said, the nodes can send an ACK to the master, or answer back with real data (like sensor status).

I intend to add the group-broadcasting feature in the future, allowing to send packets to all the nodes of a room for instance.


To do list (unsorted list)
=====
 * Create the PHP backend
 * Create the SQL database
 * Create the daemon
 * Add a group-broadcasting feature
 * Write some documentation
 * Clean and optimize the code


Changelog
=====
 * 04/01/2014 v0.0.1
  * I2C communication works both ways (from the Pi to the Arduino and conversely).
  * RF communication works both ways (from the master to a node and conversely).


Contributors
=====
Feel free to fork this repository and improve the code, any pull-request will be welcome.
