# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard test*.cpp)
# names of test executables
TESTS       = $(TESTSOURCES:%.cpp=%)

# list of sources used in project
SOURCES 	= $(wildcard *.cpp)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))
# list of objects used in project
OBJECTS		= $(SOURCES:%.cpp=%.o)

LIB = paxos.so

#Default Flags
CXXFLAGS = -std=c++14 -Wconversion -Wall -Werror -Wextra -pedantic

# make debug - will compile "all" with $(CXXFLAGS) and the -g flag
#              also defines DEBUG so that "#ifdef DEBUG /*...*/ #endif" works
debug: CXXFLAGS += -g3 -DDEBUG
debug: clean all

# highest target; sews together all objects into executable
all: $(LIB) 

$(LIB): $(OBJECTS)
	$(CXX)  $(CXXFLAGS) $(OBJECTS)  -o  $(LIB)  -shared

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TESTS) $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE)

# test1: test1.cpp drpc.so
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# rule for creating objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -g -c $*.cpp
