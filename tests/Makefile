# Copyright Paul Dardeau, SwampBits LLC 2014
# BSD License

CC = c++
CC_OPTS = -c -pthread -std=c++20 -I../src -I../chaudiere/src -I../poivre

EXE_NAME = test_misere
LIB_NAMES = ../src/libmisere.so ../chaudiere/src/libchaudiere.so

POIVRE_OBJS = TestCase.o \
TestSuite.o

OBJS = MockSocket.o \
TestHttpClient.o \
TestHTTP.o \
TestHttpException.o \
TestHttpRequest.o \
TestHttpResponse.o \
TestHttpServer.o \
TestHttpTransaction.o \
TestUrl.o \
Tests.o \
$(POIVRE_OBJS)

all : $(EXE_NAME)

clean :
	rm -f *.o
	rm -f $(EXE_NAME)

$(EXE_NAME) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE_NAME) $(LIB_NAMES) -lpthread -ldl

$(POIVRE_OBJS) : %.o : ../poivre/%.cpp
	$(CC) $(CC_OPTS) $< -o $@

%.o : %.cpp
	$(CC) $(CC_OPTS) $< -o $@
