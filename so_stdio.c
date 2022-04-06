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
            mode_flag = MODE_RDAPP;
        } else {  
            mode_flag = MODE_RDWR;
        }
    }

    // OPEN THE FILE ACCORDINGLY

    file->mode = mode_flag;
    switch(mode_flag) {
        case MODE_READ:
            file->fd = open(pathname, O_RDONLY);

            if (file->fd == -1) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_WRITE:
            file->fd = open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if (file->fd == -1) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_APPEND:
            file->fd = open(pathname, O_APPEND | O_WRONLY | O_CREAT, 0644);

            if (file->fd == -1) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_RDWR:
            if (mode[0] == 'w') {
                file->fd = open(pathname, O_RDWR | O_TRUNC | O_CREAT, 0644);
            } else {
                file->fd = open(pathname, O_RDWR | O_CREAT, 0644);
            }

            if (file->fd == -1) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_RDAPP:
            file->fd = open(pathname, O_APPEND | O_RDWR | O_CREAT, 0644);

            if (file->fd == -1) {
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

FUNC_DECL_PREFIX int so_fseek(SO_FILE *stream, long offset, int whence);
FUNC_DECL_PREFIX long so_ftell(SO_FILE *stream);


FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream) {
    int c = 0, ret = 0;
    if (stream == NULL) return SO_EOF;

    if (stream->buffer_read[stream->buffer_read_cursor] == '\0' || stream->buffer_read_cursor == BUFFER_SIZE - 1) {
        if (stream->buffer_read_cursor == BUFFER_SIZE - 1) {
            stream->buffer_read_cursor = 0;
        }
        if ((ret = read(stream->fd, stream->buffer_read + stream->buffer_read_cursor, BUFFER_SIZE - 1 - stream->buffer_read_cursor)) <= 0) {
            return SO_EOF;
        }
    }

    c = stream->buffer_read[stream->buffer_read_cursor] & 255;
    stream->buffer_read_cursor++;
    
    
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
        if (ret == SO_EOF) return 0;

        // Put the char in the final pointer
        c = (char) ret;
        *((char*)ptr + current) = c;
        current++;
    }
}


FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;

    if (strlen(stream->buffer_write) == BUFFER_SIZE - 2) {
        if (so_fflush(stream) == SO_EOF) return SO_EOF;
    }
    strcat(stream->buffer_write, (char*)(&c));
    stream->buffer_write_cursor++;
    
    return 0;
}

FUNC_DECL_PREFIX
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;

    unsigned int current = 0;

    // Iterating throught elements
    while (current < nmemb * size) {
        // If there's not enough space in the buffer
        so_fputc(*((char*)ptr + current), stream);
        current++;
    }

    return 0;
}

FUNC_DECL_PREFIX int so_fflush(SO_FILE *stream) {
    if (stream == NULL || stream->mode == MODE_READ) return SO_EOF;

    int target = strlen(stream->buffer_write);
    int current = 0, ret = 0;;
    
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

FUNC_DECL_PREFIX int so_feof(SO_FILE *stream);
FUNC_DECL_PREFIX int so_ferror(SO_FILE *stream);

FUNC_DECL_PREFIX SO_FILE *so_popen(const char *command, const char *type);
FUNC_DECL_PREFIX int so_pclose(SO_FILE *stream);

FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;
    so_fflush(stream);
    free(stream->buffer_write);
    free(stream->buffer_read);
    free(stream);
    return 0;
}

void print_so_descriptor(SO_FILE *stream) {
    if (stream == NULL) return;

    printf("====PRINTING DESCRIPTOR OF FILE====\n");
    printf("filename %s\npathname %s\nbuff %s\ncursor %s\nmode %d\nfd %d\n",
            stream->filename, stream->pathname, stream->buffer_write, stream->cursor, 
            stream->mode, stream->fd);
    printf("===================================\n");
}