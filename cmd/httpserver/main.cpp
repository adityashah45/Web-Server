#include "../../internal/server/server.hpp"
#include <iostream>
#include <csignal>
#include <semaphore.h> 
#include "../response/response.hpp"
#include "../request/request.hpp"   
#include <fstream>
#include <vector>

using namespace std;

sem_t shutdown_sem;

void signalHandler(int signum) {
    sem_post(&shutdown_sem);
}

#include <fstream>
#include <sstream>

HandlerError* videoHandler(Writer* w, Request* req) {
    ifstream file("assets/vim.mp4", ios::binary);
    
    if (!file.is_open()) {
        writeStatusLine(w, BAD_REQUEST);
        Headers h = getDefaultHeaders(14);
        writeHeaders(w, h);
        writeBody(w, "File Not Found");
        return nullptr;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string body = buffer.str();
    Headers h = getDefaultHeaders(body.length());
    h["Content-Type"] = {"video/mp4"};

    writeStatusLine(w, OK);
    writeHeaders(w, h);
    writeBody(w, body);

    return nullptr;
}

HandlerError* myHandler(Writer* w, Request* req) {
    StatusCode code;
    string body;
    if (req->requestLine.RequestTarget == "/video") {
        return videoHandler(w, req);
    }
    else if (req->requestLine.RequestTarget.find("/httpbin/") == 0) {
        string remote_path = req->requestLine.RequestTarget.substr(8);
    
        Headers h;
        h["Connection"] = {"close"};
        h["Content-Type"] = {"application/json"};
        h["Transfer-Encoding"] = {"chunked"};
        
        writeStatusLine(w, OK);
        writeHeaders(w, h);
    
        string cmd = "curl -s -N https://httpbin.org" + remote_path;
        cout << "[Proxy] Connecting to: " << cmd << endl;

        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            writeChunkedBodyDone(w);
            return nullptr;
        }
    char buffer[1024];
        while (true) {
            size_t n = fread(buffer, 1, 1024, pipe);
            if (n == 0) break; 
            cout << "[Proxy] Read " << n << " bytes from httpbin. Forwarding..." << endl;
            writeChunkedBody(w, buffer, n);
        }
        
        pclose(pipe);
        writeChunkedBodyDone(w);
        return nullptr;
    }
   else if (req->requestLine.RequestTarget == "/yourproblem") {
        code = BAD_REQUEST;
        body = "<html>\n<head>\n<title>400 Bad Request</title>\n</head>\n<body>\n<h1>Bad Request</h1>\n<p>Your request honestly kinda sucked.</p>\n</body>\n</html>";
    } 
    else if (req->requestLine.RequestTarget == "/myproblem") {
        code = INTERNAL_SERVER_ERROR;
        body = "<html>\n<head>\n<title>500 Internal Server Error</title>\n</head>\n<body>\n<h1>Internal Server Error</h1>\n<p>Okay, you know what? This one is on me.</p>\n</body>\n</html>";
    } 
    else {
        code = OK;
        body = "<html>\n<head>\n<title>200 OK</title>\n</head>\n<body>\n<h1>Success!</h1>\n<p>Your request was an absolute banger.</p>\n</body>\n</html>";
    }

    Headers h = getDefaultHeaders(body.length());
    h["Content-Type"] = {"text/html"}; 

    writeStatusLine(w, code);
    writeHeaders(w, h);
    writeBody(w, body);

    return nullptr;
}

int main() {
    const int port = 8080;
    int err = 0;
    sem_init(&shutdown_sem, 0, 0);

    Server* server = serve(port, myHandler, &err);
    if (err != 0 || server == nullptr) {
        cerr << "Error starting server! Code: " << err << endl;
        return 1;
    }

    cout << "Server started on port " << port << endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    sem_wait(&shutdown_sem);

    cout<<"\nServer finally stopped" << endl;
    
    closeServer(server);
    sem_destroy(&shutdown_sem);

    return 0;
}