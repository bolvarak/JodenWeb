CXX = gcc
CXXFLAGS = -Wall -ansi -pedantic -g

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl
endif

all: JodenServer

JodenServer: server.c
	$(CXX) -D_GNU_SOURCE -o bin/server server.c $(LIBS) -lpthread

clean:
	$(RM) JodenServer