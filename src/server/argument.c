#include "argument.h"
#include <stdlib.h>
#include <string.h>

static char *err = NULL;

static inline void reset_error_msg(const char *new_err);

#define ERROR(msg) ({ \
    reset_error_msg(msg); \
    return NULL; \
})

#define WARNING(msg) reset_error_msg(msg)

// Parses agruments supplied to main function.
// On error, NULL is returned.
// All arguents must be supplied.
char *parse_to_ip(char **restrict argv, unsigned argc)
{
    if (argc < 2)
        ERROR("Missing IP argument.");

    else if (argc > 2)
        WARNING("Only IP argument is required.");

    return argv[1];     // We keep it simple for now.
}

// Getter to error message-
const char *get_parse_error()
{
    return err;
}

// Resets error message.
static inline void reset_error_msg(const char *new_err)
{
    if (err != NULL)
    {
        free(err);
        err = NULL;
    }

    err = malloc(strlen(new_err) + 1);
    strcpy(err, new_err);
}
