typedef enum {
    JSON_DICTIONARY,
    JSON_ARRAY,
    JSON_OTHER
} JSONType;

typedef struct JSONObject JSONObject;

typedef struct {
    char* key;
    JSONObject* value;
} JSONEntry;

typedef struct JSONObject {
    JSONType type;
    union {
        struct {
            JSONEntry** entries;
            size_t size;
            size_t capacity;
        } dictionary;
        struct {
            JSONObject** elements;
            size_t size;
            size_t capacity;
        } array;

        char* string;
    } data;
} JSONObject;

void json_array_add(JSONObject* array, JSONObject* element) {
    if (array->array.size == array->array.capacity) {
        array->array.capacity *= 2;
        array->array.elements = realloc(array->elements, array->capacity * sizeof(JSONObject*));
    }
    array->array.elements[array->array.size++] = element;
}
/*
void json_array_iterate(json_array* array, void (*callback)(JSONObject*)) {
    for (size_t i = 0; i < array->size; i++) {
        callback(array->elements[i]);
    }
}

JSONObject* json_object_create(JSONType type) {
    JSONObject* object = malloc(sizeof(JSONObject));
    object->type = type;
    switch (type) {
        case JSON_ARRAY:
            object->data.array.elements = malloc(sizeof(JSONObject*));
            object->data.array.size = 0;
            object->data.array.capacity = 1;
            break;
        case JSON_DICTIONARY:
            // TODO: implement dictionary type
            break;
        case JSON_OTHER:
            object->data.string.length = 0;
            object->data.string.arr = malloc(sizeof(char));
            break;
        default:
            // пиздец
            exit(1);
    }
}*/