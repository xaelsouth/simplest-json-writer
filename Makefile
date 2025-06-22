#
# Copyright (c) 2025 <xael.south@yandex.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

CPPFLAGS ?= -I.
CPPFLAGS += -DDEBUG #-DNDEBUG
CPPFLAGS += #-DNO_MALLOC
CFLAGS ?= --std=c99 -Wall -Wpedantic -O0 -g
CXXFLAGS ?= --std=c++20 -Wall -Wpedantic -O0 -g
LDFLAGS ?=
LDLIBS ?=
CC ?= gcc
CXX ?= g++
RM ?= rm -f

OUTFILE ?= json_writer_main

DEPS = json_writer.h Makefile
OBJ = json_writer.o json_writer_main.o

all: $(OUTFILE)

# Clean this project
clean:
	$(RM) $(OUTFILE) $(OBJ)

# Clean this project and all dependencies
cleanall: clean all

%.o: %.c $(DEPS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.cc %.cpp $(DEPS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OUTFILE): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
