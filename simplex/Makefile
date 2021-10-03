TARGET = simplex
MODULES = main lpparser lp cmdlparser exception matrix
ADDITIONAL_HEADERS = types

DIST_DIR = $(TARGET)_dir

CC = g++ -Wall -ansi -pedantic
INCLUDES = -I./
EDITOR ?= vi

OBJECTS = $(MODULES:=.o)
C_SOURCES = $(MODULES:=.cpp)
H_SOURCES = $(MODULES:=.hpp)
A_HEADERS = $(ADDITIONAL_HEADERS:=.hpp)

ALL_SOURCES = $(C_SOURCES) $(H_SOURCES) $(A_HEADERS) Makefile

.PHONY = all clean ed dist

all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.cpp %.hpp $(A_HEADERS)
	$(CC) -c $< $(INCLUDES) -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)

ed:
	$(EDITOR) $(ALL_SOURCES) &

dist:
	mkdir $(DIST_DIR)
	cp $(ALL_SOURCES) $(DIST_DIR) 
	tar czf $(TARGET).tgz $(DIST_DIR)
	rm -f -r $(DIST_DIR)
