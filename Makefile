PROJECT = robot-control
SOURCES = $(wildcard *.cpp)
HEADERS = $(filter-out main.hpp, $(SOURCES:.cpp=.hpp))
OBJECTS = $(SOURCES:.cpp=.o)
CXX = g++
CXXFLAGS = -Wall -Wextra -g -O2 -std=c++98 -D DEBUG=1 # -D TEST_VERSION=1
LDLIBS = -lglfw -lGL -lGLEW -lX11 -lXrandr -pthread -lm
CTAGS = ctags
INSTALL = install
PREFIX = /usr/local

$(PROJECT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

deps.mk: $(SOURCES) Makefile
	$(CXX) -MM $(SOURCES) > $@

tags: $(SOURCES) $(HEADERS)
	$(CTAGS) $(SOURCES) $(HEADERS)

run: $(PROJECT)
	./$(PROJECT)

memcheck:
	valgrind -s --track-origins=yes --leak-check=full ./$(PROJECT)

tar:
	tar -cf $(PROJECT).tar $(SOURCES) $(HEADERS) Makefile README.txt

clean:
	rm -f $(PROJECT) *.o deps.mk tags

install: $(PROJECT)
	$(INSTALL) $(PROJECT) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(PROJECT)

ifneq (unistall, $(MAKECMDGOALS))
ifneq (clean, $(MAKECMDGOALS))
ifneq (tags, $(MAKECMDGOALS))
ifneq (tar, $(MAKECMDGOALS))
-include deps.mk
endif
endif
endif
endif

