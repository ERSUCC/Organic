CC = g++
CFLAGS = -std=c++11

ifeq ($(OS), Windows_NT)
	RTLIB = rtaudiowindows
else
	UNAME := $(shell uname -s)
	
	ifeq ($(UNAME), Darwin)
		RTLIB = rtaudio
	endif
	ifeq ($(UNAME), Linux)
		RTLIB = rtaudiolinux
	endif
endif

organic: audiosource
	$(CC) $(CFLAGS) src/organic.cpp -l $(RTLIB) bin/audiosource -o bin/organic

audiosource:
	$(CC) -c $(CFLAGS) src/audiosource.cpp -o bin/audiosource