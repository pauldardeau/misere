# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

CC = c++
CC_OPTS = -c -Wall -fPIC -O2 -pthread -I../../chaudiere/src
LINK_CMD = c++
# remove -ldl for non-linux
LINK_LIBS = -lpthread -lsocket -lnsl

EXE_NAME = misere
SO_NAME = libmisere.so

OBJS =  HttpClient.o \
HTTP.o \
HttpException.o \
HttpRequest.o \
HttpRequestHandler.o \
HttpResponse.o \
HttpServer.o \
HttpSocketServiceHandler.o \
HttpTransaction.o \
HttpConnection.o \
AbstractHandler.o \
EchoHandler.o \
GMTDateTimeHandler.o \
ServerDateTimeHandler.o \
ServerObjectsDebugging.o \
ServerStatsHandler.o \
ServerStatusHandler.o \
Url.o

MAIN_OBJS = main.o


all : $(SO_NAME) $(EXE_NAME)

clean :
	rm -f *.o
	rm -f $(SO_NAME)
	rm -f $(EXE_NAME)

$(SO_NAME) : $(OBJS)
	$(LINK_CMD) -shared -fPIC $(OBJS) -o $(SO_NAME) $(LINK_LIBS)

$(EXE_NAME) : $(SO_NAME) $(MAIN_OBJS)
	$(CC) -c -Wall -O2 -pthread -I../../chaudiere/src main.cpp -o main.o
	$(LINK_CMD) $(MAIN_OBJS) -o $(EXE_NAME) ./$(SO_NAME) ../../chaudiere/src/libchaudiere.so $(LINK_LIBS) -ldl

%.o : %.cpp
	$(CC) $(CC_OPTS) $< -o $@
