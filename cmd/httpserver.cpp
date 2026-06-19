#include "../../internal/server/server.hpp"
#include <iostream>
#include <csignal>
#include <semaphore.h> 

using namespace std;

sem_t shutdown_sem;

void signalHandler(int signum) {
    sem_post(&shutdown_sem);
}

int main() {
    const int port = 42069;
    int err = 0;
    sem_init(&shutdown_sem, 0, 0);

    Server* server = serve(port, &err);
    if (err != 0 || server == nullptr) {
        cerr << "Error starting server! Code: " << err << endl;
        return 1;
    }

    cout << "Server started on port " << port << endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    sem_wait(&shutdown_sem);

    cout << "\nServer gracefully stopped" << endl;
    
    closeServer(server);
    sem_destroy(&shutdown_sem);

    return 0;
}