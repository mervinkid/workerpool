# Makefile for workerpool project
#
# The MIT License (MIT)
#
# Copyright (c) 2016 Mervin <mofei2816@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

NAME = workerpool
VERSION = 0.1.0
BUILDNAME = lib$(NAME)
CFLAGS = -Wall -Isrc -O -W
LDLIBS = -pthread
CC = clang
AR = ar
PLATFORM = $(shell sh -c 'uname -s | tr "[A-Z]" "[a-z]"')
SRCDIR = src
TESTDIR = test
BUILDDIR = build

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

# setup file name
ANAME = $(BUILDNAME).a
ifeq ($(PLATFORM), darwin)
SONAME = $(BUILDNAME).$(VERSION).dylib
SOEXT = dylib
else
SONAME = $(BUILDNAME).so.$(VERSION)
SOEXT = so
endif

INSTALL := install
UNINSTALL := rm
LINK := ln -s
PREFIX := /usr/local

# compile and build
all: library

# make build output dir
buildpath:
	mkdir -p $(BUILDDIR)

# compile worker pool
workerpool.o: buildpath
	$(CC) $(CFLAGS) -c -fpic $(SRCDIR)/workerpool.c -o $(BUILDDIR)/workerpool.o

# compile task queue
taskqueue.o: buildpath
	$(CC) $(CFLAGS) -c -fpic $(SRCDIR)/taskqueue.c -o $(BUILDDIR)/taskqueue.o

# compile test
test.o: buildpath
	$(CC) $(CFLAGS) -c -fpic $(TESTDIR)/test.c -o $(BUILDDIR)/test.o

# build static library
static: workerpool.o taskqueue.o
	$(AR) -r $(BUILDDIR)/$(ANAME) $(BUILDDIR)/workerpool.o $(BUILDDIR)/taskqueue.o

# build shared library
shared: workerpool.o taskqueue.o
	$(CC) $(CFLAGS) $(LDLIBS) -shared $(BUILDDIR)/workerpool.o $(BUILDDIR)/taskqueue.o -o $(BUILDDIR)/$(SONAME)

# build both shared and static library
library: static shared

# test
test: test.o workerpool.o taskqueue.o
	$(CC) $(CFLAGS) $(LDLIBS) $(BUILDDIR)/test.o $(BUILDDIR)/workerpool.o $(BUILDDIR)/taskqueue.o -o $(BUILDDIR)/test
	$(BUILDDIR)/test

# install to system
install: library
	$(INSTALL) $(SRCDIR)/workerpool.h $(PREFIX)/include/workerpool.h
	$(INSTALL) $(SRCDIR)/taskqueue.h  $(PREFIX)/include/taskqueue.h
	$(INSTALL) $(BUILDDIR)/$(SONAME)  $(PREFIX)/lib/$(SONAME)
	$(INSTALL) $(BUILDDIR)/$(ANAME)   $(PREFIX)/lib/$(ANAME)
	$(LINK) $(PREFIX)/lib/$(SONAME)   $(PREFIX)/lib/$(BUILDNAME).$(SOEXT)

# uninstall from system
uninstall:
	$(UNINSTALL) $(PREFIX)/lib/$(BUILDNAME).$(SOEXT)
	$(UNINSTALL) $(PREFIX)/lib/$(SONAME)
	$(UNINSTALL) $(PREFIX)/lib/$(ANAME)
	$(UNINSTALL) $(PREFIX)/include/workerpool.h
	$(UNINSTALL) $(PREFIX)/include/taskqueue.h

# clean up all build output files.
clean:
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/$(ANAME) $(BUILDDIR)/$(SONAME) $(BUILDDIR)/test 
