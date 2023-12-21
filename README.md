# Organic

Organic is a programming language for generative audio synthesis, with the goal of providing electronic musicians with a flexible yet easy-to-learn way to create unique music.

## Table of Contents

1. [Using Organic](#using-organic)
2. [Organic Syntax](#organic-syntax)

## Using Organic

Organic is a command-line interpreter, run from the Command Prompt (Windows) or the Terminal (Mac/Linux). To run it, first open the command-line and navigate to the location where you installed the program. If you're on Windows, run the command `organic` (Windows) or `./organic` (Mac/Linux). In both cases, you will need to provide at least one input, which is the path to your Organic code. Here is a full example (Mac/Linux):

```
./organic /Users/johndoe/Documents/play_a_sine.organic
```

This is all you need to know for the simplest usage of Organic, but the next section further details the `organic` command and its other arguments.

### Program Arguments

TODO

## Organic Syntax

The first fundamental building block of an Organic program is a variable, a symbolic name that represents a value of your choice. The name of a variable can contain lowercase and uppercase letters, numbers, hyphens, and underscores, but it must start with a letter. You can define a variable like this:

```
my_variable = 10
```

This would create a variable called `my_variable` and give it the value 10.

The second fundamental building block of an Organic program is a function, a special instruction that takes in a series of inputs and returns a specific output. For example, you can use the `sine` function to create a sine wave that plays infinitely:

```
sine(frequency: 440, volume: 1)
```

This would create a sine wave with a frequency of 440 and a volume of 1. In this example, the name of the function is `sine`, and the inputs of the function are `frequency` and `volume`. Each input has a name and a value separated by a colon, and the inputs are separated by a comma. Note that a function can have multiple inputs, but each input has a default value and is optional.

In Organic, variables can store any value, including the output of a function. For example, you could store the output of the previous example in a variable:

```
my_sine = sine(frequency: 440, volume: 1)
```

Now that you know how to use variables and functions, you can use them in tandem:

```
freq = 440

sine(frequency: freq)
square(frequency: freq)
saw(frequency: freq)
```

In this example program, the variable `freq` makes it easier to change the frequency of all three oscillators at the same time, ensuring that they always have the same value regardless of what is assigned to `freq`.
