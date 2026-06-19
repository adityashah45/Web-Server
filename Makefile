CXX = clang++
CXXFLAGS = -std=c++17 -Wall -pthread
INCLUDES = -I internal/request -I internal/headers -I internal/server

TARGET = httpserver

SRCS = cmd/httpserver/main.cpp \
       internal/server/server.cpp \
       internal/request/request.cpp \
       internal/request/requestline.cpp \
       internal/headers/headers.cpp \
       internal/response/response.cpp # <-- Add this line!

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
