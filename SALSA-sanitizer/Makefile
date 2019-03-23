TESTNAME = Main

DIRS := ./ hp/
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.cpp) )
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCE_FILES))

all: $(TESTNAME)

$(TESTNAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -std=c++11 -g -lpthread -fsanitize=thread

$(OBJECTS): %.o : %.cpp
	$(CXX) -c -o $@ $< -fsanitize=thread -g

clean:
	rm -f $(TESTNAME) *.o hp/*.o

print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
