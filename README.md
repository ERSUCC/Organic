# Organic

Organic is a programming language for generative audio synthesis, with the goal of providing electronic musicians with a flexible yet easy-to-learn way to create unique music.

## Table of Contents

1. [Using Organic](#using-organic)
2. [Organic Syntax](#organic-syntax)
3. [Building Organic](#building-organic)
4. [Credits](#credits)

## Using Organic

Organic is a command-line compiler, run from the Command Prompt (Windows) or the Terminal (Mac/Linux). To run it, first open the command-line and navigate to the location where you installed the program. If you're on Windows, run the command `organic` (Windows) or `./organic` (Mac/Linux). In both cases, you will need to provide at least one input, which is the path to your Organic code. Here is a full example (Mac/Linux):

```
./organic /Users/johndoe/Documents/play_a_sine.organic
```

The following section details the optional inputs, for more advanced use of Organic.

### Program Arguments

--time *number*: Set the runtime of the program in milliseconds. If unspecified, the program will run infinitely.

--export *string*: Render the program to the specified audio file instead of playing back in time. Must be used in conjunction with --time.

--mono: Use mono audio for the program. If not included, the program will run in stereo.

## Organic Language Specification

TBD

## Building Organic

This section details the process of building Organic yourself, rather than using one of the released binary files.

Currently the only provided method for building Organic is CMake, which can be found here: [https://cmake.org/download/](https://cmake.org/download/). Make sure CMake is installed and working properly before continuing.

Once you have downloaded the Organic source code, open the command-line and navigate to the directory where you installed the source code. To build with CMake, run the following commands:

```
mkdir build
cd build
cmake ..
cmake --build .
```

This will create the `organic` binary in the `build` directory (Mac/Linux) or the `build/Debug` directory (Windows). Move it wherever you would like, then return to [Using Organic](#using-organic) to continue.

## Credits

Organic uses RtAudio for cross-platform real-time audio output, which can be found here: [https://github.com/thestk/rtaudio](https://github.com/thestk/rtaudio).

Organic uses libsndfile for audio file input and output, which can be found here: [https://github.com/libsndfile/libsndfile](https://github.com/libsndfile/libsndfile)
