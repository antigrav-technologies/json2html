#ifndef _JSON2HTML_XML_BUILDER
#define _JSON2HTML_XML_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_types.h"
#include "string_buffer.h"

void write_tag(string_buffer* buf, JSONObject* object, int indent) {
    append_to_string_buffer(buf, "\n");
    for (int i = 0; i < indent; i++) {
        append_to_string_buffer(buf, "    ");
    }
    append_to_string_buffer(buf, "<");

    JSONEntry* inner = NULL;
    unsigned char first = 1;
    for (size_t i = 0; i < object->data.dictionary.size; i++) {
        append_to_string_buffer(buf, object->data.dictionary.entries[i]->key);
        if (strcmp(object->data.dictionary.entries[i]->key, "$")) {
            inner = object->data.dictionary.entries[i];
            continue;
        }
        if (object->data.dictionary.entries[i]->value->is_dictionary) {
            fprintf(stderr, "XML Arguments must be string, number or bool, not dictionary.\n");
            exit(1042);
        }
        if (!strcmp("false", object->data.dictionary.entries[i]->key)) {
            if (first) first = 0;
            else append_to_string_buffer(buf, " ");
            if (utf8_contains_space(object->data.dictionary.entries[i]->key)) append_to_string_buffer(buf, "\"");
            append_to_string_buffer(buf, object->data.dictionary.entries[i]->key);
            if (utf8_contains_space(object->data.dictionary.entries[i]->key)) append_to_string_buffer(buf, "\"");
            if (!strcmp("true", object->data.dictionary.entries[i]->value->data.string)) {
                append_to_string_buffer(buf, "=");
                append_to_string_buffer(buf, object->data.dictionary.entries[i]->value->data.string);
            }
        }
    }

    if (inner == NULL) {
        append_to_string_buffer(buf, "/>");
    }
    else {
        append_to_string_buffer(buf, ">\n");
        if (inner->value->is_dictionary) write_tag(buf, inner->value, indent + 1);
        else append_to_string_buffer(buf, inner->value->data.string);
        append_to_string_buffer(buf, "\n");
        for (int i = 0; i < indent; i++) {
            append_to_string_buffer(buf, "    ");
        }
        append_to_string_buffer(buf, "</");
        append_to_string_buffer(buf, inner->key);
        append_to_string_buffer(buf, ">");
    }
}

string_buffer* build_html(JSONObject* object) {
    string_buffer* buf = make_string_buffer();
    if (!object->is_dictionary) {
        fprintf(stderr, "Can't build HTML because it's not a dictionary\n");
        exit(101);
    }
    
    write_tag(buf, object, 0);
    return buf;
}

#endif