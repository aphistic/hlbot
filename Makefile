EXECUTABLE = hlbot 

CC = g++
CFLAGS = -Wall
COMPILE = $(CC) $(CFLAGS) -c
INCLUDE = -Iinclude
LIBS = -ldl
SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,src/%.o,$(SRCS))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LIBS) -g -O -o $(EXECUTABLE) $(OBJS)

%.o: %.cpp
	$(COMPILE) $(INCLUDE) -g -O -o $@ $<
	
clean:
	-rm $(OBJS) $(EXECUTABLE)

install:
	@echo "There isn't a make install at the moment."
