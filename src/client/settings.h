#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef DEBUG
#define CONFIG_FILE "config"
#else
#define CONFIG_FILE "~/.pekar_drive/config"
#endif

void set_master_host(const char *host);
void set_master_port(unsigned short port);
const char *get_master_host();
unsigned short get_master_port();

#endif
