PROJECT = robot-control
SOURCES = $(wildcard *.cpp)
HEADERS = $(filter-out main.hpp, $(SOURCES:.cpp=.hpp))
OBJECTS = $(SOURCES:.cpp=.o)
CXX = g++
CXXFLAGS = -Wall -Wextra -Wno-unused-parameter -g -O2 -std=c++98 -pedantic \
           -D DEBUG -D GRAPHICS_ENABLE #-D CHECK_OBSTACLES
LDLIBS = -lglfw -lGL -lGLEW -lX11 -lXau -ldl -pthread -lm
CTAGS = /usr/bin/ctags
INSTALL = /usr/bin/install
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

memcheck: $(PROJECT)
	valgrind -s --leak-check=full ./$(PROJECT)

tar:
	tar -cf $(PROJECT).tar $(SOURCES) $(HEADERS) Makefile README.md

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

