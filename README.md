George Lazureanu

STDIO IMPLEMENTATION FOR FILE HANDLING

Objective:
    - create an alternative for stdio.h implementation of the file handling
    - export the code as a dinamically linked library
    - provide support for:
        -read 'r'           | including r+
        -write 'w'          | including w+
        -append 'a'         | including a+

Final product: libso_stdio.so
Driver file: so_stdio.h
Implementation file: so_stdio.c

Structures used:
    struct so_file {
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
    }

Functions implemented:
    SO_FILE *so_fopen(const char *pathname, const char *mode)
        creates a new file descriptor and opens the file accordingly
        @pathname - where to find the file in the file system
        @mode - changes the file handling flow 
            r - readonly    ('+' also writeable)
            w - writeonly   ('+' also readable)
            a - writeonly, append ('+' also readable)
        
        @return - pointer to the file descriptor

    int so_fclose(SO_FILE *stream)
        closes the file and frees memory used by the file descriptor

        @return -1 in case of error, 0 otherwise
    
    int so_fgetc(SO_FILE *stream)
        reads one character from the filestream
        @return -1 in case of error, the integer equivalent of the char read otherwise
    int so_fputc(int c, SO_FILE *stream)
        writes one char to the filestream
        @return -1 in case of error, the integer equivalent of the char writen otherwise

    size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
        Reads from file an exact amount of information
        @ptr - memory where the reading takes place to
        @size - the size of ptr
        @nmemb - number of members in ptr to be read
        @stream - file descriptor, used to read from file
        
    size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
        Writes to file an exact amount of information
        @ptr - memory where the reading takes place from
        @size - the size of ptr
        @nmemb - number of member in ptr to be read
        @stream - file descriptor, used to write to file

    int so_feof(SO_FILE *stream)
        @return 1 if the current descriptor reached eof, 0 otherwise

    int so_ferror(SO_FILE *stream)
        @return 1 if the current descriptor encountered any error, 0 otherwise

    int so_fseek(SO_FILE *stream, long offset, int whence)
        wrapper over lseek

    long so_ftell(SO_FILE *stream)
        @return -1 in case of error, the position in the file otherwise

    int so_fflush(SO_FILE *stream)
        Flushes the buffer, emptying it
        @return -1 in case of error, 0 otherwise
    
