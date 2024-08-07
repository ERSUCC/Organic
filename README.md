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

The following section details the optional inputs, for more advanced use of Organic.

### Program Arguments

--time *number*: Set the runtime of the program in milliseconds. If unspecified, the program will run infinitely.

--export *string*: Render the program to the specified audio file instead of playing back in time. Must be used in conjunction with --time.

--mono: Use mono audio for the program. If not included, the program will run in stereo.

## Organic Language Specification

Functions are used to create various audio-related objects, such as audio sources like sine waves or modulators like LFOs. Each function has various inputs, all of which are optional and have default values if not specified. See the following example for a few different ways to call a function.

```
// a valid call to the sine function, but will create a sine wave with the defaults of 0 volume and 0 frequency

sine()

// creates a sine wave at full volume with a frequency of 220 hz

sine(volume: 1, frequency: 220)
```

Variables are defined and assigned to using the equals sign, as in the following example:

```
my_var = 440
```

Variables can only be used after they are defined, but they can be assigned to multiple times. Note that assigning a new value to a variable after using it will also cause the prior uses to be reassigned. To prevent this, type a '#' in front of the variable name when it is used. The following two examples show this key difference:

```
note = 220

sine(volume: 1, frequency: note)

// this reassignment will affect both sine waves, creating two of the same frequency, 440 hz

note = 440

sine(volume: 1, frequency: note)
```

```
note = 220

sine(volume: 1, frequency: #note)

// this reassignment will only affect the second sine wave, creating one of each frequency

note = 440

sine(volume: 1, frequency: note)
```

Some functions return a value, which can be assigned to a variable or used as an input to another function. This can be especially useful when creating complex modulations that will be used in multiple places, as in the following example:

```
// note that sequence accepts a list of modulators as an input, defined with parentheses and separated with commas

freq = sequence(values: (
    sweep(from: 110, to: 220, length: 1000),
    sweep(from: 220, to: 110, length: 1000)
))

sine(volume: 1, frequency: freq)
sine(volume: 1, frequency: freq * 2)
sine(volume: 1, frequency: freq * 4)
sine(volume: 1, frequency: freq * 8)
```

If you have a section of code that you want to be executed multiple times, either with the same or with different values, you can define your own function to more easily duplicate those lines of code. Functions are defined as follows:

```
my-function(my-input-1, my-input-2) = {
    // your code here
}
```

The above code example would create a function called "my-function" with two named inputs, "my-input-1" and "my-input-2", which when called would run the code placed in between the curly braces. You can call a function you define just like any other function in Organic, for example:

```
my-function(my-input-1: 100, my-input-2: 433)
```

One special function, `perform`, allows you to schedule functions to be run in the future, rather than being executed immediately. You can use `perform` as follows:

```
// define a function to create a triangle wave at a given frequency

make-triangle(frequency) = {
    triangle(volume: 1, frequency: frequency)
}

// call the function immediately to create the first note

make-triangle(frequency: 220)

// wait 5 seconds, then double the first note up an octave

perform(function: make-triangle(frequency: 440), delay: 5000)
```

The examples above show minimal examples of all key aspects of the language and its syntax. More examples and detailed descriptions of functions can be found in the Organic documentation (TODO).

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

Organic uses AudioFile for audio file input and output, which can be found here: [https://github.com/adamstark/AudioFile](https://github.com/adamstark/AudioFile)
