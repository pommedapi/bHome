bHome
=====
bHome is a home automation project, using a Raspberry Pi as master (central server, orchestrator), and multiple Arduinos or DIO by Chacon power plugs as nodes.

For now, this project is more a proof of concept than a usable system.

The reason I started this project is because I wanted to have a cheap, simple yet configurable home automation system.
The commercial systems are way to expensive, or consists of simple on/off systems.


Pour ce que j'en ai compris pas de programmation à la main de chaque arduinos :
  - les noeud (arduinos) on une programmation générique de tout les modules supportés par bHome(est-ce que ça tiens dans l'ATmega????)
  - les noeud vont avoir en mémoire une déclaration des modules connectés sur ces derniers.
  - le noeud va avoir en mémoire (EEPROM??? un peu juste pour ça à mon goût) les scénarios qu'il doit appliquer.
 
Les noeuds sont donc relativement indépendants du Pi, le Pi servant de chef d'orchestre dans la mesure où c'est à partir de celui-ci que l'on pourra "reconfigurer" les scénarios des différents noeuds.

pommy - Il me semble que cette programmation générique risque d'être compliqués à mettre en place en ne s'appuyant que sur les spec d'un ATmega. L'espace disponible est particulièrement limité. Prévoir éventuellement un moyen de stockage supplémentaire plus conséquent (qui permettrais la reconfiguration de l'ATmega OTA [_Oh putain ça c'est une feature mega cool!!!_] et des scénarios nolimit&tm


Why?
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

| PREAMBLE | START SYMBOL | MESSAGE LENGTH | HOME ADDRESS | EMITTER ADDRESS | RECEIVER ADDRESS | COMMAND TYPE | DATA       | REMAINING PACKETS | FCS       |
|----------|--------------|----------------|--------------|-----------------|------------------|--------------|------------|-------------------|-----------|
|          | 0xb38        | (4-30)         |   0-255      | 0 or 1024-65545 |   0-65536        |              | max = 20b  |                   |           |
| 4 bytes  | 12 bits      | 1 byte         |   1 byte     | 2 bytes         |   2 bytes        | 1 byte       | n Bytes    | 1 byte            | 2 bytes   |




Physical layer
------
bHome relies on UHF 433MHz frequencies. The project relies on the VirtualWire implementation.

| PREAMBLE | START SYMBOL | MESSAGE LENGTH |  PAYLOAD  | FCS       |
|----------|--------------|----------------|-----------|-----------|
|          | 0xb38        | (4-30)         | max = 27b |           |
| 4 bytes  | 12 bits      | 1 byte         | n Bytes   | 2 bytes   |


Data link layer
------

Each "home" is identified by an 1 byte identifier (255 homes max)
Each node is identified by a 2 bytes identifier (65535 nodes max, 0 reserved for master, currently hard coded, in the future we plan to provide a mean to pair devices in a network).
The nodes 1 to 1023 are reserved for grouping purpose.

| HOME ADDRESS | EMITTER ADDRESS | RECEIVER ADDRESS |  PAYLOAD  | REMAINING PACKETS | 
|--------------|-----------------|------------------|-----------|-------------------|
|   0-255      | 0 or 1024-65545 |   0-65536        | max = 21b |                   |
|   1 byte     | 2 bytes         |   2 bytes        | n Bytes   | 1 byte            |
 
Application layer
-------

| COMMAND TYPE | DATA       |
|--------------|------------|
|              | max = 20b  |
| 1 byte       | n Bytes    |

TYPE OF COMMAND

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
