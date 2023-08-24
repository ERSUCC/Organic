CC = g++
CPPFLAGS = -std=c++11

organic: audiosource
	$(CC) $(CPPFLAGS) src/organic.cpp -l rtaudio bin/audiosource -o bin/organic

audiosource:
	$(CC) -c $(CPPFLAGS) src/audiosource.cpp -o bin/audiosource