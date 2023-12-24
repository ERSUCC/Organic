# Organic

Organic is a programming language for generative audio synthesis, with the goal of providing electronic musicians with a flexible yet easy-to-learn way to create unique music.

## Table of Contents

1. [Using Organic](#using-organic)
2. [Organic Syntax](#organic-syntax)
3. [Building Organic](#building-organic)
4. [Credits](#credits)

## Using Organic

Organic is a command-line interpreter, run from the Command Prompt (Windows) or the Terminal (Mac/Linux). To run it, first open the command-line and navigate to the location where you installed the program. If you're on Windows, run the command `organic` (Windows) or `./organic` (Mac/Linux). In both cases, you will need to provide at least one input, which is the path to your Organic code. Here is a full example (Mac/Linux):

```
./organic /Users/johndoe/Documents/play_a_sine.organic
```

### Program Arguments

TODO

## Organic Syntax

TODO

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

This will create the `organic` binary in the `build` directory. Move it wherever you would like, then return to [Using Organic](#using-organic) to continue.

## Credits

Organic relies on RtAudio for cross-platform audio output, which can be found here: [https://github.com/thestk/rtaudio](https://github.com/thestk/rtaudio).
