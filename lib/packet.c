#include "packet.h"
#include <string.h>
#include <stdlib.h>

// Initializes packet struct.
struct packet p_init(unsigned seq_number, const char *arg, enum type msg_type)
{
    struct packet p = {.seq_number = seq_number, .arg = malloc(strlen(arg) + 1),
                        .msg_len = strlen(arg), .msg_type = msg_type};
    strcpy(p.arg, arg);
    return p;
}

// Encodes struct packet into string format.
const void *p_encode(struct packet p)
{
    void *buffer = malloc(p.msg_len + 1 + sizeof(int) * 3);
    memcpy(buffer, &p.seq_number, sizeof(unsigned));
    memcpy(buffer + sizeof(unsigned), &p.msg_len, sizeof(unsigned));
    memcpy(buffer + sizeof(int) * 2, &p.msg_type, sizeof(int));
    memcpy(buffer + sizeof(int) * 3, p.arg, p.msg_len);

    return buffer;
}

// Decodes string into struct packet.
struct packet p_decode(const void *p)
{
    struct packet res;
    memcpy(&res.seq_number, p, sizeof(unsigned));
    memcpy(&res.msg_len, p + sizeof(unsigned), sizeof(unsigned));
    memcpy(&res.msg_type, p + sizeof(unsigned) * 2, sizeof(unsigned));

    res.arg = malloc(res.msg_len);
    memcpy(res.arg, p + sizeof(unsigned) * 3, res.msg_len);

    return res;
}

// Cleans up heap memory.
void p_cleanup(struct packet p)
{
    free(p.arg);
}
