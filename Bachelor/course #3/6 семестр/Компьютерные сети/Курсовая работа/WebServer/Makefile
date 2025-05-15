CXX = g++
CXXFLAGS = -std=c++20 -Wall -pthread
LDFLAGS = -lssl -lcrypto

SRC = main.cpp \
      server_backup/HttpServer.cpp \
      server_backup/RequestHandler.cpp \
      server_backup/ThreadPool.cpp \
      server_backup/SSLContext.cpp

OBJ = $(SRC:.cpp=.o)

all: server

server: $(OBJ)
	$(CXX) $(CXXFLAGS) -o server $(OBJ) $(LDFLAGS)

clean:
	rm -f server $(OBJ)
