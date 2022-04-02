#include "so_stdio.h"

FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode) {
    SO_FILE* file = (SO_FILE*) calloc(1, sizeof(SO_FILE));
    int mode_flag;

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
        }        
        mode_flag = MODE_RDWR;
    }

    // OPEN THE FILE ACCORDINGLY
    switch(mode_flag) {
        case MODE_READ:
            file->fd = open(pathname, O_RDONLY);

            if (file->fd == NULL) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_WRITE:
            file->fd = open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if (file->fd == NULL) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_APPEND:
            file->fd = open(pathname, O_APPEND | O_TRUNC | O_CREAT, 0644);

            if (file->fd == NULL) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_RDWR:
            file->fd = open(pathname, O_RDWR | O_TRUNC | O_CREAT, 0644);

            if (file->fd == NULL) {
                so_fclose(file);
                return NULL;
            }
            break;
        case MODE_RDAPP:
            file->fd = open(pathname, O_APPEND | O_RDWR | O_TRUNC | O_CREAT, 0644);

            if (file->fd == NULL) {
                so_fclose(file);
                return NULL;
            }
            break;
    }
    file->mode = mode_flag;
    strcpy(file->pathname, pathname);
}

FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream) {
    if (stream == NULL) return SO_EOF;

    free(stream->buff);
    free(stream);
    return 0;
}

FUNC_DECL_PREFIX int so_fflush(SO_FILE *stream);

FUNC_DECL_PREFIX int so_fseek(SO_FILE *stream, long offset, int whence);
FUNC_DECL_PREFIX long so_ftell(SO_FILE *stream);

FUNC_DECL_PREFIX
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream);
FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream);

FUNC_DECL_PREFIX
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream);
FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream) {
    if (write(stream->fd, (char*)(&c), 1) != -1) {
        return c;
    }
    return SO_EOF;
}

FUNC_DECL_PREFIX int so_feof(SO_FILE *stream);
FUNC_DECL_PREFIX int so_ferror(SO_FILE *stream);

FUNC_DECL_PREFIX SO_FILE *so_popen(const char *command, const char *type);
FUNC_DECL_PREFIX int so_pclose(SO_FILE *stream);