#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../headers/headers.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h> 

enum StatusCode {
    OK = 200,
    BAD_REQUEST = 400,
    INTERNAL_SERVER_ERROR = 500
};

enum WriterState {
    EXPECT_STATUS,
    EXPECT_HEADERS,
    EXPECT_BODY,
    DONE_WRITING
};

struct Writer {
    int client_fd;
    WriterState state;
};

Headers getDefaultHeaders(int contentLen);
int writeStatusLine(Writer* w, StatusCode code);
int writeHeaders(Writer* w, const Headers& headers);
int writeBody(Writer* w, const std::string& body);

int writeChunkedBody(Writer* w, const char* p, size_t len);
int writeChunkedBodyDone(Writer* w);

#endif