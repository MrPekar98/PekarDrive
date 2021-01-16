#include "balance.h"
#include <stdlib.h>

// Returns file_server with least work load.
// Temporary implementation.
struct file_server *least_load()
{
    if (table_count() == 0)
        return NULL;

    return getfs_index(0);
}
