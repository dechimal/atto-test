
ROOT := $(shell pwd)
SRCDIR := $(ROOT)/src
HEADERDIR := $(ROOT)/include

-include config.mk

IPATH := -I$(SRCDIR) -I. -I$(HEADERDIR)

ifneq ($(BOOST_INCLUDE),)
IPATH += -I$(BOOST_INCLUDE)
endif

ifeq ($(findstring clang,$(CXX)),clang)
CXX11FLAGS := -std=c++11
else
ifeq ($(findstring g++,$(CXX)),g++)
CXX11FLAGS := -std=gnu++0x
endif
endif
CXXFLAGS += $(EXTRACXXFLAGS) $(CXX11FLAGS)

export

.PHONY: build clean test all

all: build

build:
	$(MAKE) -C src build

test: build
	$(MAKE) -C test test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
