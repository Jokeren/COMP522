include ../benchmarks.mk

TESTNAME = Main

DIRS := ./ hp/
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.cpp) )
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCE_FILES))

all: $(TESTNAME)

$(TESTNAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) -std=c++11 -g

$(OBJECTS): %.o : %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TESTNAME) *.o hp/*.o

print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
