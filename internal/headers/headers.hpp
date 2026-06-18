#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <vector>
#include <string>
#include <map>
using  namespace std;

#define ERR_INVALID_HEADER 5

typedef map<string, vector<string>> Headers;
int parseHeader(Headers& headers, const string& buf, bool* done, int* err);
bool validChar(char c);
string trimOWS(const string& s);
string getHeader(const Headers& headers, const string& key);

#endif