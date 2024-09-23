#ifndef _JSON2HTML_JSON_TYPES
#define _JSON2HTML_JSON_TYPES

#include <stdlib.h>

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

JSONObject* json_make_string(char* s) {
    JSONObject* object = json_object_create(0);
    free(object->data.string);
    object->data.string = s;
    return object;
}

#endif