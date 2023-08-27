CC = g++
CFLAGS = -std=c++11

organic:
	$(CC) $(CFLAGS) src/* -l rtaudio -o bin/organic