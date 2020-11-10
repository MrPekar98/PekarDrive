#ifndef FS_H
#define FS_H

#ifdef  __cplusplus
extern "C"
{
#endif

short create_file(const char *file);
void delete_file(const char *file);
long write_file(const char *file, void *buffer, unsigned long size, short isappend);
void *read_file(const char *file);

#ifdef __cplusplus
}
#endif

#endif
