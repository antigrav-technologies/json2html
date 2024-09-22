#include <stdio.h>
#include "json_reader.h"
#include "xml_builder.h"

void print_json(JSONObject* object, int indent) {
    if (object->is_dictionary) {
        printf("{\n");
        for (size_t i = 0; i < object->data.dictionary.size; i++) {
            for (int j = 0; j <= indent; j++) printf("    ");
            printf("\"%s\": ", object->data.dictionary.entries[i]->key);
            print_json(object->data.dictionary.entries[i]->value, indent + 1);
            printf("\n");
        }
        for (int j = 0; j < indent; j++) printf("    ");
        printf("}\n");
    }
    else {
        printf("%s", object->data.string);
    }
}

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
    string_buffer* buf = build_xml(read_json(buffer, &idx)); 
    FILE* fp = fopen(argc < 3 ? "output.html" : argv[2], "w");
    if (fp != NULL) {
        fwrite(buf->string, 1, buf->len, fp);
        fclose(fp);
    }
    else {
        fprintf(stderr, "Error opening output file.\n");
        exit(100);
    }
    print_json(read_json(buffer, &idx), 0);
    free(buffer);
}