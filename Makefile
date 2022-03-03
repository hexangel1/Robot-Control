PROJECT = robot-control
SOURCES = $(wildcard *.cpp)
HEADERS = $(filter-out main.hpp, $(SOURCES:.cpp=.hpp))
OBJECTS = $(SOURCES:.cpp=.o)
CXX = g++
CXXFLAGS = -Wall -g -O2 -std=c++98 -D DEBUG=1
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

memcheck:
	valgrind -s --leak-check=full ./$(PROJECT)

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

