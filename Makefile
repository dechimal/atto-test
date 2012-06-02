
ROOT := $(shell pwd)
SRCDIR := $(ROOT)/src
HEADERDIR := $(ROOT)/include

-include Makefile.config

IPATH := -I$(SRCDIR) -I. -I$(HEADERDIR)

ifeq ($(BOOST_INCLUDE),)
IPATH := $(IPATH) -I$(BOOST_INCLUDE)
endif

ifeq ($(findstring clang,$(CXX)),clang)
CXX11FLAGS := -std=c++11
else
ifeq ($(findstring g++,$(CXX)),g++)
CXX11FLAGS := -std=gnu++0x
endif
endif
CXXFLAGS := $(USER_CXXFLAGS) $(CXX11FLAGS) $(EXTRAFLAGS)

export SRCDIR
export HEADERDIR
export CXX
export CXXFLAGS
export IPATH

.PHONY: build clean test all time

all: build

build:
	$(MAKE) -C src build

test: build
	$(MAKE) -C test test

time: build
	$(MAKE) -C test time

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
