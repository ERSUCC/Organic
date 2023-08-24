CC = g++
CFLAGS = -std=c++11

organic: audiosource
	$(CC) $(CFLAGS) src/organic.cpp -l rtaudio bin/audiosource -o bin/organic

audiosource:
	$(CC) -c $(CFLAGS) src/audiosource.cpp -o bin/audiosource