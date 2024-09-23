CPPC = g++
CPPFLAGS = -g -std=c++23
WARNINGS = -Wall
LINKS = -lncurses
BINARY = main

OBJECTS = ./obj/main.o ./obj/App.o ./obj/DirectoryEntry.o ./obj/EggLog.o
# ./obj/ObjectName.o

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CPPC) $(CPPFLAGS) $(WARNINGS) -o $@ $^ $(LINKS)

./obj/%.o: %.cpp
	$(CPPC) $(CPPGLAGS) $(WARNINGS) -c -o $@ $^

clean:
	rm -f $(BINARY) $(OBJECTS)
