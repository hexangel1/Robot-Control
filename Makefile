PROJECT = robot-control
SOURCES = main.cpp manager.cpp vehicle.cpp\
          graphobject.cpp environment.cpp vector2d.cpp
HEADERS = $(filter-out main.hpp, $(SOURCES:.cpp=.hpp))
OBJECTS = $(SOURCES:.cpp=.o)
CXX = g++
CXXFLAGS = -Wall -Wextra -g -O2 -std=c++98 -D DEBUG=0
LDLIBS = -lglfw -lGL -lGLEW -lX11 -lXrandr -pthread -lm
CTAGS = /usr/bin/ctags

$(PROJECT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

deps.mk: $(SOURCES) Makefile
	$(CXX) -MM $(SOURCES) > $@

tags: $(SOURCES) $(HEADERS)
	$(CTAGS) $(SOURCES) $(HEADERS)

tar:
	tar -cf $(PROJECT).tar $(SOURCES) $(HEADERS) Makefile README.txt

clean:
	rm -f $(PROJECT) *.o deps.mk tags

run: $(PROJECT)
	./$(PROJECT)

ifneq (clean, $(MAKECMDGOALS))
ifneq (tags, $(MAKECMDGOALS))
ifneq (tar, $(MAKECMDGOALS))
-include deps.mk
endif
endif
endif

