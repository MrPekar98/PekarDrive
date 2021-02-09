# PekarDrive
This is a simple distributed storage system. The system is made of a single master with multiple workers. The workers are responsible for carrying out file operatins, such as writing, reading and deleting files. The master is responsible for load balancing. A client is included and used to initiate operations on the master. An archive file is found in the /include folder that can be used as an API to the master.

The general architecture of PekarDrive is seen in the figure below:

![PekarDrive Architecture](https://github.com/MrPekar98/PekarDrive/blob/main/images/architecture.png)

## Fault tolerance
Fault tolerance is achieved by letting the master write checkpoints of itself periodically. Furthermore, the master pings the workers every minute. There is no failure detection implemented to detect master failure, though having the workers detect master failure if ping message has not been received for over a minute could be implemented.

## Guide
This section describes with examples how to use the PekarDrive interface as well as the terminal application.

### Terminal

The PekarDrive terminal application comes with 5 operations:

- **ls**: List all contents stored on all worker nodes.
- **read**: Read all contents of a given file.
- **write**: Write a new file.
- **append**: Append to an existing file.
- **delete**: Delete a given file.

List all contents of every file in every node with the following command:

`pekardrive ls`

Read contents of a file with the following command:

`pekardrive read <FILENAME>`

Write to a file using the following command:

`pekardrive write <FILENAME> <DATA>`

Append to an existing file with the following command:

`pekardrive append <FILENAME> <DATA>`

Delete a file with the following command:

`pekardrive delete <FILENAME>`

### PekarDrive library

The library comes with three headers used to interact with a master node. The header `interface.h` contains everything needed to interact with a master node. The header comes with the following declarations:

`const void *ls(const char *host, unsigned short port)`

`const void *file_read(const char *file_name, const char *host, unsigned short port)`

`size_t file_write(const char *file_name, const void *buffer, size_t len, short append, const char *host, unsigned short port)`

`short file_delete(const char *file_name, const char *host, unsigned short port)`
