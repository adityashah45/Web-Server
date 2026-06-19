#include "server.hpp"

using namespace std;


void writeHandlerError(Writer* w, HandlerError* err) {
    writeStatusLine(w, err->code);
    Headers headers = getDefaultHeaders(err->message.length());
    writeHeaders(w, headers);
    writeBody(w, err->message);
}

void handleConnection(Server* s, int client_fd) {
    int err = 0;
    Request* req = RequestFromReader(client_fd, &err, 1024);
    Writer w = {client_fd, EXPECT_STATUS};

    if (err != 0 || req == nullptr) {
        HandlerError parseErr = {BAD_REQUEST, "Invalid HTTP Request\n"};
        writeHandlerError(&w, &parseErr);
    } else {
        HandlerError* handlerErr = s->handler(&w, req);

        if (handlerErr != nullptr) {
            writeHandlerError(&w, handlerErr);
            delete handlerErr; 
        }
    }

    if (req != nullptr) delete req;
    close(client_fd); 
    sem_post(&s->connection_sem); 
}

void listenLoop(Server* s) {
    while (s->running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(s->server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            if (!s->running) break; 
            continue;
        }

        sem_wait(&s->connection_sem);

        thread client_thread(handleConnection, s, client_fd);
        client_thread.detach(); 
    }
}

int createServerSocket(int port, int* err){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        *err = 1;
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        *err = 2;
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        *err = 3;
        close(server_fd);
        return -1;
    }
    return server_fd;
}
Server* initServer(int server_fd, HandlerFunc handler, int* err) {
    Server* s = new Server();
    s->server_fd = server_fd;
    s->running = true;
    sem_init(&s->connection_sem, 0, MAX_CONNECTIONS);
    s->handler = handler;
    s->listener_thread = thread(listenLoop, s);
    return s;
}

Server* serve(int port, HandlerFunc handler, int* err) {
    *err = 0;
    int server_fd = createServerSocket(port, err);
    if (server_fd == -1) {
        return nullptr;
    }
    Server* s = initServer(server_fd,handler, err);
    return s;
}
void closeServer(Server* s) {
    if (!s) return;
    
    s->running = false;
    shutdown(s->server_fd, SHUT_RDWR);
    close(s->server_fd);
    
    if (s->listener_thread.joinable()) {
        s->listener_thread.join();
    }
    
   cout<<"Waiting for active workers to finish..." << endl;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        sem_wait(&s->connection_sem);
    }
    sem_destroy(&s->connection_sem);

   cout<<"Worker Threads are destroyed. Server is shutting down." << endl;
    delete s;
}