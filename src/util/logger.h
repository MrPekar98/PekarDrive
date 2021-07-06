#ifndef LOGGER_H
#define LOGGER_H

#define COMP_MASTER "PekarDrive master"
#define COMP_WORKER "PekarDrive worker"
#define COMP_CLIENT "PekarDrive client"
#define COMP_DEFAULT "PekarDrive"

enum log_t {ERROR, WARNING, MESSAGE, IGNORE};

const char *logger(enum log_t lt, const char *component, const char *message);

#endif
