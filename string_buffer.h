#ifndef _JSON2HTML_STRING_BUFFER
#define _JSON2HTML_STRING_BUFFER

#include "utils.h"

typedef struct {
    char* string;
    size_t len;
    size_t capacity;
} string_buffer;

string_buffer* make_string_buffer() {
    string_buffer* buf = malloc(sizeof(string_buffer));
    buf->string = malloc(1);
    buf->len = 0;
    buf->capacity = 1;
    return buf;
}

void resize_string_buffer(string_buffer* buf, size_t new_capacity) {
    buf->string = realloc(buf->string, new_capacity);
    if (buf->string == NULL) {
        fprintf(stderr, "Failed to allocate memory for string buffer");
        exit(73);
    }
    buf->capacity = new_capacity;
}

void append_to_string_buffer(string_buffer* buf, char* str) {
    size_t str_len = sizeofutf8(str);
    size_t new_capacity = roundup(buf->len + str_len);
    if (new_capacity > buf->capacity) resize_string_buffer(buf, new_capacity);
    memcpy(buf->string + buf->len, str, str_len);
    buf->len += str_len;
}

#endif