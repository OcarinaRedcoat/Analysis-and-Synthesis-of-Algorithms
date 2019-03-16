SOURCE = p1.cpp
TARGET = p1
CC = g++

all: p1

p1: p1.cpp
	$(CC) $(SOURCE) -o $(TARGET)

clean: 
	@echo 
	@echo Cleaning project ...
	rm -f $(TARGET)
