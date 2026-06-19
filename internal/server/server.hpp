#ifndef SERVER_HPP
#define SERVER_HPP

#include <thread>
#include <semaphore.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "../response/response.hpp"
#include "../request/request.hpp"
#define MAX_CONNECTIONS 100

using namespace std;

struct HandlerError {
    StatusCode code;
    string message;
};

typedef HandlerError* (*HandlerFunc)(Writer* w, Request* req);

struct Server {
    int server_fd;                   
    bool running;                    
    thread listener_thread;     
    sem_t connection_sem;            
    HandlerFunc handler;           
};


Server* serve(int port, HandlerFunc handler, int* err);
void closeServer(Server* s);
void listenLoop(Server* s);
void handleConnection(Server* s, int client_fd);


#endif