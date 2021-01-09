#ifndef FS_H
#define FS_H

#ifdef  __cplusplus
extern "C"
{
#endif

short fs_create_file(const char *file);
void fs_delete_file(const char *file);
long fs_write_file(const char *file, const void *buffer, unsigned long size, short isappend);
void *fs_read_file(const char *file);

#ifdef __cplusplus
}
#endif

#endif
