#include "request.hpp"

int parseRequestLine(const string& buf, RequestLine* rl, int *err) {
    vector<string> parts;
    string part = "";
    
    int consumed = 0;
    bool found = false;

    for (int i = 0; i < buf.size(); i++) {
        char c = buf[i];
        if (c == '\r' && i + 1 < buf.size() && buf[i+1] == '\n') {
            found = true;
            consumed = i + 2; 
            break; 
        }
        if (c == ' ') {
            if (!part.empty()) {
                parts.push_back(part);
                part = "";
            }
        } 
        else {
            part += c;
        }
    }    

    if (!found) {
        *err = 0;
        return 0; 
    }
    if (!part.empty()) {
        parts.push_back(part);
    }
    if (parts.size() != 3) {
        *err = INC_START_LINE;
        return -1;
    }

    for (char c : parts[0]) {
        if (!(c >= 'A' && c <= 'Z')) {
            *err = BAD_START_LINE;
            return -1;
        }
    }

    if (parts[2] != "HTTP/1.1") {
        *err = UNSUPPORTED_HTTP_VERSION;
        return -1;
    }
    rl->Method = parts[0];
    rl->RequestTarget = parts[1];
    rl->HttpVersion = parts[2].substr(5); 

    return consumed;
}