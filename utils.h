#ifndef _JSON2HTML_UTILS
#define _JSON2HTML_UTILS

size_t sizeofutf8(char* s) {
    size_t counter = 0;
    while (1) {
        if (s[counter] == '\0') return counter;
        else if ((s[counter] & 0x80) == 0) counter++;
        else if ((s[counter] & 0xE0) == 0xC0) counter += 2;
        else if ((s[counter] & 0xF0) == 0xE0) counter += 3;
        else if ((s[counter] & 0xF8) == 0xF0) counter += 4;
        else return 69420; // should never run beause there is a check for this in json_reader.h
    }
}

size_t roundup(size_t n) {
    size_t result = 1;
    while (result < n) {
        result <<= 1;
    }
    return result;
}

#endif