# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard test*.cpp)
# names of test executables
TESTS       = $(TESTSOURCES:%.cpp=%)

# list of sources used in project
SOURCES 	= $(wildcard *.cpp)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))
# list of objects used in project
OBJECTS		= $(SOURCES:%.cpp=%.o)

SO_PATH = $(LD_LIBRARY_PATH)

LIB = paxos.so
RPC_LIB = drpc.so

#Default Flags
CXXFLAGS = -std=c++14 -Wconversion -Wall -Werror -Wextra -pedantic -pthread

# make debug - will compile "all" with $(CXXFLAGS) and the -g flag
#              also defines DEBUG so that "#ifdef DEBUG /*...*/ #endif" works
debug: CXXFLAGS += -g3 -DDEBUG
debug: clean all

# highest target; sews together all objects into executable
all: $(LIB) test_many_clients test_many_forget test_forget test_endpoint test_concurrent test_basic test_deaf test_min test_many test_old paxos_server

final: clean $(LIB)
	ln -f $(LIB) $(SO_PATH)

$(LIB): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(LIB) $(SO_PATH)/$(RPC_LIB) -shared

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TESTS) $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE) *.out

headers:
	cp ../drpc/drpc.h .
	cp ../drpc/Channel.h .

# test1: test1.cpp $(LIB)
# 	$(CXX) $(CXXFLAGS) -o $@ $^
test_basic: test_basic.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_endpoint: test_endpoint.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_concurrent: test_concurrent.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_deaf: test_deaf.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_min: test_min.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_forget: test_forget.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_many: test_many.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_many_forget: test_many_forget.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_many_clients: test_many_clients.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

test_old: test_old.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

paxos_server: paxos_server.cpp $(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^

# rule for creating objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -g -c $*.cpp
