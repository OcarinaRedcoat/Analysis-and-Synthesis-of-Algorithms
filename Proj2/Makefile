SOURCE = p2.cpp
TARGET = p2
CC = g++
FLAGS = -Wall -Wextra -Wpedantic -Wformat -g

all: p2

p2: p2.cpp
	$(CC) $(SOURCE) -o $(TARGET) $(FLAGS)

clean: 
	@echo 
	@echo ----  Cleaning project ...  ----
	@echo 
	rm -f $(TARGET) ./tests/*.my.out ./tests/*.diff massif.out.*
	@echo
	@echo ----  Very nice!  ----

run:
	./test.sh