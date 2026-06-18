#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include "../headers/headers.hpp" 

using namespace std;

enum Errors{
    BAD_START_LINE,
    INC_START_LINE,
    UNSUPPORTED_HTTP_VERSION ,
    BODY_LEN_EXCEEDED
};
struct RequestLine {
    string HttpVersion;
    string RequestTarget;
    string Method;
};  

enum ParseState {
    INIT,
    PARSING_HEADERS,
    PARSING_BODY,
    DONE
};

struct Request {
    RequestLine requestLine;
    Headers headers;         
    string body;
    ParseState state = INIT;
};

int parseRequestLine(const string& buf, RequestLine* rl, int* err);

int parseSingle(Request* req, const string& unparsed_buf, int* err);

int parse(Request* req, const string& unparsed_buf, int* err);

Request* RequestFromReader(FILE* reader, int* err, int chunks = 8);

#endif