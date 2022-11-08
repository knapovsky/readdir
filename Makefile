########################################################################
# Makefile pro IPK Projekt #2
# autor:	Martin Knapovsky
#			xknapo02@stud.fit.vutbr.cz
########################################################################

S1 = ./client.cpp
S2 = ./server.cpp
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
RFLAGS = -std=c++98 -W -Wall -pedantic

all : $(S1) $(S2) ./server.h ./client.h
	$(CC) $(LFLAGS) $(S1) -o client
	$(CC) $(LFLAGS) $(S2) -o server
