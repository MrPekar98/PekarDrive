#include <pthread.h>
#include "worker_admin.h"

// Stores information about workers.
// Load balances workload based in worker information in server table.
// When client wants to read a file, find the file amongst the workers. This is also true when appending to a file.
int main()
{
    pthread_t admin = start_admin();

    while (1)
    {

    }

    return 0;
}
