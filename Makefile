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

CFLAGS = -Wall -Isrc -O -W
LDLIBS = -pthread
CC = clang
AR = ar
OS_TYPE = $(shell uname)
NAME = workerpool
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
BUILD_NAME = lib$(NAME)

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

# setup file name
BUILD_STATIC = $(BUILD_NAME).a
ifeq ($(OS_TYPE), Darwin)
BUILD_SHARED = $(BUILD_NAME).dylib
else
BUILD_SHARED = $(BUILD_NAME).so
endif

# compile and build
all: workerpool.o taskqueue.o static shared test

# make build output dir
buildpath:
	$(shell mkdir -p $(BUILD_DIR))

# compile worker pool
workerpool.o: buildpath
	$(CC) $(CFLAGS) -c -fpic $(SRC_DIR)/workerpool.c -o $(BUILD_DIR)/workerpool.o

# compile task queue
taskqueue.o: buildpath
	$(CC) $(CFLAGS) -c -fpic $(SRC_DIR)/taskqueue.c -o $(BUILD_DIR)/taskqueue.o

# build static lib
static: workerpool.o taskqueue.o
	$(AR) -r $(BUILD_DIR)/$(BUILD_STATIC) $(BUILD_DIR)/workerpool.o $(BUILD_DIR)/taskqueue.o

# build shared lib
shared: workerpool.o taskqueue.o
	$(CC) $(CFLAGS) $(LDLIBS) -shared $(BUILD_DIR)/workerpool.o $(BUILD_DIR)/taskqueue.o -o $(BUILD_DIR)/$(BUILD_SHARED)

# compile test
test.o: buildpath
	$(CC) $(CFLAGS) -c -fpic $(TEST_DIR)/test.c -o $(BUILD_DIR)/test.o

# test
test: test.o workerpool.o taskqueue.o
	$(CC) $(CFLAGS) $(LDLIBS) $(BUILD_DIR)/test.o $(BUILD_DIR)/workerpool.o $(BUILD_DIR)/taskqueue.o -o $(BUILD_DIR)/test
	$(BUILD_DIR)/test

# install to system
install: all
	$(shell mkdir -p /opt/$(NAME))
	$(shell mkdir -p /opt/$(NAME)/lib)
	$(shell mkdir -p /opt/$(NAME)/include)
	$(shell cp $(BUILD_DIR)/$(BUILD_SHARED) /opt/$(NAME)/lib/)
	$(shell cp $(BUILD_DIR)/$(BUILD_STATIC) /opt/$(NAME)/lib/)
	$(shell cp $(SRC_DIR)/workerpool.h /opt/$(NAME)/include/)
	$(shell cp $(SRC_DIR)/taskqueue.h  /opt/$(NAME)/include/)
	$(shell ln -s /opt/$(NAME)/lib/$(BUILD_SHARED) /usr/local/lib/)
	$(shell ln -s /opt/$(NAME)/lib/$(BUILD_STATIC) /usr/local/lib/)
	$(shell ln -s /opt/$(NAME)/include/workerpool.h /usr/local/include/)
	$(shell ln -s /opt/$(NAME)/include/taskqueue.h  /usr/local/include/)
	@echo "$(NAME): install done."

# uninstall from system
uninstall:
	$(shell rm -rf /usr/local/lib/$(BUILD_SHARED))
	$(shell rm -rf /usr/local/lib/$(BUILD_STATIC))
	$(shell rm -rf /usr/local/include/workerpool.h)
	$(shell rm -rf /usr/local/include/taskqueue.h)
	$(shell rm -rf /opt/$(NAME))
	@echo "$(NAME): uninstall done."

# clean up all build output files.
clean:
	$(shell rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.dylib $(BUILD_DIR)/*.so $(BUILD_DIR)/*.a $(BUILD_DIR)/test)
	@echo "$(NAME): clean done."
