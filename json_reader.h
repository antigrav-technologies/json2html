#include <stdlib.h>
#include <string.h>

typedef struct JSONObject JSONObject;

// Defines a JSON entry, which consists of a key and a value
typedef struct {
    char* key;
    JSONObject* value;
} JSONEntry;

// Defines a JSON object, which can be either a dictionary or a string
typedef struct JSONObject {
    // Indicates whether the object is a dictionary (1) or a string (0)
    unsigned char is_dictionary;
    union {
        struct {
            // An array of JSON entries pointers for the dictionary
            JSONEntry** entries;
            // The current number of entries in the dictionary
            size_t size;
            // The maximum number of entries the dictionary can hold without reallocation
            size_t capacity;
        } dictionary;
        // The string value of the object (if it's not a dictionary)
        char* string;
    } data;
} JSONObject;

// Adds a key-value pair to a JSON dictionary object.
void json_dictionary_add(JSONObject* object, char* key, JSONObject* value) {
    // Check if the dictionary needs to be resized.
    if (object->data.dictionary.size == object->data.dictionary.capacity) {
        object->data.dictionary.capacity *= 2;
        JSONEntry** new_entries = realloc(object->data.dictionary.entries, object->data.dictionary.capacity * sizeof(JSONEntry*));
        if (new_entries == NULL) {
            puts("Failed to reallocate memory for JSON Object (Dictionary).");
            exit(1);
        }
        object->data.dictionary.entries = new_entries;
    }

    // Create a new JSON entry.
    JSONEntry* entry = malloc(sizeof(JSONEntry));
    if (entry == NULL) {
        puts("Failed to allocate memory for JSON Object Entry.");
        exit(2);
    }

    entry->key = key;
    entry->value = value;
    object->data.dictionary.entries[object->data.dictionary.size++] = entry;
}

// Creates a new JSON object.
JSONObject* json_object_create(unsigned char is_dictionary) {
    // Allocate memory for the JSON object.
    JSONObject* object = malloc(sizeof(JSONObject));
    if (object == NULL) {
        puts("Failed to allocate memory for JSON Object.");
        exit(3);
    }

    object->is_dictionary = is_dictionary;

    // If the object is a dictionary, allocate memory for the entries array.
    if (is_dictionary) {
        object->data.dictionary.entries = malloc(sizeof(JSONEntry*));
        if (object->data.dictionary.entries == NULL) {
            puts("Failed to allocate memory for JSON Object (Dictionary).");
            exit(4);
        }
        object->data.dictionary.size = 0;
        object->data.dictionary.capacity = 1;
    }
    // If the object is not a dictionary, allocate memory for the string value.
    else {
        object->data.string = malloc(sizeof(char));
        if (object->data.string == NULL) {
            puts("Failed to allocate memory for JSON Object (Not dictionary).");
            exit(5);
        }
    }

    return object;
}

void error(char* msg, char* doc, size_t pos, int err_code) {
    int line = 1;
    int col = 1;
    for (size_t i = 0; i < pos; i++) {
        if (doc[i] == '\n') {
            line++;
            col = 1;
        }
        else {
            col++;
        }
    }
    printf("%s: line %d column %d (char %lld)\n", msg, line, col, pos);
    exit(err_code);
}

int decode_uXXXX(char* s, size_t idx) {
    char esc[5];
    for (int i = 0; i < 4; i++) {
        esc[i] = s[idx + i + 1];
    }
    esc[4] = '\0';
    int codepoint;
    if (sscanf(esc, "%4x", &codepoint) == 1) {
        return codepoint;
    }
    error("Invalid \\uXXXX escape", s, idx, 7);
}

char* decode_string(char* s, int* idx) {
    size_t begin = *idx - 1;
    char* s_ = malloc(strlen(s) + 1);
    int s_idx = 0;

    size_t start_idx = *idx;
    while (1) {
        if (s[*idx] < ' ')
            error("Unterminated string literal starting at:", s, start_idx, 6);
        if (s[*idx] == '"') {
            idx++;
            s_[s_idx] = '\0';
            s_ = realloc(s_, s_idx + 1);
            return s_; 
        }
        if (s[*idx] == '\\') {
            idx++;
            switch (s[*idx]) {
                case 'u':
                    int uni = decode_uXXXX(s, *idx);
                    size_t uni_begin = *idx + 1;
                    idx += 5;
                    if (0xd800 <= uni && uni <= 0xdbff && s[*idx] == '\\' && s[*idx] == 'u') {
                        int uni2 = decode_uXXXX(s, *idx + 1);
                        if (0xdc00 <= uni2 && uni2 <= 0xdfff) {
                            uni = 0x10000 + (((uni - 0xd800) << 10) | (uni2 - 0xdc00));
                            idx += 6;
                        }
                    }
                    if (uni <= 0x7F) {
                        s_[s_idx++] = uni;
                    }
                    else if (uni <= 0x7FF) {
                        s_[s_idx++] = 0xC0 | (uni >> 6);
                        s_[s_idx++] = 0x80 | (uni & 0x3F);
                    }
                    else if (uni <= 0xFFFF) {
                        s_[s_idx++] = 0xE0 | (uni >> 12);
                        s_[s_idx++] = 0x80 | ((uni >> 6) & 0x3F);
                        s_[s_idx++] = 0x80 | (uni & 0x3F);
                    }
                    else if (uni <= 0x10FFFF) {
                        s_[s_idx++] = 0xF0 | (uni >> 18);
                        s_[s_idx++] = 0x80 | ((uni >> 12) & 0x3F);
                        s_[s_idx++] = 0x80 | ((uni >> 6) & 0x3F);
                        s_[s_idx++] = 0x80 | (uni & 0x3F);
                    }
                    else {
                        error("Invalid \\uXXXX escape sequence codepoint at:", s, uni_begin, 8);
                    }
                    break;
                case '"':
                    (*idx)++;
                    s[s_idx++] = '"';
                    break;
                case '\\':
                    (*idx)++;
                    s[s_idx++] = '\\';
                    break;
                case '/':
                    (*idx)++;
                    s[s_idx++] = '/';
                    break;
                case 'b':
                    (*idx)++;
                    s[s_idx++] = '\b';
                    break;
                case 'f':
                    (*idx)++;
                    s[s_idx++] = '\f';
                    break;
                case 'n':
                    (*idx)++;
                    s[s_idx++] = '\n';
                    break;
                case 'r':
                    (*idx)++;
                    s[s_idx++] = '\r';
                    break;
                case 't':
                    (*idx)++;
                    s[s_idx++] = 't';
                    break;
                default:
                    error("Invalid \\ escape at:", s, *idx, 9);
            }
        }
        else if ((s[*idx] & 0x80) == 0) {
            s_[s_idx++] = s[(*idx)++];
        }
        else if ((s[*idx] & 0xE0) == 0xC0) {
            s_[s_idx++] = s[(*idx)++];
            s_[s_idx++] = s[(*idx)++];
        }
        else if ((s[*idx] & 0xF0) == 0xE0) {
            s_[s_idx++] = s[(*idx)++];
            s_[s_idx++] = s[(*idx)++];
            s_[s_idx++] = s[(*idx)++];
        }
        else if ((s[*idx] & 0xF8) == 0xF0) {
            s_[s_idx++] = s[(*idx)++];
            s_[s_idx++] = s[(*idx)++];
            s_[s_idx++] = s[(*idx)++];
            s_[s_idx++] = s[(*idx)++];
        }
        else {
            error("Can't parse character at:", s, begin, 6);
        }
    }
}


JSONObject* read_json() {
    size_t idx = 0;
    return json_object_create(0);
}