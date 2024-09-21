#include <stdio.h>
#include "json_reader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No input file name provided.\n");
        fprintf(stderr, "Usage: ./json2html <json_file> <output_file> (optional)\n");
        exit(100);
    }
    if (argc < 3) {
        fprintf(stderr, "No output file name provided. Defaulting to output.html\n");
    }
    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "File cannot be opened or doesn't exist.\n");
        exit(100);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed.");
        exit(100);
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        free(buffer);
        fclose(file);
        
        fprintf(stderr, "Reading file failed.");
        exit(100);
    }

    buffer[bytes_read] = '\0';
    fclose(file);

    size_t idx = 0;
    read_json(buffer, &idx);

    free(buffer);
}