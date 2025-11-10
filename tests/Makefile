# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

CC = c++
CC_OPTS = -c -pthread -I../src -I../../chaudiere/src -I../../poivre

EXE_NAME = test_misere
LIB_NAMES = ../src/libmisere.so ../../poivre/poivre.so ../../chaudiere/src/libchaudiere.so

OBJS = MockSocket.o \
TestHttpClient.o \
TestHTTP.o \
TestHttpException.o \
TestHttpRequest.o \
TestHttpResponse.o \
TestHttpServer.o \
TestHttpTransaction.o \
TestUrl.o \
Tests.o

all : $(EXE_NAME)

clean :
	rm -f *.o
	rm -f $(EXE_NAME)
	
$(EXE_NAME) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE_NAME) $(LIB_NAMES) -lpthread

%.o : %.cpp
	$(CC) $(CC_OPTS) $< -o $@
