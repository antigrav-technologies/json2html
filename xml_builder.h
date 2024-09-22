#ifndef _JSON2HTML_XML_BUILDER
#define _JSON2HTML_XML_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_reader.h"
#include "string_buffer.h"

string_buffer* build_xml(JSONObject* object) {
    string_buffer* buf = make_string_buffer();
    append_to_string_buffer(buf, "<xml>");
    return buf;
}

#endif