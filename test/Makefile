
TARGETS := $(patsubst %.cpp,%,$(wildcard *.cpp))
SCRIPTS := $(wildcard *.sh)

$(TARGETS) : $(SRCDIR)/libattotest.so
$(TARGETS) : % : %.cpp
	$(CXX) $(CXXFLAGS) $(IPATH) -g -o $@ $^

.PHONY: clean test

clean:
	rm -f $(TARGETS)

test: $(sort $(TARGETS) $(SCRIPTS))
	for i in $^; do ./$$i || exit; done
