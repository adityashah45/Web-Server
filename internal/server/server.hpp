#ifndef SERVER_HPP
#define SERVER_HPP

#include <thread>
#include <semaphore.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

#define MAX_CONNECTIONS 100 


struct Server {
    int server_fd;                   
    bool running;                    
    std::thread listener_thread;     
    sem_t connection_sem;           
};

Server* serve(int port, int* err);
void closeServer(Server* s);
int createServerSocket(int port, int* err);
Server* initServer(int server_fd, int* err);
void listenLoop(Server* s);
void handleConnection(Server* s, int client_fd); 

#endif