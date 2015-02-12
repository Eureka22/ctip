CC=g++
CFLAGS=-c -Wall --std=c++11 -g -ggdb  
LDFLAGS=
SOURCES=Main.cpp Data.cpp Util.cpp Model.cpp Sampler.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm *.o
