#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <vector>
#include <string>
#include <map>
using  namespace std;

#define ERR_INVALID_HEADER 1;

typedef map<string, vector<string>> Headers;
int parseHeader(Headers& headers, const string& buf, bool* done, int* err);

#endif