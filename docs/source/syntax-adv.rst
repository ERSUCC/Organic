Advanced Syntax
===============

This section of the documentation explains the specifics of the language, providing details about valid and invalid
syntax. If you haven't yet read the :doc:`syntax` page, you should do so before reading this page to get a basic idea of
how the language is constructed.

.. _types:

#####
Types
#####

There are 5 integral types in Organic: numbers, booleans, constants, strings, and lists. There are also 3 complex types
that can be returned by built-in functions: audio sources, effects, and lambda functions. Organic does not support the
creation of custom types or data structures, so all built-in functions and all user-defined functions operate
exclusively on the above types. The following sections detail the creation and use of these 8 types.

-------
Numbers
-------

All numbers in Organic are internally represented as double-precision floating point numbers. Numbers can be written as
integers or decimals, both with an optional preceding minus sign for negation. Organic does not support scientific
notation or any other alternative formats for notating numeric values. For example, the following values are all valid
numbers:

.. code-block::

   42
   -61
   1234.5678
   -0.001

Organic supports the following arithmetic operators, all used as infix operators:

* :code:`+` (Add)
* :code:`-` (Subtract)
* :code:`*` (Multiply)
* :code:`/` (Divide)
* :code:`^` (Power)

--------
Booleans
--------

Organic supports the boolean literals :code:`true` and :code:`false`, as well as the following boolean operators, all
used as infix operators:

* :code:`<` (Less than)
* :code:`>` (Greater than)
* :code:`<=` (Less than or equal to)
* :code:`>=` (Greater than or equal to)
* :code:`==` (Equal to)

---------
Constants
---------

The first of two types of constants in Organic are note names. As a shortcut for pitches in the Western chromatic scale,
some frequencies can be written as note names instead of numbers. For example, :code:`a4` refers to the frequency 440
:term:`Hz <hertz>`, and will be converted to the numeric value :code:`440` during parsing. This means that note names
can also be used in arithmetic expressions, such as :code:`c2 * 2`, which would produce a frequency twice that of
:code:`c2`. All note names can optionally include :code:`s` or :code:`f` before the octave number, to indicate a sharp
or flat note, respectively.

The second type of constant in Organic is an enumeration, which is a set of reserved names that are used to control the
behavior of specific built-in functions. Enumeration constants cannot be used in arithmetic or boolean expressions, as
they cannot be interpreted as numeric or boolean values. A list of these constants can be found on the :doc:`dictionary`
page.

-------
Strings
-------

Strings are denoted by a sequence of characters surrounded by double quotation marks. Currently, strings are only used
to refer to file paths, like the following example:

.. code-block::

   "/Users/johndoe/verycoolsound.wav"

If necessary, double quotation marks within the contents of a string can be escaped with a backslash, like this:

.. code-block::

   "\"quoteme\".txt"

-----
Lists
-----

Lists are denoted by one or more comma-separated values or expressions, surrounded by square brackets. For example, the
following are all valid lists:

.. code-block::

   [ 77 ]
   [ 1, 2, 3, 4 ]
   [ 42 + 13, -0.1 ^ (5 + 5), 10 ]

Lists can be of any type, not just numbers, but all the elements of a list must be of the same type. For example, you
could also create a list of boolean values:

.. code-block::

   [ true, 55 > 22, false ]

-------------
Audio Sources
-------------

Audio sources are the most important type in Organic, because they are the values that actually produce audio output.
Audio sources can be stored in variables or returned from functions, but are only sent to output if returned into the
global scope. Audio sources are not an integral type, so they must be created using built-in functions like :code:`sine`
and :code:`saw`.

-------
Effects
-------

Effects are used to manipulate the raw audio output of audio sources, and are therefore only valid when used in the
context of an audio source. All built-in functions that return an audio source have an optional :code:`effects` input,
with which you can specify a list of effects that you would like to apply to that audio source. Effects are not an
integral type, so they must be created using built-in functions like :code:`delay` and :code:`reverb`.

----------------
Lambda Functions
----------------

Lambda functions allow you to use functions with input values provided internally by Organic, rather than input values
provided manually in your program. Any user-defined function can be used as a lambda function if it is referenced
without any parentheses or arguments after its name. One example of this is the :code:`oscillator` function, which
allows you to define a custom oscillating waveform. This waveform is provided as a lambda function with one input,
representing the phase of the oscillator. At runtime, Organic will use the provided function to construct the audio
output for the oscillator, filling in the correct phase values automatically.

#########
Variables
#########

Variables in Organic are immutable and must have a unique name within their scope. Variables can be defined anywhere in
an Organic program, however they can only be used in statements after their definition. Due to the immutability of
variables in Organic, the type of each variable can be determined from its value without explicit type declarations.
Variable names must start with a letter or an underscore, and can contain letters, numbers, hyphens, and underscores
after the first character. Organic is a :term:`case-sensitive` language, and you can use any case that you desire,
however the standard is to use :term:`kebab case`. For example, the following are all valid variable definitions:

.. code-block::

   frequency = 440
   super-high-frequency = 15000
   blistering_speed = true
   _LENGTH_OF_SOUND = 1000 * 60
   Thing42 = 22 + 20

#########
Functions
#########

Functions in Organic are primarily used to create and configure objects of the 3 complex types named in the :ref:`types`
section above. Because Organic is a declarative language, neither built-in functions nor user-defined functions can have
side effects; they simply construct and return a value based on the inputs to the function. In Organic, all function
arguments must be named, but they can appear in any order.

All functions in Organic return a value, which can optionally be assigned to a variable or used as the input to a
function. Any function that returns an audio source can also be used directly in the global scope, in which case the
returned audio source will be added to the program output.

####################
Function Definitions
####################

Function definitions can occur anywhere in an Organic program, including within another function definition. Like
variables, user-defined functions can only be used in the scope where they are defined, and they can only be used after
they are defined. Names of user-defined functions follow the same rules as variable names, and function names cannot
conflict with variable names. All arguments in a function definition must provide default values, which allows Organic
to determine the type of each argument. The last instruction in a function body serves as its return value, and any
prior instructions in the function body other than variable and function definitions will be ignored.

#############
Value Lengths
#############

In Organic, each function generates a series of values over time, rather than just a single value at the instant it is
used. Some functions generate infinite series of values, and some have set lengths. When you use a function, make sure
you know what its length will be, so that you can prevent unintended silences or incorrect values.

If a function nested inside another function stops before the surrounding function does, the nested function will switch
to a reasonable default value based on its type. The default value for each type is as follows:

- Number: :code:`0`
- Boolean: :code:`false`
- Sequence Order: :code:`forward`
- Random Type: :code:`stay`
- Round Direction: :code:`nearest`
- String: :code:`""`
- List: :code:`[]`
- Audio Source: Silence
- Effect: No effect
- Lambda Function: Default value of return type
