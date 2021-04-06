#ifndef PACKET_H
#define PACKET_H

struct packet
{
    unsigned seq_number;
    char *arg;
    unsigned msg_len;
    unsigned short error;
    unsigned long token;

    enum type
    {
        PING,
        LS,
        WRITE,
        READ,
        APPEND,
        REGISTER,
        DELETE
    } msg_type;
};

#ifdef __cplusplus
extern "C"
{
#endif
struct packet p_init(unsigned seq_number, const char *arg, enum type t);
void attach_token(unsigned long token, struct packet *restrict p);
const void *p_encode(struct packet p);
struct packet p_decode(const void *p);
void p_cleanup(struct packet p);
struct packet p_error(unsigned seq_number, const char *arg, enum type t, unsigned long token);
#ifdef __cplusplus
}
#endif

#endif
