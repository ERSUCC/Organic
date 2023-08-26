CC = g++
CFLAGS = -std=c++11

organic: audiosource parameter event
	$(CC) $(CFLAGS) src/organic.cpp -l rtaudio bin/audiosource bin/parameter bin/event -o bin/organic

audiosource:
	$(CC) -c $(CFLAGS) src/audiosource.cpp -o bin/audiosource

parameter:
	$(CC) -c $(CFLAGS) src/parameter.cpp -o bin/parameter

event:
	$(CC) -c $(CFLAGS) src/event.cpp -o bin/event