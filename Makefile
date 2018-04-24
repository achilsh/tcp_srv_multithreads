INC_SVR_COMM = -I.  -I/usr/local/include
LIB_SVR_COMM = -L/usr/local/lib -levent -lpthread

INC_ALL=$(INC_SVR_COMM) 
LIB_ALL=$(LIB_SVR_COMM)

BINARY = TcpSrvMain   TcpClientMain

all:$(BINARY)


.SUFFIXES: .o .cpp
CXX = g++
CC = gcc

CXXFLAGS= -g -Wall

%.o:%.cpp
	$(CXX) $(CFLAGS) -c $^ $(INC_ALL)

TcpSrvMain:t_main.o    t_tcpsrv.o  t_socket.o  t_proto.o  t_eventpipe.o  t_eventconn.o  t_eventbase.o  t_worktask.o t_eventlisten.o  t_thread.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB_ALL)
TcpClientMain:t_client.o    t_tcpsrv.o  t_socket.o  t_proto.o  t_eventpipe.o  t_eventconn.o  t_eventbase.o  t_worktask.o t_eventlisten.o  t_thread.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB_ALL)
clean:
	rm -f *.o *~ $(BINARY)

strip:
	strip $(BINARY)
