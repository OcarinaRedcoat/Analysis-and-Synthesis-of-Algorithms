SOURCE = p1.cpp
TARGET = p1
CC = g++
FLAGS = -Wall -Wextra -pedantic

all: p1

p1: p1.cpp
	$(CC) $(SOURCE) -o $(TARGET) $(FLAGS)

clean: 
	@echo 
	@echo ----  Cleaning project ...  ----
	@echo 
	rm -f $(TARGET) ./tests/*.my.out ./tests/*.diff
	@echo
	@echo ----  Very nice!  ----
