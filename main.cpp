#include <iostream>
#include <bits/stdc++.h>

using namespace std;


vector<string> getlinesfromChannel(FILE* fptr){
    vector<string> lines;
    
    char buffer[8];
    string data="";

    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        for (int i = 0; buffer[i] != '\0'; ++i) {
            char c = buffer[i];
            
            if (c == '\n') {
                lines.push_back(data);
                data = ""; 
            } else {
                data += c;
            }
        }
    }
    if (!data.empty()) {
        lines.push_back(data);
    }

    return lines;
}
int main(){
    FILE *fptr;
    fptr = fopen("msg.txt", "r");

    if (fptr == NULL) {
        cout << "Error: File could not be opened." << endl;
        return 1;
    }
    vector<string> lines = getlinesfromChannel(fptr);
    for(string line:lines) cout<<"reads: "<<line<<endl;

    fclose(fptr);
    return 0;
}