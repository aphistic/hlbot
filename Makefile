EXECUTABLE = hlbot 

CC = g++
CFLAGS = -Wall
COMPILE = $(CC) $(CFLAGS) -c
INCLUDE = -Iinclude
SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,src/%.o,$(SRCS))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS)

%.o: %.cpp
	$(COMPILE) $(INCLUDE) -o $@ $<
	
clean:
	-rm $(OBJS) $(EXECUTABLE)

install:
	@echo "There isn't a make install at the moment."
