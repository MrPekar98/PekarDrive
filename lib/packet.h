#ifndef PACKET_H
#define PACKET_H

struct packet
{
    unsigned seq_number;
    char *arg;
    unsigned msg_len;
    unsigned short error;

    enum type
    {
        PING,
        LS,
        WRITE,
        READ,
        APPEND
    } msg_type;
};

const void *p_encode(struct packet p);
struct packet p_decode(const void *p);
struct packet p_init(unsigned seq_number, const char *arg, enum type t);
void p_cleanup(struct packet p);
struct packet p_error(unsigned seq_number, const char *arg, enum type t);

#endif
