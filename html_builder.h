#ifndef _JSON2HTML_XML_BUILDER
#define _JSON2HTML_XML_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_types.h"
#include "string_buffer.h"

void write_tag(string_buffer* buf, JSONObject* object, int indent) {
    if (object->is_dictionary) {
        for (size_t i = 0; i < object->data.dictionary.size; i++) {
            for (int j = 0; j < indent; j++) append_to_string_buffer(buf, "    ");
            append_to_string_buffer(buf, "<");
            append_to_string_buffer(buf, object->data.dictionary.entries[i]->key);
            append_to_string_buffer(buf, ">\n");
            write_tag(buf, object->data.dictionary.entries[i]->value, indent + 1);
            append_to_string_buffer(buf, "\n");
            for (int j = 0; j < indent; j++) append_to_string_buffer(buf, "    ");
            append_to_string_buffer(buf, "</");
            append_to_string_buffer(buf, object->data.dictionary.entries[i]->key);
            append_to_string_buffer(buf, ">\n");
        }
    }
    else {
        for (int j = 0; j < indent; j++) append_to_string_buffer(buf, "    ");
        append_to_string_buffer(buf, object->data.string);
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