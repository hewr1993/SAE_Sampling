CC=g++
CFLAGS=-c -Wall --std=c++11 -g -ggdb  
LDFLAGS=
SOURCES=triangle.cpp matrix.cpp qr_uptriangle.cpp matrix_init.cpp multiplication.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

