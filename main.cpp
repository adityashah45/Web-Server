#include <iostream>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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


int create_socket(int* listener){
    
    if ((*(listener) = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Error: Failed to create socket\n";
        exit(1);
    }
    sockaddr_in listener_addr{};
    listener_addr.sin_family = AF_INET;
    listener_addr.sin_addr.s_addr = INADDR_ANY; 
    listener_addr.sin_port = htons(8080);     

    if (::bind(*listener, (struct sockaddr*)&listener_addr, sizeof(listener_addr)) < 0) {
        cerr << "Error: Failed to bind to port 8080\n";
        close(*(listener));
        exit(1);
    }
    return 0;
}
void accept_clients(int listener) {

    while (true) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);

            int conn = accept(listener, (struct sockaddr*)&client_addr, &client_len);
            
            if (conn < 0) {
                cerr << "Error: Failed to accept client connection\n";
                exit(1); 
            }

            FILE* fptr = fdopen(conn, "r");
            if (fptr == nullptr) {
                cerr << "Error: fdopen failed\n";
                close(conn);
                continue;
            }
            vector<string> lines = getlinesfromChannel(fptr);
            
            for (auto line : lines) {
                cout << "read: " << line << "\n";
            }

            fclose(fptr); 
        }
}
int main(){
    int listener;
    create_socket(&listener);
    if (listen(listener, 10) < 0) {
        cerr << "Error: Failed to listen on socket\n";
        close(listener);
        exit(1);
    }
    cout << "Server is listening on port 8080..." << endl;
    accept_clients(listener);
    return 0;
}