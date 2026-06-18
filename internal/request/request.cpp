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

int parse(Request* req, const string& unparsed_data, int* err) {
    if (req->state == INIT) {
        int consumed = parseRequestLine(unparsed_data, &(req->requestLine), err);
        if (consumed > 0 && *err == 0) {
            req->state = DONE;
        }
        return consumed;
    }
    return 0; 
}
Request* RequestFromReader(FILE* reader, int* err, int chunks) {
    Request* req = new Request();
    string unparsed_buf= "";
    vector<char> read_buf(chunks); 

    while (req->state != DONE) {
        int bytes_read = fread(&read_buf[0], 1, chunks, reader);
        if (bytes_read == 0) {
            *err = 1;
            delete req;
            return nullptr;
        }

        unparsed_buf.append(&read_buf[0], bytes_read);
        int consumed = parse(req, unparsed_buf, err);

        if (*err != 0 || consumed < 0) {
            delete req;
            return nullptr;
        }
        if (consumed > 0) {
            unparsed_buf.erase(0, consumed);
        }
    }

    return req; 
}