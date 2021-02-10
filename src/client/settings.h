#ifndef SETTINGS_H
#define SETTINGS_H

#define CONFIG_FILE "~/.pekar_drive/config"

void set_master_host(const char *host);
void set_master_port(unsigned short port);
const char *get_master_host();
unsigned short get_master_port();

#endif
