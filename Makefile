CC = g++
CFLAGS = -std=c++17

organic:
	$(CC) $(CFLAGS) src/* -l rtaudio -o bin/organic