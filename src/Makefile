# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

CC = clang++
CC_OPTS = -c -Wall -O2 -pthread -fblocks -std=c++11 -I../../chaudiere/src
LINK_CMD = clang++
LINK_LIBS = -lpthread -ldl

EXE_NAME = misere

OBJS =  HttpClient.o \
HTTP.o \
HttpException.o \
HttpRequest.o \
HttpRequestHandler.o \
HttpResponse.o \
HttpServer.o \
HttpSocketServiceHandler.o \
HttpTransaction.o \
AbstractHandler.o \
EchoHandler.o \
GMTDateTimeHandler.o \
ServerDateTimeHandler.o \
ServerObjectsDebugging.o \
ServerStatsHandler.o \
ServerStatusHandler.o \
main.o

all : $(EXE_NAME)

clean :
	rm -f *.o
	rm -f $(EXE_NAME)

$(EXE_NAME) : $(OBJS)
	$(LINK_CMD) -o $(EXE_NAME) $(OBJS) ../../chaudiere/src/libchaudiere.a $(LINK_LIBS)

%.o : %.cpp
	$(CC) $(CC_OPTS) $< -o $@
