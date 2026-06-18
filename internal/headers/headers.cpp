#include "headers.hpp"

string trimOWS(const string& s) {
    int start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
        start++;
    }
    if (start == s.size()) {
        return "";
    }

    int end = s.size() - 1;
    while (end >= start && (s[end] == ' ' || s[end] == '\t')) {
        end--;
    }
    return s.substr(start, end - start + 1);
}

bool validChar(char c) {
    if ((c>='A' && c<='Z')|| (c>='a' && c<='z')|| (c>='0' && c<='9')) return true;

    switch (c) {
        case '!': case '#': case '$': case '%': case '&': case '\'': 
        case '*': case '+': case '-': case '.': case '^': case '_': 
        case '`': case '|': case '~':
            return true;
        default:
            return false;
    }
}

string getHeader(const Headers& headers,const string& key) {
    
    string lower_key = "";
    for (char c : key) {
        if (c >= 'A' && c <= 'Z') {
            lower_key += (c - 'A' + 'a');
        } else {
            lower_key += c;
        }
    }
    if(headers.find(lower_key) != headers.end()) {
        return headers.at(lower_key).front();
    }
    return "";
}

int parseHeader(Headers& headers, const string& buf, bool* done, int* err) {
    *done = false;
    *err = 0;

    int crlf_pos = -1;
    for (int i = 0; i + 1 < buf.size(); i++) {
        if (buf[i] == '\r' && buf[i+1] == '\n') {
            crlf_pos = i;
            break;
        }
    }

    if (crlf_pos == -1) {
        return 0; 
    }

    if (crlf_pos == 0) {
        *done = true;
        return 2; 
    }

    string line = buf.substr(0, crlf_pos);
    int consumed = crlf_pos + 2;

    int colon_pos = -1;
    for (int i = 0; i < line.size(); i++) {
        if (line[i] == ':') {
            colon_pos = i;
            break;
        }
    }

    if (colon_pos == -1 || colon_pos == 0) {
        *err = ERR_INVALID_HEADER;
        return 0; 
    }
    if (line[0] == ' ' || line[0] == '\t' || 
        line[colon_pos - 1] == ' ' || line[colon_pos - 1] == '\t') {
        *err = ERR_INVALID_HEADER;
        return 0; 
    }

    string raw_key = line.substr(0, colon_pos);
    string key="";
    for (char c : raw_key) {
        if (validChar(c)) {
            if (c >= 'A' && c <= 'Z') {
                key+=(c-'A' + 'a');
            } else {
                key += c;
            }
        } else {
            *err = ERR_INVALID_HEADER;
            return 0; 
        }
    }
    string value = line.substr(colon_pos + 1);
    headers[key].push_back(trimOWS(value));

    return consumed;
}