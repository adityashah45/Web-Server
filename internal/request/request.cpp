#include "request.hpp"


int parseSingle(Request* req, const string& unparsed_buf, int* err) {
    if (unparsed_buf.empty()) {
        return 0;
    }

    switch (req->state) {
        case INIT: {
            int consumed = parseRequestLine(unparsed_buf, &(req->requestLine), err);   
            if (consumed > 0 && *err == 0) {
                req->state = PARSING_HEADERS; 
            }
            return consumed;
        } 
        case PARSING_HEADERS: {
            bool done = false;
            int consumed = parseHeader(req->headers, unparsed_buf, &done, err);
            if (*err != 0) {
                return 0; 
            }
            if (done) {
                req->state = PARSING_BODY; 
            }
            return consumed; 
        }
        case PARSING_BODY: {
            string cl_str = getHeader(req->headers, "Content-Length");
            if (cl_str.empty()) {
                req->state = DONE;
                return 0; 
            }
            int expected_len=stoi(cl_str);
            req->body += unparsed_buf;
            int consumed = unparsed_buf.size();

            if (req->body.size() > expected_len) {
                *err = BODY_LEN_EXCEEDED; 
                return 0;
            }

            if (req->body.size() == expected_len) {
                req->state = DONE;
            }

            return consumed;
        }
        case DONE:
            return 0;

        default:
            return 0; 
    }
}


int parse(Request* req, const string& unparsed_buf, int* err) {
    int total_consumed = 0;
    while (req->state != DONE) {
        string current_data = unparsed_buf.substr(total_consumed);
        int consumed = parseSingle(req, current_data, err);
        if (*err != 0) {
            return -1; 
        }
        if (consumed == 0) {
            break; 
        }
        total_consumed += consumed;
    }
    return total_consumed;
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