#ifndef _SO_ADDITIONAL_
#define _SO_ADDITIONAL_

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MODE_APPEND 1
#define MODE_READ   2
#define MODE_WRITE  3
#define MODE_RDWR   4
#define MODE_RDAPP  5

#define BUFFER_SIZE 4097

struct _so_file {
    char filename[20];
    char pathname[100];
    char *buffer_write;
    char *buffer_read;
    char *cursor;
    int mode;
    int fd;
    int buffer_write_cursor;
    int buffer_read_cursor;
};

#endif