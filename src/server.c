#include <stdio.h>
#include "fs/fs.h"
#include <comm.h>
#include "server_table.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // Maintain a collection of records that tells some information about each file serber.
    // This information includes location, what files they have, and a measure of how much data they store.
    // This collection is then used to determine where to route a request.
    // Implement locking so that no two clients access the same file at once. No shared locks.

    return 0;
}
