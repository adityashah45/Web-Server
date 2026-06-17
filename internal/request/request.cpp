#include "request.hpp"

RequestLine* parseRequestLine(string b, int *err) {
    RequestLine* rl= new RequestLine();

    vector<string> parts;
    string part = "";

    for (char c : b) {
        if (c == ' ') {
            if (!part.empty()) {
                parts.push_back(part);
                part = "";
            }
        } 
        else if (c != '\r' && c != '\n') {
            part += c;
        }
    }    
    if (!part.empty()) {
        parts.push_back(part);
    }
    if (parts.size() != 3) {
        *err = INC_START_LINE;
        delete rl; 
        return nullptr;
    }


    for(char c:parts[0]){
        if(!(c>='A' && c<='Z')){
            *err = BAD_START_LINE;
            delete rl;
            return nullptr;
        }
    }
    

    if(parts[2]!="HTTP/1.1"){
        *err = UNSUPPORTED_HTTP_VERSION;
        delete rl;
        return nullptr;
    }

    rl->Method = parts[0];
    rl->RequestTarget = parts[1];
    rl->HttpVersion = parts[2];

    return rl;

}

Request* RequestFromReader(FILE* reader, int* err) {
    char buffer[4096];     
    if (fgets(buffer, sizeof(buffer), reader) == NULL) {
        *err = BAD_START_LINE; 
        return nullptr;
    }
    
    string requestLineStr(buffer);
    RequestLine* rl = parseRequestLine(requestLineStr, err);
    if (rl == nullptr || *err != 0) {
        return nullptr; 
    }
    
    Request* req = new Request();
    req->requestLine = *rl;
    delete rl; 
    
    return req; 
}