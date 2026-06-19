#include "response.hpp"


using namespace std;

Headers getDefaultHeaders(int contentLen) {
    Headers h;
    h["Content-Length"] = {to_string(contentLen)};
    h["Connection"] = {"close"};
    h["Content-Type"] = {"text/plain"};
    return h;
}

int writeStatusLine(Writer* w, StatusCode code) {
    if (w->state != EXPECT_STATUS) {
        cerr << "SERVER ERROR: writeStatusLine called out of order!" << endl;
        return -1;
    }
    w->state = EXPECT_HEADERS; 
    
    string line = "HTTP/1.1 " + to_string(code) + " ";
    if (code == OK) line += "OK";
    else if (code == BAD_REQUEST) line += "Bad Request";
    else if (code == INTERNAL_SERVER_ERROR) line += "Internal Server Error";
    line += "\r\n";
    
    return write(w->client_fd, line.c_str(), line.length());
}

int writeHeaders(Writer* w, const Headers& headers) {
    if (w->state != EXPECT_HEADERS) {
        cerr << "SERVER ERROR: writeHeaders called out of order!" << endl;
        return -1;
    }
    w->state = EXPECT_BODY; 

    string out = "";
    for (const auto& pair : headers) {
        for (const auto& val : pair.second) {
            out += pair.first + ": " + val + "\r\n";
        }
    }
    out += "\r\n"; 
    
    return write(w->client_fd, out.c_str(), out.length());
}

int writeBody(Writer* w, const string& body) {
    if (w->state != EXPECT_BODY) {
        cerr << "SERVER ERROR: writeBody called out of order!" << endl;
        return -1;
    }
    w->state = DONE_WRITING;
    
    return write(w->client_fd, body.c_str(), body.length());
}

int writeChunkedBody(Writer* w, const char* p, size_t len) {
    if (len == 0) return 0;

    stringstream ss;
    ss << hex << len;
    string hex_len = ss.str() + "\r\n";
    
    write(w->client_fd, hex_len.c_str(), hex_len.length());
    write(w->client_fd, p, len);
    write(w->client_fd, "\r\n", 2);

    w->state = EXPECT_BODY; 
    return len;
}

int writeChunkedBodyDone(Writer* w) {
    string final_chunk = "0\r\n\r\n";
    w->state = DONE_WRITING;
    return write(w->client_fd, final_chunk.c_str(), final_chunk.length());
}