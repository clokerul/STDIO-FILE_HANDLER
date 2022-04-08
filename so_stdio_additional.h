#ifndef _SO_ADDITIONAL_
#define _SO_ADDITIONAL_

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MODE_APPEND 1
#define MODE_READ   2
#define MODE_WRITE  3
#define MODE_RDPLUS 4
#define MODE_APPLUS 5
#define MODE_WRPLUS 6

#define BUFFER_SIZE 4097

struct _so_file {
    char filename[20];
    char pathname[100];
    char *buffer_write;
    char *buffer_read;
    int cursor;
    int mode;
    int last_instr;

    int fd;
    int buffer_write_cursor, buffer_write_current_size;
    int buffer_read_cursor, buffer_read_current_size;

    int f_eof;
    int f_error;
};

#endif