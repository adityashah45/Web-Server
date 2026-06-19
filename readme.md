# C++ HTTP/1.1 Web Server

## Project Structure

```text
.
├── assets/                  # Binary media files 
├── cmd/
│   └── httpserver/
│       └── main.cpp         # Application entry point, router, and custom handlers
├── internal/
│   ├── headers/             # HTTP header parsing and manipulation
│   ├── request/             # Request parsing
│   ├── response/            # Response writer, chunking logic
│   └── server/              # TCP socket loop 
├── public/                  # Static text/HTML files
└── Makefile                 
```

## Build and Run

Compile the server:
Use the provided Makefile to compile the executable.
```
make clean && make
```

Start the server:

Run the executable from the project root so it can locate the `public/` and `assets/` directories.

```
./httpserver
```

Test the endpoints:

Open your browser or use curl to interact with the server.

- Standard Route: `curl -i http://localhost:8080/`
- Custom Error Route: `curl -i http://localhost:8080/yourproblem`
- Live Proxy Stream: `curl --raw -i http://localhost:8080/httpbin/html`
- Binary Video Stream: Navigate to `http://localhost:8080/video` in a web browser.

Note: A video must be present in the public folder to be able to stream your preferred video.
