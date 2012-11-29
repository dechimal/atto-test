
.PHONY: build clean

CXXFLAGS += -shared -fpic -g -O0
sources := $(wildcard *.cpp)

lib%.so : %
	cp $< $@

build: libattotest.so

attotest: *.cpp

clean:
	rm -rf *.a *.o *.so attotest *.deps libattotest.so

%.deps: %.cpp
	$(CXX) -MM -MT'$@.so' $(CPPFLAGS) $(CXXFLAGS) $< >$@
	[[ -s $@ ]] || rm -f $@

include $(sources:.cpp=.deps)
