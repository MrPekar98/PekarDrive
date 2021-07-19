# PekarDrive

![Tests](https://github.com/MrPekar98/PekarDrive/actions/workflows/c.yml/badge.svg)

This is a simple distributed storage system. The system is made of a single master with multiple workers. The workers are responsible for carrying out file operatins, such as writing, reading and deleting files. The master is responsible for load balancing. A client is included and used to initiate operations on the master. An archive file is found in the /include folder that can be used as an API to the master.

The general architecture of PekarDrive is seen in the figure below:

![PekarDrive Architecture](https://github.com/MrPekar98/PekarDrive/blob/main/images/architecture.png)

## Fault Tolerance
Fault tolerance is achieved by letting the master write checkpoints of itself periodically. Furthermore, the master pings the workers every minute. There is no failure detection implemented to detect master failure, though having the workers detect master failure if ping message has not been received for over a minute could be implemented.

## Guide
This section describes with examples how to use the PekarDrive interface as well as the terminal application.

### Compiling
Run the following command to build an archive `libpekardrive.a` file in folder /lib used for linkage:

`make install`

Then, compile with the following options:

`... -l<PATH TO LIBRARY>/lib/libpekardrive.a` `-I<PATH TO LIBRARY>/INCLUDE`

### Terminal

In order to specify the address of a PekarDrive, the commands `set-ip` and `set-port` are used, as seen in the example below:

`pekar set-ip <IP>`

`pekar set-port <PORT>`

The PekarDrive terminal application comes with 5 operations:

- **ls**: List all contents stored on all worker nodes.
- **read**: Read all contents of a given file.
- **write**: Write a new file.
- **append**: Append to an existing file.
- **delete**: Delete a given file.

List all contents of every file in every node with the following command:

`pekar ls`

Read contents of a file with the following command:

`pekar read <FILENAME>`

Write to a file using the following command:

`pekar write <FILENAME> <DATA>`

Append to an existing file with the following command:

`pekar append <FILENAME> <DATA>`

Delete a file with the following command:

`pekar delete <FILENAME>`

### PekarDrive library

The library comes with three headers used to interact with a master node. The header `interface.h` contains everything needed to interact with a master node. The header comes with the following functions:

`const void *ls(const char *host, unsigned short port)`

`const void *file_read(const char *file_name, const char *host, unsigned short port)`

`size_t file_write(const char *file_name, const void *buffer, size_t len, short append, const char *host, unsigned short port)`

`short file_delete(const char *file_name, const char *host, unsigned short port)`

## Master and Worker Setup

Execute the following Makefile command to build a master and worker executable, including a client to interact with the master:

`make MASTER_TKN=<MASTER_TOKEN> WORKER_TKN=<WORKER_TOKEN>`

Tokens are required to be assigned to the master and worker. The tokens must be integer values. In order to only build a single executable, insert either `master` or `worker` after `make` to build the master or the worker executable, respectively.

Note, it is important the master does not run on the same machine as any workers. If this is needed, change the globally used port number in lib/comm.c and src/server/boot.h before building an individual component.

Also, if you have not downloaded a release, the project is compiled in debug mode. To disable debug mode and logging, remove `-DDEBUG` and set to `-DVERBOSE_1` instead of `-DVERBOSE_2` from the Makefile.
