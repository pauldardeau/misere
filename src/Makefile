# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

CC = c++
CC_OPTS = -c -Wall -fPIC -O2 -pthread -I../../chaudiere/src
LINK_CMD = c++
# remove -ldl for non-linux
LINK_LIBS = -lpthread

EXE_NAME = misere
SO_NAME = misere.so

OBJS =  HttpClient.o \
HTTP.o \
HttpException.o \
HttpRequest.o \
HttpResponse.o \
HttpTransaction.o \
HttpConnection.o \
Url.o

MAIN_OBJS = main.o


all : $(SO_NAME)

clean :
	rm -f *.o
	rm -f $(SO_NAME)

$(SO_NAME) : $(OBJS)
	$(LINK_CMD) -shared -fPIC $(OBJS) -o $(SO_NAME) $(LINK_LIBS)

%.o : %.cpp
	$(CC) $(CC_OPTS) $< -o $@
