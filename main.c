#include <stdio.h>
#include "json_reader.h"

int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        printf("argv[%i] = %s\n", i, argv[i]);
        for (int j = 0; argv[i][j] != '\0'; j++) {
            printf("argv[%i][%i] = %i\n", i, j, argv[i][j]);
        }
    }
    return 0;
}