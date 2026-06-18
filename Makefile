CXX = clang++
CXXFLAGS = -std=c++17 -Wall
INCLUDES = -I internal/request
TARGET = tcplistener
SRCS = cmd/tcplistener.cpp internal/request/request.cpp internal/request/requestline.cpp internal/headers/headers.cpp
OBJS = $(SRCS:.cpp=.o)


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
