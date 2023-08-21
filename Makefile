CC = gcc
FLAGS = -Wall -Werror -Wextra

all : main

rebuild : clear all

clear :
	rm *.out *.exe

main :
	$(CC) $(FLAGS) game_of_life.c -o game_of_life -lncurses -DNCURSES_STATIC