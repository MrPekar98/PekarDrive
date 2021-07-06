#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char *log_msg = NULL;

// Prototypes.
static inline const char *log_error(const char *component, const char *message);
static inline const char *log_warning(const char *component, const char *message);
static inline const char *log_message(const char *component, const char *message);
static inline const char *log_ignore(const char *component, const char *message);

// Logs log message of given type to stdout, except for type IGNORE.
// The function also returns the log message.
const char *logger(enum log_t lt, const char *component, const char *message)
{
    if (log_msg == NULL)
        log_msg = malloc(1000);

    switch (lt)
    {
        case ERROR:
            return log_error(component, message);

        case WARNING:
            return log_warning(component, message);

        case MESSAGE:
            return log_message(component, message);

        case IGNORE:
            return log_ignore(component, message);

        default:
            return "PekarDrive: Invalid logging type.";
    }
}

static inline const char *log_error(const char *component, const char *message)
{
    const char *msg = log_ignore(component, message);
    char *error = malloc(strlen(msg) + 10);

    sprintf(error, "\033[31m%s\033[0m", msg);
    printf("%s\n", error);
    free(error);

    return msg;
}

static inline const char *log_warning(const char *component, const char *message)
{
    const char *msg = log_ignore(component, message);
    char *warning = malloc(strlen(msg) + 10);

    sprintf(warning, "\033[33m%s\033[0m", msg);
    printf("%s\n", warning);
    free(warning);

    return msg;
}

static inline const char *log_message(const char *component, const char *message)
{
    const char *msg = log_ignore(component, message);
    printf("%s\n", msg);
    return msg;
}

static inline const char *log_ignore(const char *component, const char *message)
{
    sprintf(log_msg, "%s: %s", component, message);
    return log_msg;
}
