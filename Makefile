PROJECT = simulator
SRCMODULES = main.cpp manager.cpp vehicle.cpp graphobject.cpp localmap.cpp vector2d.cpp
OBJMODULES = $(SRCMODULES:.cpp=.o)
CXX = g++
CXXFLAGS = -Wall -Wextra -g -O2 -std=c++98
LDLIBS = -lglfw -lGL -lGLEW -lX11 -lXrandr -pthread -lm

$(PROJECT): $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

deps.mk: $(SRCMODULES) Makefile
	$(CXX) -MM $(SRCMODULES) > $@

clean:
	rm -f $(PROJECT) deps.mk *.o

run: $(PROJECT)
	./$(PROJECT)

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

