#include <stdio.h>
#include "so_stdio.h"
#include "so_stdio_additional.h"

int main() {
    int a;
    SO_FILE *file = so_fopen("file.txt", "r");
    char str[100];str[1] = 0;
    so_fread(str, sizeof(char), 1, file);
    printf("%s\n", str); 
    print_so_descriptor(file);
    
    so_fclose(file);
    return 0;
}