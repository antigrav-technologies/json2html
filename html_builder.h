#ifndef _JSON2HTML_XML_BUILDER
#define _JSON2HTML_XML_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_types.h"
#include "string_buffer.h"

string_buffer* build_html(JSONObject* object) {
    string_buffer* buf = make_string_buffer();
    if (!object->is_dictionary) {
        fprintf(stderr, "Can't build HTML because it's not a dictionary\n");
        exit(101);
    }
    
    append_to_string_buffer(buf, "<xml>");
    return buf;
}

#endif