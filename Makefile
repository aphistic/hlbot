EXECUTABLE = hlbot 

CC = g++
#CFLAGS = -Wall
COMPILE = $(CC) $(CFLAGS) -c
INCLUDE = -Iinclude

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,src/%.o,$(SRCS))

### FreeBSD Detection ###
OS := $(shell uname)
ifeq ($(OS),FreeBSD)
	ifeq ($(MAKE),gmake)
		# OK, we're good
	else
		GMAKE_ERR = Y
	endif

	LIBS =
else
	LIBS = -ldl
endif

#########################

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)

	$(CC) $(LIBS) -g -O -o $(EXECUTABLE) $(OBJS)

%.o: %.cpp

ifeq ($(GMAKE_ERR),Y)

	@echo "WARNING ####################################"
	@echo "You're running FreeBSD."
	@echo "You must use GNU Make to compile HLBot."
	@echo "To fix: MAKE=gmake"
	@echo "USE GMAKE!!!!"
	$(error Use gmake!)

endif

	$(COMPILE) $(INCLUDE) -g -O -o $@ $<
	
clean:
	-rm $(OBJS) $(EXECUTABLE)

install:
	sh installscript