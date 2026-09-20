What is Organic?
================

Organic is a programming language for algorithmic :term:`audio synthesis`, designed to provide a simple yet flexible way
to create drones, ambient soundscapes, and unique effects. Organic makes automation easy on any time scale, from
sub-millisecond frequency modulation to hour-long evolutions.

Organic provides a programming environment inspired by the concepts of
`modular synthesis <https://en.wikipedia.org/wiki/Modular_synthesizer>`__. Like a modular synthesizer, Organic provides
methods by which every parameter can be modulated by almost anything else, and every input is continuously evaluated
to produce new output.

##################
Who should use it?
##################

If you're interested in creating any kind of digital music, Organic is for you. Organic was created with a wide range of
programming skill levels in mind, so even if you've only ever created music with a DAW, it won't take long to get
started with Organic. Also, using Organic doesn't mean you have to abandon the tools you're familiar with; you can use
it alongside another music creation tool to generate portions of a larger track, or just to brainstorm and play with new
sounds.

#################
Design Principles
#################

The Organic programming language is not a direct descendant of any existing programming language, and it takes
inspiration from a variety of existing tools. The core principles of the language are simplicity, readability,
extendability, and composability. The following sections explain the ways in which Organic upholds these principles.

----------
Simplicity
----------

Organic is a relatively simple language to learn, due to its non-technical purpose. Unlike many other programming
languages, Organic has no reserved words for control flow, variable declarations, or function definitions. It also
features a reduced set of operators, focusing on familiar mathematical operators like addition and subtraction and
delegating more complex operators like modulo and boolean operators to functions. Organic also does not allow the
creation of custom data types, so once a user has learned the :organic:mono:`7` core types, they will be able to read
and understand any Organic program.

Another way that Organic reduces complexity is by ensuring that no fatal runtime errors can occur. In some programming
languages, unexpected crashes can be confusing, often providing either no information or an overload of technical
details. By contrast, the structure of Organic allows the compiler to catch all technical errors and some conceptual
errors, meaning that a program that passes compilation is guaranteed to run forever and produce output that at least
partially matches the user's intent.

-----------
Readability
-----------

One way that Organic improves readability is by avoiding shortened function names. Many programming languages shorten
the names of functions to make the code less verbose and faster to type, but this can make it more confusing for anyone
who isn't familiar with the language to determine the result of a program. For example, Organic uses
:organic:code:function:`oscillator` instead of :organic:code:function:`osc`, :organic:code:function:`sequence` instead
of :organic:code:function:`seq`, and :organic:code:function:`absolute` instead of :organic:code:function:`abs`.

Another way that Organic improves readability is by requiring that all function inputs be named. Most programming
languages require users to remember the order of inputs to each function. Although this is very reasonable from a
technical perspective, it can make it more difficult to get acquainted with the standard library of functions, and it
can make it more difficult for new users to quickly determine the purpose of any given line of code. By requiring named
inputs, Organic allows users to specify inputs in the order that makes the most sense to them, and it allows functions
to have a large number of inputs without causing mixups between inputs of the same type.

-------------
Extendability
-------------

Organic improves extendability by providing sensible default values for any inputs that aren't required to achieve the
basic behavior of a function. This allows advanced users to configure each function to an arbitrary degree, while
preventing new users from getting bogged down by complexity. For example, the function :organic:code:function:`sine` has
only one required input, :organic:code:`frequency`. Users who are trying the language for the first time can use just
this input and immediately get meaningful audio output. But after becoming more familiar with the language, it's easy to
extend the program from there by adding optional inputs such as :organic:code:`pan` and :organic:code:`effects`, without
needing to change the overall structure of the program or refactor any existing code.

-------------
Composability
-------------

Organic is structured in a way that allows most functions to be nested inside most other functions, allowing users to
create complex, expressive modulations that can't be easily achieved with standard music creation techniques. There are
also a variety of built-in functions that are able to operate on any type, creating an intuitive and flexible system for
parameter control.
