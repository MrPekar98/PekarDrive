# PekarDrive
This is a simple distributed storage system. The system is made of a single master with multiple workers. The workers are responsible for carrying out file operatins, such as writing, reading and deleting files. The master is responsible for load balancing. A client is included and used to initiate operations on the master. An archive file is found in the /include folder that can be used as an API to the master.

The general architecture of PekarDrive is seen in the figure below:

![PekarDrive Architecture](https://github.com/MrPekar98/PekarDrive/blob/main/images/architecture.png)

## Fault tolerance
Fault tolerance is achieved by letting the master write checkpoints of itself periodically. Furthermore, the master pings the workers every minute. There is no failure detection implemented to detect master failure, though having the workers detect master failure if ping message has not been received for over a minute could be implemented.

## Guide
