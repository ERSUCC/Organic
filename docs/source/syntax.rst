Basic Syntax
============

This section of the documentation provides an overview of the syntax of the Organic programming language, explaining all
of the concepts you need to know in order to interpret and write simple Organic programs. Note that some specific
details of the language are skipped here to facilitate the learning process, so don't worry if you have trouble
experimenting with any of the examples on this page. Once you get acquainted with the general structure of the language,
you can then read the :doc:`syntax-adv` page, which provides more specific details about what you can and cannot do in
Organic.

#################
Program Structure
#################

An Organic program consists of a list of instructions for the computer, each on a separate line. There are only three
types of instructions, which you can use as many times as you want and in any order. For now, we'll focus on the first
two: creating variables and using functions.

------------------
Creating Variables
------------------

If you have a complex instruction or a piece of information that you use in multiple places in your program, it is often
helpful to give it a name to make the code more readable and to help you keep track of the purpose of each instruction.
In an Organic program, this name is known as a variable, and the information it contains is known as its value. Creating
a variable, also known as defining or declaring a variable, might look something like this:

.. code-block::

   frequency = 440

The above example creates a variable with the name :code:`frequency` and the value :code:`440`. In future instructions
in your program, you can then use the name :code:`frequency` to refer to the number :code:`440`.

In Organic, a variable's value doesn't have to be a single number. For example, it can instead be the result of a
mathematical expression:

.. code-block::

   frequency = (2 * 12) + 100

The above example creates a variable with the name :code:`frequency` and the value :code:`124`. In future instructions
in your program, you can then use the name :code:`frequency` to refer to the number :code:`124`, without needing to type
or evaluate the expression :code:`(2 * 12) + 100` again.

Another common way to create a variable is by defining it in terms of a previously created variable, such as the
following example:

.. code-block::

   frequency = 440

   higher = frequency + 100
   really-high = frequency + 200

The above example first creates a variable with the name :code:`frequency` and the value :code:`440`. Then, two more
variables with the names :code:`higher` and :code:`really-high` are created based on the value of :code:`frequency`.
These variables will then have the values :code:`540` and :code:`640`, respectively.

---------------
Using Functions
---------------

To create sounds and control their parameters, Organic comes with a variety of built-in pieces of code called functions.
Like variables, functions are referenced by name. However, unlike variables, each function is configurable so that it
can return a variety of different values based on how you use it. When you use a function, you can configure it by
assigning values to a set of special variables, which are known as its inputs. For example, recall the code from
:ref:`first-program`:

.. code-block::

   sine(frequency: 440)

The above example uses the function named :code:`sine`, assigning the number :code:`440` to the input named
:code:`frequency`. As you heard when you ran this example program earlier, this creates a sine wave with the specified
frequency; in this case, 440 Hz. The parentheses after the name :code:`sine` tell Organic that you want to use and
configure a function, instead of using a variable.

If you want to configure multiple inputs at once, you can do so by separating them with commas inside the parentheses,
like in the following example:

.. code-block::

   sine(volume: 0.5, frequency: 440)

The above example is similar to the previous one, but in addition to the input named :code:`frequency`, the input named
:code:`volume` is also specified.

As you may have noticed, even though you didn't provide a value for the :code:`volume` input in the first example, you
still heard a sine wave when you ran the program. This is because the :code:`volume` input for the :code:`sine` function
is optional, and Organic will set it to :code:`1` (full volume) if you don't provide a value yourself. By contrast, the
:code:`frequency` input for the :code:`sine` function is required, and Organic won't let you run the program if you
forget to provide a value for it.

Like the :code:`sine` function, every built-in function has some core inputs that are required, and some that are
optional. These optional inputs can be used to fine-tune the result of the function, or in some cases to access more
advanced functionality. Optional inputs don't have to be specified after required inputs; inputs can appear in any order
for all functions. Once you are familiar with the syntax of the language, you can read the :doc:`dictionary` page for a
comprehensive list of built-in functions and their inputs.

You can also create your own functions in order to extend the functionality of the built-in functions or to help you
structure your code more efficiently. The details of this will be covered in a later section, since it's useful to know
more about the built-in functions before creating your own.

##################
Types of Functions
##################

So far, you've only seen one function, the :code:`sine` function. There are more functions like :code:`sine` that create
audio sources, but there are also a few other types of functions in Organic that can be used to create other values,
such as automated numbers and audio effects. This section introduces the other types of functions, explaining their
purpose and providing a few basic examples.

-----------
Controllers
-----------

The most common type of function in Organic is a controller, which can be used to automate function inputs over time.
For example, consider the :code:`sweep` function:

.. code-block::

   sweep(from: 0, to: 1, length: 1000)

The above example produces a number that starts at 0 and steadily increases to 1 over the course of 1000 milliseconds,
or 1 second. This doesn't do much on its own, but if you use it as the value for an input to another function, that
input will change over time instead of remaining constant. For example, you can use the :code:`sweep` function to extend
the original sine wave example:

.. code-block::

   sine(frequency: sweep(from: 220, to: 440, length: 1000))

The above example creates a sine wave with a frequency that increases over time from 220 Hz to 440 Hz. However, unlike
in the original example, the sound stops after the frequency reaches 440 Hz, instead of continuing forever. One way to
solve this issue is with another controller function, called :code:`repeat`:

.. code-block::

   sine(frequency: repeat(value: sweep(from: 220, to: 440, length: 1000)))

In the above example, the :code:`sweep` function is no longer used directly as the value for the :code:`sine` function's
:code:`frequency` input. Instead, it is used in the :code:`repeat` function's :code:`value` input, which in turn is used
as the value for the :code:`sine` function's :code:`frequency` input. Now, instead of stopping after one second, it
repeats infinitely, setting the frequency back to 220 Hz every second. Although it can be difficult to interpret at
first, nesting controllers like this allows you to create much more dynamic and interesting sounds.

-----------------------
Conditional Controllers
-----------------------

Another type of controller is a conditional controller, which allows you to switch between multiple values while the
program is running based on the result of a conditional statement. A conditional statement is a statement like "if the
frequency is less than 440 Hz, play the sound, otherwise don't play the sound". In Organic, this can be done with the
:code:`if` function, like in the following example:

.. code-block::

   freq = repeat(value: sweep(from: 220, to: 880, length: 2000))
   vol = if(condition: frequency < 440, is-true: 1, is-false: 0)

The above example first creates a variable named :code:`freq` to contain the frequency, using the :code:`sweep` function
and the :code:`repeat` function as you saw in the previous section to create a rising pitch that resets every 2 seconds.
Then, it creates a variable named :code:`vol` to contain the volume, which is either 1 (full volume) or 0 (silent)
depending on the current frequency. Next, you can use these variables as the inputs to the :code:`sine` function to hear
how it sounds:

.. code-block::

   sine(volume: vol, frequency: freq)

When you're ready, run the program. You should hear the lower portion of the rising tone, followed by a moment of
silence during what would be the upper portion of the rising tone. Now, try switching the 1 and the 0 in the :code:`if`
function, and see how it sounds compared to the first example. You should hear the exact opposite: a moment of silence,
followed by the upper portion of the rising tone.

-------
Effects
-------

The final type of function is an effect, which can only be used along with an audio source like :code:`sine`. Effects
alter the output of an audio source after its creation, directly manipulating the stream of audio data before it is
played. This allows you to fine-tune more abstract parameters of a sound, such as :term:`timbre`, regardless of which
function was used to create the original sound. One common example of an effect is reverberation, or reverb for short.
Reverb adds echoes to a sound, making it seem like it was produced in a much larger space like a church or a cave. In
Organic, you can create a reverb effect like this:

.. code-block::

   reverb(length: 10000)

The above example creates a reverb effect with a 10000 millisecond, or 10 second, decay time. To use it with an audio
source, you can provide it as the value for that audio source's :code:`effects` input. To hear what this sounds like,
let's build up an example using what you've learned about controllers and effects. Start by creating a basic saw wave
at 440 Hz:

.. code-block::

   saw(frequency: 440)

A saw wave is a much harsher and more complex sound than a sine wave, so it should help you hear the reverb effect more
clearly. Now, add a :code:`sweep` for the :code:`volume` input:

.. code-block::

   saw(volume: sweep(from: 1, to: 0, length: 500), frequency: 440)

This will be the final sound before adding reverb, so try running the program now to see how it sounds. Finally, add a
:code:`reverb` effect:

.. code-block::

   saw(volume: sweep(from: 1, to: 0, length: 500), frequency: 440, effects: [
      reverb(length: 10000)
   ])

Before you run this example, notice that unlike the other inputs, the :code:`reverb` effect has a pair of square
brackets, :code:`[` and :code:`]`, surrounding it. This is because the :code:`effects` input expects you to provide a
list of effects, instead of just a single effect. In Organic, whenever a function input expects a list, you can group
your desired values together by surrounding them with square brackets and separating them with commas. In this example,
there is only one effect, but you still need to surround it with square brackets so that Organic knows it's a list.

##################
Creating Functions
##################

Now that you know how to use the built-in functions, you can learn how to create your own. If you want to use a built-in
function multiple times with slight variations in the inputs, creating your own function can make your program more
readable and easier to change later on. For example, say that you want to create multiple sine waves, each with a rising
frequency, but each ending at a different frequency. With the techniques you've learned so far, this might look like the
following example:

.. code-block::

   sine(frequency: repeat(value: sweep(from: 220, to: 330, length: 1000)))
   sine(frequency: repeat(value: sweep(from: 220, to: 440, length: 1000)))
   sine(frequency: repeat(value: sweep(from: 220, to: 550, length: 1000)))

This works, but there is a lot of repeated code. If you create a function that produces one of these sine waves, and add
an input for the ending frequency, you can make your code much simpler:

.. code-block::

   create-sine(end: 220) = {
      sine(frequency: repeat(value: sweep(from: 220, to: end, length: 1000)))
   }

   create-sine(end: 330)
   create-sine(end: 440)
   create-sine(end: 550)

The above example creates a new function named :code:`create-sine`. It has one input named :code:`end`, whose default
value is :code:`220` if no value is provided when the function gets used. The code to be run when the function is used
goes inside the curly brackets, :code:`{` and :code:`}`. In this example, the function only has one line of code, which
uses the :code:`sine` function to create a sine wave. Once the function is created, you can use it as many times as you
would like, only using the name of the function instead of typing out all the code inside of it each time. In the above
example, :code:`create-sine` is used three times, each of which runs the code inside the function, creating a total of
three new sine waves.

##########
Next Steps
##########

Congratulations, you now know all the core concepts of the syntax of the Organic programming language! For now, you can
keep experimenting with the functions you have learned so far, nesting them further to create more unique sounds. Once
you feel ready, take a look at the :doc:`dictionary` page to learn about the other built-in functions in Organic. After
that, if you want to dive deeper into the specifics of the language, you can read the :doc:`syntax-adv` page. Good luck!
