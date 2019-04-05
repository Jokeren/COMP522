TESTNAME = Main

DIRS := ./ hp/
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.cpp) )
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCE_FILES))

SANITIZER ?= 1

CXXFLAGS := -g -std=c++11 -O3

ifeq ($(SANITIZE), 1)
CXXFLAGS += -fsanitize=thread
endif

all: $(TESTNAME)

$(TESTNAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CXXFLAGS) -lpthread

$(OBJECTS): %.o : %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(TESTNAME) *.o hp/*.o

print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
