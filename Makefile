# --- Variables ---
# You can change this back to g++ if you prefer
CXX = clang++

# The compiler flags we discussed
CXXFLAGS = -std=c++17 -Wall

# Tell the compiler where to find your header files
INCLUDES = -I internal/request

# The name of your final runnable server
TARGET = tcplistener

# List all your source files here
SRCS = cmd/tcplistener.cpp internal/request/request.cpp

# Automatically generate a list of object files (.o) from the .cpp files
OBJS = $(SRCS:.cpp=.o)

# --- Rules ---

# 'all' is the default target that runs when you just type 'make'
all: $(TARGET)

# How to build the final executable (links the .o files together)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# How to compile each individual .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# A handy rule to clean up your directory (deletes the server and .o files)
clean:
	rm -f $(TARGET) $(OBJS)

# Tells Make that 'all' and 'clean' are commands, not physical files
.PHONY: all clean
