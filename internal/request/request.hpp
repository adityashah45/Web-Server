#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <vector>
#include <iostream>
#include <string>

using namespace std;

#define BAD_START_LINE 1
#define SEPARATOR "\r\n"
#define INC_START_LINE 2
#define UNSUPPORTED_HTTP_VERSION 3

typedef struct {
    string HttpVersion;
    string RequestTarget;
    string Method;
} RequestLine;  

typedef struct{
    RequestLine requestLine;
} Request;

RequestLine* parseRequestLine(string b,int *err);

Request* RequestFromReader(FILE* reader,int* err);

#endif