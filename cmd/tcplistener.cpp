// C++ Std Libs
#include <iostream>
#include <cstdlib> 
#include <cstdio>  

// POSIX Networking Headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "request.hpp"

using namespace std;


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
            continue; 
        }

        FILE* fptr = fdopen(conn, "r");
        if (fptr == nullptr) {
            cerr << "Error: fdopen failed\n";
            close(conn); 
            continue;
        }

        int err = 0;
        Request* req = RequestFromReader(fptr, &err, 1024);
        
        if (req == nullptr) {
            cerr << "Error: Failed to parse request\n";
            fclose(fptr); 
            continue;
        }

        if (err != 0) { 
            cerr << "Error: Invalid request format\n";
            delete req;
            fclose(fptr); 
            continue;     
        }
        cout<<"Requestline:"<<endl;
        cout << "Method: " << req->requestLine.Method << endl;
        cout << "Target: " << req->requestLine.RequestTarget << endl;
        cout << "HTTP Version: " << req->requestLine.HttpVersion << endl;   
        cout<<"--------------------"<<endl;
        cout<<"Headers:"<<endl;
        for (auto& header : req->headers) {
            for (int i = 0; i < header.second.size(); i++) {
                cout << header.first << ": "  << header.second[i] << endl;
            } 
        }
        cout<<"--------------------"<<endl;
        delete req;
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