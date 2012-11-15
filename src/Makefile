
.PHONY: build clean
build: libattotest.so

libattotest.so: *.cpp $(shell find $(HEADERDIR)/attotest -name '*.cpp' -or -name '*.hpp')
	$(CXX) $(CXXFLAGS) $(IPATH) -shared -fpic -g -o $@ $^

clean:
	rm -rf *.lib *.o *.so
