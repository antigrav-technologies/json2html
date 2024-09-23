#ifndef _JSON2HTML_JSON_READER
#define _JSON2HTML_JSON_READER

#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "json_types.h"

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
    fprintf(stderr, "%s: line %d column %d (char %lld)\n", msg, line, col, pos);
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

char* decode_string(char* s, size_t* idx, unsigned char in_brackets) {
    size_t begin = *idx - 1;
    char* s_ = malloc(sizeofutf8(s) - *idx + 1 + in_brackets);
    if (in_brackets) s_[0] = '"';
    size_t s_idx = (size_t)in_brackets;

    size_t start_idx = *idx;
    while (1) {
        if ((unsigned char)s[*idx] < ' ')
            error("Unterminated string literal starting at", s, start_idx, 6);
        if (s[*idx] == '"') {
            (*idx)++;
            if (in_brackets) {
                s_[s_idx] = '"';
                s_[s_idx + 1] = '\0';
            }
            else {
                s_[s_idx] = '\0';
            }
            s_ = realloc(s_, s_idx + 1 + in_brackets);
            return s_;
        }
        if (s[*idx] == '\\') {
            (*idx)++;
            switch (s[*idx]) {
                case 'u':
                    int uni = decode_uXXXX(s, *idx);
                    size_t uni_begin = *idx + 1;
                    *idx += 5;
                    if (0xd800 <= uni && uni <= 0xdbff && s[*idx] == '\\' && s[*idx] == 'u') {
                        int uni2 = decode_uXXXX(s, *idx + 1);
                        if (0xdc00 <= uni2 && uni2 <= 0xdfff) {
                            uni = 0x10000 + (((uni - 0xd800) << 10) | (uni2 - 0xdc00));
                            *idx += 6;
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
                        error("Invalid \\uXXXX escape sequence codepoint at", s, uni_begin, 8);
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
                    s[s_idx++] = '\t';
                    break;
                default:
                    error("Invalid \\ escape at", s, *idx, 9);
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
            error("Can't parse character at", s, begin, 6);
        }
    }
}

void skip_whitespace(char* s, size_t* idx) {
    while (*idx <= strlen(s) && (s[*idx] == ' ' || s[*idx] == '\r' || s[*idx] == '\n' || s[*idx] == '\t')) {
        (*idx)++;
    }
}

JSONObject* read_json(char* s, size_t* idx);

JSONObject* decode_object(char* s, size_t* idx) {
    JSONObject* dict = json_object_create(1);
    skip_whitespace(s, idx);
    if (*idx <= strlen(s) && s[*idx] == '}') {
        (*idx)++;
        return dict;
    }
    while (1) {
        skip_whitespace(s, idx);
        if (*idx >= strlen(s) || s[*idx] != '"') error("Expecting string at", s, *idx, 11);
        (*idx)++;
        char* key = decode_string(s, idx, 0);
        skip_whitespace(s, idx);
        if (s[*idx] != ':') error("Expecting ':' delimiter at", s, *idx, 12);
        (*idx)++;
        skip_whitespace(s, idx);
        JSONObject* value = read_json(s, idx);
        json_dictionary_add(dict, key, value);
        skip_whitespace(s, idx);
        if (*idx < strlen(s) && s[*idx] == '}') {
            (*idx)++;
            return dict;
        }
        if (s[*idx] != ',') error("Expecting ',' delimiter at", s, *idx, 13);
        (*idx)++;
    }
}

size_t number_match(char* s, size_t* idx) {
    size_t newIdx = *idx;

    while ('0' <= s[newIdx] && s[newIdx] <= '9') newIdx++;
    if (newIdx == *idx) return newIdx;

    if (s[newIdx] == '.') {
        newIdx++;
        while ('0' <= s[newIdx] && s[newIdx] <= '9') newIdx++;
    }

    if (s[newIdx] == 'e' || s[newIdx] == 'E') {
        newIdx++;
        if (s[newIdx] == '+' || s[newIdx] == '-') newIdx++;
        while ('0' <= s[newIdx] && s[newIdx] <= '9') newIdx++;
    }

    return newIdx;
}

JSONObject* read_json(char* s, size_t* idx) {
    if (*idx >= sizeofutf8(s)) error("Expecting value", s, *idx, 10);
    char nextchar = s[*idx];
    if (nextchar == '"') {
        (*idx)++;
        return json_make_string(decode_string(s, idx, 1));
    }
    if (nextchar == '{') {
        (*idx)++;
        return decode_object(s, idx);
    }
    if (nextchar == '[') {
        error("Arrays aren't supported", s, *idx, 11);
    }
    if (nextchar == 't' && (strncmp(s + *idx + 1, "rue", 3) == 0)) {
        *idx += 4;
        return json_make_string("true");
    }
    if (nextchar == 'f' && (strncmp(s + *idx + 1, "alse", 4) == 0)) {
        *idx += 5;
        return json_make_string("false");
    }
    if (nextchar == 'N' && (strncmp(s + *idx + 1, "aN", 2) == 0)) {
        *idx += 3;
        return json_make_string("NaN");
    }
    if (nextchar == 'I' && (strncmp(s + *idx + 1, "nfinity", 7) == 0)) {
        *idx += 8;
        return json_make_string("Infinity");
    }
    if (nextchar == '-' && (strncmp(s + *idx + 1, "Infinity", 8) == 0)) {
        *idx += 9;
        return json_make_string("-Infinity");
    }
    size_t newIdx = number_match(s, idx);
    if (newIdx != *idx) {
        char* substring = malloc(newIdx - *idx + 1);
        strncpy(substring, s + *idx, newIdx - *idx);
        substring[newIdx - *idx + 1] = '\0';
        *idx = newIdx;
        return json_make_string(substring);
    }
    error("Expecting value", s, *idx, 14);
}

#endif