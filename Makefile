SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
TESTDIR	 = pytest

# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard ${SRCDIR}/test*.cpp)
# names of test executables
TESTS       = $(TESTSOURCES:$(SRCDIR)/%.cpp=$(BINDIR)/%)

# list of sources used in project
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
SOURCES  := $(filter-out $(TESTSOURCES), $(SOURCES))
# list of objects used in project
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

SO_PATH = $(LD_LIBRARY_PATH)

LIB = paxos.so
RPC_LIB = drpc.so

#Default Flags
CXXFLAGS = -std=c++14 -Wconversion -Wall -Werror -Wextra -pedantic -pthread

# highest target; sews together all objects into executable
all: $(LIB) $(TESTS)

fast: CXXFLAGS += -ofast
fast: clean all

# make debug - will compile "all" with $(CXXFLAGS) and the -g flag
#              also defines DEBUG so that "#ifdef DEBUG /*...*/ #endif" works
debug: CXXFLAGS += -g3 -DDEBUG
debug: clean all

test: $(TESTS)
	@pytest

$(LIB): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o ${OBJDIR}/$(LIB) $(SO_PATH)/$(RPC_LIB) -shared
	ln -f ${OBJDIR}/$(LIB) $(SO_PATH)

clean:
	rm -rf ${OBJDIR} ${BINDIR} paxos_server

headers:
	cp ../channel/Channel.h ./${SRCDIR}
	cp ../drpc/src/drpc.h ./${SRCDIR}

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p ${OBJDIR}
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

$(TESTS): $(BINDIR)/% : $(SRCDIR)/%.cpp $(SO_PATH)/$(LIB) $(SO_PATH)/$(RPC_LIB) 
	@mkdir -p ${BINDIR}
	$(CXX) $(CXXFLAGS) -lm -I. -o $@ $^

paxos_server: $(SRCDIR)/paxos_server.cpp $(SO_PATH)/$(LIB) $(SO_PATH)/$(RPC_LIB) 
	$(CXX) $(CXXFLAGS) -o $@ $^
