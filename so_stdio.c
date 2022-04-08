#include "so_stdio.h"
#include "so_stdio_additional.h"

FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode) {
    SO_FILE* file;
    int mode_flag;

    file = (SO_FILE*) calloc(1, sizeof(SO_FILE));
    if (file == NULL) exit(-1);

    if (mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a') {
        free(file);
        return NULL;
    }

    // PROCESS MODE
    switch(mode[0]) {
        case 'r':
            mode_flag = MODE_READ;
            break;
        case 'w':
            mode_flag = MODE_WRITE;
            break;
        case 'a':
            mode_flag = MODE_APPEND;
            break;
    }
    if (mode[1] == '+') {
        if (mode[0] == 'a') {
            mode_flag = MODE_APPLUS;
        } else if (mode[0] == 'w'){  
            mode_flag = MODE_WRPLUS;
        } else {
            mode_flag = MODE_RDPLUS;
        }
    }

    // OPEN THE FILE ACCORDINGLY

    file->mode = mode_flag;
    file->f_eof = 0;
    file->f_error = 0;
    file->buffer_write_current_size = 0;
    file->buffer_write_cursor = 0;
    file->buffer_read_current_size = 0;
    file->last_instr = -1;
    switch(mode_flag) {
        case MODE_READ:
            file->fd = open(pathname, O_RDONLY);
            
            if (file->fd == -1) {
                file->f_error = 1;
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_WRITE:
            file->fd = open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if (file->fd == -1) {
                file->f_error = 1;
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_APPEND:
            file->fd = open(pathname, O_APPEND | O_WRONLY | O_CREAT, 0644);

            if (file->fd == -1) {
                file->f_error = 1;
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_WRPLUS:
            file->fd = open(pathname, O_RDWR | O_TRUNC | O_CREAT, 0644);            

            if (file->fd == -1) {
                file->f_error = 1;
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_APPLUS:
            file->fd = open(pathname, O_APPEND | O_RDWR | O_CREAT, 0644);

            if (file->fd == -1) {
                file->f_error = 1;
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_RDPLUS:
            file->fd = open(pathname, O_RDWR | O_CREAT, 0644);

            if (file->fd == -1) {
                file->f_error = 1;
                so_fclose(file);
                return NULL;
            }
            break;
    }
    file->buffer_write = (char*) calloc (BUFFER_SIZE, sizeof(char));
    if (file->buffer_write == NULL) exit(-1);
    file->buffer_read = (char*) calloc (BUFFER_SIZE, sizeof(char));
    if (file->buffer_read == NULL) exit(-1);
    strcpy(file->pathname, pathname);

    return file;
}

FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream) {
    int c = 0, ret = 0;
    if (stream == NULL) return SO_EOF;

    if (stream->buffer_read_cursor == stream->buffer_read_current_size || stream->buffer_read_cursor == BUFFER_SIZE - 1) {
        // Reset buff
        if (stream->buffer_read_cursor == BUFFER_SIZE - 1) {
            stream->buffer_read_cursor = 0;
            stream->buffer_read_current_size = 0;
        }

        // Read again
        if ((ret = read(stream->fd, stream->buffer_read + stream->buffer_read_current_size, BUFFER_SIZE - 1 - stream->buffer_read_current_size)) <= 0) {
            if (ret == 0) stream->f_eof = 1;
            return SO_EOF;
        }

        // Set new buffer read size
        stream->buffer_read_current_size += ret;
    }

    c = stream->buffer_read[stream->buffer_read_cursor] & 255;
    stream->buffer_read_cursor++;
    stream->cursor++;
    stream->last_instr = MODE_READ;
    
    return c;
}

FUNC_DECL_PREFIX
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    long target = size * nmemb, current = 0;
    int ret = 0;
    char c = 0;

    while (current < target) {
        ret = so_fgetc(stream);
        //printf("%d ", ret);
        if (ret == SO_EOF) {
            stream->f_error = 1;
            return current;
        }

        // Put the char in the final pointer
        c = (char) ret;
        *((char*)ptr + current) = c;
        current++;
    }

    return nmemb;
}

FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;

    if (stream->buffer_write_cursor == BUFFER_SIZE - 1) {
        if (so_fflush(stream) == SO_EOF) return SO_EOF;
    }
    stream->buffer_write[stream->buffer_write_cursor] = (char)c;
    stream->buffer_write_cursor++;
    stream->cursor++;
    stream->last_instr = MODE_WRITE;
    
    return c & 255;
}

FUNC_DECL_PREFIX
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;

    long current = 0;
    long target = nmemb * (long)size;

    // Iterating throught elements
    while (current < target) {
        // If there's not enough space in the buffer
        stream->ITER = current;
        if (so_fputc(*((char*)ptr + current), stream) == SO_EOF) {
            stream->f_error = 1;
            return 0;
            }
        current++;
    }

    return nmemb;
}

FUNC_DECL_PREFIX int so_fflush(SO_FILE *stream) {
    if (stream == NULL || stream->mode == MODE_READ) return SO_EOF;

    int target = stream->buffer_write_cursor;
    int current = 0, ret = 0;

    while (current < target) {
        ret = write(stream->fd, stream->buffer_write + current, target - current);
        if (ret <= 0) {
            return SO_EOF;
        }
        current += ret;
    }
    stream->buffer_write_cursor = 0;
    memset(stream->buffer_write, 0, BUFFER_SIZE);
    return 0;
}

#if defined(__linux__)
FUNC_DECL_PREFIX int so_fileno(SO_FILE *stream) {
    return stream->fd;
}
#elif defined(_WIN32)
FUNC_DECL_PREFIX HANDLE so_fileno(SO_FILE *stream);
#else
#error "Unknown platform"
#endif




FUNC_DECL_PREFIX long so_ftell(SO_FILE *stream) {
    return stream->cursor;
}

FUNC_DECL_PREFIX int so_feof(SO_FILE *stream) {
    if (stream->f_eof == 1) return 1;
    return 0;
}

FUNC_DECL_PREFIX int so_ferror(SO_FILE *stream) {
    return stream->f_error;
}

// TODO
FUNC_DECL_PREFIX SO_FILE *so_popen(const char *command, const char *type) {
    return NULL;
}
FUNC_DECL_PREFIX int so_pclose(SO_FILE *stream) {
    return -1;
}
FUNC_DECL_PREFIX int so_fseek(SO_FILE *stream, long offset, int whence) {
    if (stream->last_instr == MODE_WRITE) {
        if (so_fflush(stream) == SO_EOF) return -1;
    } else if (stream->last_instr == MODE_READ) {
        stream->buffer_read_cursor = 0;
        stream->buffer_read_current_size = 0;
    }

    stream->cursor = lseek(stream->fd, offset, whence);
    if (stream->cursor == -1) return -1;
    return 0;
}

FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;

    // Returning flush value only for append and write
    int ret;
    if (stream->last_instr != MODE_WRITE) ret = 0;
    else ret = so_fflush(stream);

    free(stream->buffer_write);
    free(stream->buffer_read);
    free(stream);
    return ret;
}

void print_so_descriptor(SO_FILE *stream) {
    if (stream == NULL) return;

    printf("====PRINTING DESCRIPTOR OF FILE====\n");
    printf("filename %s\npathname %s\nbuff %s\ncursor %d\nmode %d\nfd %d\n",
            stream->filename, stream->pathname, stream->buffer_write, stream->cursor, 
            stream->mode, stream->fd);
    printf("===================================\n");
}