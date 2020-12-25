#include "file_exec.h"
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Entrypoint for handling file operations.
struct file_output f_exec(short op, const void *arg, size_t len)
{
    pthread_mutex_lock(&mutex);

    if (op & FILE_READ)
    {

    }

    if (op & FILE_WRITE)
    {

    }

    pthread_mutex_unlock(&mutex);
}

// Returns a list of files names.
struct file_list file_list()
{
    pthread_mutex_lock(&mutex);

    pthread_mutex_unlock(&mutex);
}
