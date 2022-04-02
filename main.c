#include <stdio.h>
#include "so_stdio.h"

int main() {
    int a;
    SO_FILE *file = so_fopen("asd.txt", "w");
    so_fputc((int)'b', file);
    return 0;
}