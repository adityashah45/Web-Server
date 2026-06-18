#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include "../headers/headers.hpp" 

using namespace std;

#define BAD_START_LINE 1
#define INC_START_LINE 2
#define UNSUPPORTED_HTTP_VERSION 3

struct RequestLine {
    string HttpVersion;
    string RequestTarget;
    string Method;
};  

enum ParseState {
    INIT,
    PARSING_HEADERS,
    DONE
};

struct Request {
    RequestLine requestLine;
    Headers headers;         
    ParseState state = INIT;
};

int parseRequestLine(const string& buf, RequestLine* rl, int* err);

int parseSingle(Request* req, const string& unparsed_buf, int* err);

int parse(Request* req, const string& unparsed_buf, int* err);

Request* RequestFromReader(FILE* reader, int* err, int chunks = 8);

#endif