Dictionary
==========

This section of the documentation details the built-in constants, as well as the behavior of each built-in function and
its inputs.

#########
Constants
#########

Constants are special variables built into the language. You can use them at any time, but you cannot change their
values or create a new variable with the same name as a constant.

-----------------
General Constants
-----------------

:code:`pi`
   A numeric constant, equal to the standard mathematical constant `pi <https://en.wikipedia.org/wiki/Pi>`__, or
   approximately :code:`3.141592`.

:code:`tau`
   A numeric constant, equal to twice the value of :code:`pi`, or approximately :code:`6.283184`.

:code:`e`
   A numeric constant, equal to the standard mathematical constant
   `e <https://en.wikipedia.org/wiki/E_(mathematical_constant)>`__, or approximately :code:`2.71828`.

:code:`true`, :code:`false`
   These are boolean constants. They can be used in place of conditional statements, such as :code:`frequency < 440`, to
   represent a conditional that does not change.

.. _sequence-order:

------------------------
Sequence Order Constants
------------------------

These constants are used in the :code:`order` input of the :code:`sequence` function, to control the order in which the
values in the sequence are traversed.

:code:`forward`
   Traverse the values in the order they are specified.

:code:`backward`
   Traverse the values in the opposite order from which they are specified.

:code:`shuffle`
   Traverse the values in random order, with no repeated values.

.. _random-type:

---------------------
Random Type Constants
---------------------

These constants are used in the :code:`type` input of the :code:`random` function, to control how the returned value
changes in between random number selections.

:code:`stay`
   Keep the returned value constant until the next selection.

:code:`linear`
   Linearly interpolate the returned value between the current selection and the next selection.

.. _round-direction:

-------------------------
Round Direction Constants
-------------------------

These constants are used in the :code:`direction` input of the :code:`round` function, to control the direction in which
the :code:`value` input is rounded.

:code:`nearest`
   Round the value to the nearest whole number. With this option, values ending in .5 will be rounded away from 0.

:code:`up`
   Round the value up to the next whole number. This option always rounds in the positive direction, regardless of sign.

:code:`down`
   Round the value down to the previous whole number. This option always rounds in the negative direction, regardless of
   sign.

-------------------
Note Name Constants
-------------------

These numeric constants can be used to specify frequencies that correspond to the notes in the Western chromatic scale.
The format is the note name, followed by an optional :code:`s` or :code:`f` to indicate a sharp or flat note,
respectively, followed by the octave number from 0-9. For example, :code:`a4` refers to A in the 4th octave, or 440
:term:`Hz <hertz>`, and :code:`fs6` refers to F sharp in the 6th octave, or 1479.98 Hz.

#########
Functions
#########

The following sections detail the behavior and inputs for every built-in function in Organic. Each entry begins with the
function's name, followed by the type of value it returns. The lines below the function name list any required inputs
and their types. Next, the function's standard behavior is described. Finally, any optional inputs are listed, along
with their types and a description of how their use affects the behavior of the function.

-----------
Controllers
-----------

These functions are used to automate the values of other parameters, and can typically be nested to an arbitrary depth.

.. organic:function:: time
   :return: number

   Returns the time in milliseconds since the start of the program.

.. organic:function:: hold
   :required: value ~ anything, length ~ number
   :return: type of :code:`value`

   Returns :code:`value` for :code:`length` milliseconds, then stops.

.. organic:function:: lfo
   :required: from ~ number, to ~ number, length ~ number
   :return: number

   Generates a sinusoidal interpolation between :code:`from` and :code:`to`, with a period equal to :code:`length`
   milliseconds. Stops after one period is completed.

.. organic:function:: sweep
   :required: from ~ number, to ~ number, length ~ number
   :return: number

   Generates a linear interpolation between :code:`from` and :code:`to`, lasting for :code:`length` milliseconds. Stops
   after reaching :code:`to`.

.. organic:function:: sequence
   :required: values ~ list of anything
   :return: type of :code:`values`

   Steps through :code:`values` one value at a time, waiting until each value stops before moving on to the next. Stops
   after each value has been completed once.

   **Optional Inputs**

   .. organic:input:: order
      :type: :code:`forward` | :code:`backward` | :code:`shuffle`
      :default: forward

      Use this input to control the order in which the specified values are traversed. Note that the function will stop
      if the value specified for this input stops. See :ref:`sequence-order` for details on the constants used in this
      input.

.. organic:function:: repeat
   :required: value ~ anything
   :return: type of :code:`value`

   Restarts :code:`value` when it stops. By default, this causes :code:`value` to repeat infinitely.

   **Optional Inputs**

   .. organic:input:: repeats
      :type: number
      :default: 0

      Use this input to specify a finite number of repeats. If a number other than :code:`0` is specified, the function
      will stop after the final repeat.

.. organic:function:: random
   :required: from ~ number, to ~ number, length ~ number
   :return: number

   Generates a random number between :code:`from` and :code:`to`, holding it for :code:`length` milliseconds. Stops
   after :code:`length` milliseconds have elapsed.

   **Optional Inputs**

   .. organic:input:: type
      :type: :code:`stay` | :code:`linear`
      :default: stay

      Use this input to control the behavior of the returned value during the length of the function. See
      :ref:`random-type` for details on the constants used in this input.

.. organic:function:: limit
   :required: value ~ number, min ~ number, max ~ number
   :return: number

   Restricts :code:`value` to the inclusive range between :code:`min` and :code:`max`. A number is always returned, but
   if :code:`value` goes below :code:`min` or above :code:`max`, the corresponding bound is returned instead of the
   original :code:`value`. Stops when :code:`value` stops.

.. organic:function:: trigger
   :required: value ~ anything, condition ~ boolean
   :return: type of :code:`value`

   Waits until :code:`condition` becomes :code:`true`, then starts and returns :code:`value`. Stops after :code:`value`
   has started and completed.

.. organic:function:: if
   :required: condition ~ boolean, if-true ~ anything, if-false ~ type of :code:`if-true`
   :return: type of :code:`if-true`

   Returns :code:`if-true` if :code:`condition` evaluates to :code:`true`, otherwise returns :code:`if-false`.
   :code:`if-true` and :code:`if-false` can be of any type, as long as they are both of the same type. Stops after
   :code:`condition` stops.

.. organic:function:: all
   :required: values ~ list of boolean
   :return: boolean

   Returns :code:`true` if all items in :code:`values` evaluate to :code:`true`, otherwise returns :code:`false`. Stops
   when any item in :code:`values` stops.

.. organic:function:: any
   :required: values ~ list of boolean
   :return: boolean

   Returns :code:`true` if any item in :code:`values` evaluates to :code:`true`, otherwise returns :code:`false`. Stops
   when any item in :code:`values` stops.

.. organic:function:: none
   :required: values ~ list of boolean
   :return: boolean

   Returns :code:`true` if all items in :code:`values` evaluate to :code:`false`, otherwise returns :code:`false`. Stops
   when any item in :code:`values` stops.

.. organic:function:: min
   :required: values ~ list of number
   :return: number

   Returns the minimum value in :code:`values`. Stops when any item in :code:`values` stops.

.. organic:function:: min
   :required: values ~ list of number
   :return: number

   Returns the maximum value in :code:`values`. Stops when any item in :code:`values` stops.

.. organic:function:: round
   :required: value ~ number
   :return: number

   Rounds :code:`value` to the nearest whole number. Stops when :code:`value` stops.

   **Optional Inputs**

   .. organic:input:: step
      :type: number
      :default: 1

      Use this input to round to increments other than whole numbers.

   .. organic:input:: direction
      :type: :code:`nearest` | :code:`up` | :code:`down`
      :default: nearest

      Use this input to control the direction in which :code:`value` is rounded. See :ref:`round-direction` for details
      on the constants used in this input.

.. organic:function:: absolute
   :required: value ~ number
   :return: number

   Returns the `absolute value <https://en.wikipedia.org/wiki/Absolute_value_(algebra)>`__ of :code:`value`. Stops when
   :code:`value` stops.

-------------
Audio Sources
-------------

These functions are used to create various sources of audio, such as oscillating waveforms or pre-recorded sounds.

.. organic:function:: sine
   :required: frequency ~ number
   :return: audio source

   Generates a `sine wave <https://en.wikipedia.org/wiki/Sine_wave>`__ with the specified frequency.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated sine wave. Any number greater than or equal to
      :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated sine wave. Valid values are in the range from
      :code:`-1` to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to
      the right speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated sine wave. Effects will be applied in the order they are
      specified in this input.

.. organic:function:: square
   :required: frequency ~ number
   :return: audio source

   Generates a `square wave <https://en.wikipedia.org/wiki/Square_wave_(waveform)>`__ with the specified frequency.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated square wave. Any number greater than or equal
      to :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated square wave. Valid values are in the range from
      :code:`-1` to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to
      the right speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated square wave. Effects will be applied in the order they are
      specified in this input.

.. organic:function:: triangle
   :required: frequency ~ number
   :return: audio source

   Generates a `triangle wave <https://en.wikipedia.org/wiki/Triangle_wave>`__ with the specified frequency.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated triangle wave. Any number greater than or equal
      to :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated triangle wave. Valid values are in the range from
      :code:`-1` to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to
      the right speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated triangle wave. Effects will be applied in the order they
      are specified in this input.

.. organic:function:: saw
   :required: frequency ~ number
   :return: audio source

   Generates a `saw wave <https://en.wikipedia.org/wiki/Sawtooth_wave>`__ with the specified frequency.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated saw wave. Any number greater than or equal to
      :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated saw wave. Valid values are in the range from
      :code:`-1` to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to
      the right speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated saw wave. Effects will be applied in the order they are
      specified in this input.

.. organic:function:: oscillator
   :required: frequency ~ number, waveform ~ ???
   :return: audio source

   Generates a custom oscillator with the specified waveform and frequency.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated wave. Any number greater than or equal to
      :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated wave. Valid values are in the range from :code:`-1`
      to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to the right
      speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated wave. Effects will be applied in the order they are
      specified in this input.

.. organic:function:: noise
   :return: audio source

   Generates white noise.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated noise. Any number greater than or equal to
      :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated noise. Valid values are in the range from :code:`-1`
      to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to the right
      speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated noise. Effects will be applied in the order they are
      specified in this input.

.. organic:function:: sample
   :required: file ~ string
   :return: audio source

   Plays back the contents of the specified audio file. Currently, this function supports WAV and MP3 files.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the sample. Any number greater than or equal to :code:`0` is
      valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the sample. Valid values are in the range from :code:`-1` to
      :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to the right
      speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the sample. Effects will be applied in the order they are specified in
      this input.

.. organic:function:: granulate
   :required: sample ~ string
   :return: audio source

   Performs `granular synthesis <https://en.wikipedia.org/wiki/Granular_synthesis>`__, using the audio file specified in
   the :code:`sample` input as the source for audio samples.

   **Optional Inputs**

   .. organic:input:: length
      :type: number
      :default: 0

      Use this input to control the length of each grain, specified in milliseconds.

   .. organic:input:: grains
      :type: number
      :default: 1

      Use this input to control the number of grains playing at the same time.

   .. organic:input:: shape
      :type: ???
      :default: ???

      Use this input to control the amplitude of each grain over the course of its lifetime.

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the generated audio. Any number greater than or equal to
      :code:`0` is valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the generated audio. Valid values are in the range from :code:`-1`
      to :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to the right
      speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the generated audio. Effects will be applied in the order they are
      specified in this input.

.. organic:function:: group
   :required: sources ~ list of audio source
   :return: audio source

   Creates a composite audio source from the specified sources. Inputs to this function will be applied to the group as
   a whole, so any existing inputs on the specified sources will still be applied to those sources.

   **Optional Inputs**

   .. organic:input:: volume
      :type: number
      :default: 1

      Use this input to increase or decrease the volume of the group. Any number greater than or equal to :code:`0` is
      valid, but extremely high volume values can be dangerous or otherwise undesirable.

   .. organic:input:: pan
      :type: number
      :default: 0

      Use this input to control the stereo balance of the group. Valid values are in the range from :code:`-1` to
      :code:`1`, where :code:`-1` sends all output to the left speaker and :code:`1` sends all output to the right
      speaker. If :option:`--mono` is specified, this input has no effect.

   .. organic:input:: effects
      :type: list of effect
      :default: []

      Use this input to apply audio effects to the group. Effects will be applied in the order they are specified in
      this input.

-------
Effects
-------

These functions are used to manipulate the output of an audio source after it is created. They operate on the
:term:`audio samples <audio sample>` generated by an audio source, so any given effect can be applied to any audio
source regardless of the function that created it or the values of its inputs.

.. organic:function:: delay
   :required: delay ~ number, feedback ~ number
   :return: effect

   Applies a delay effect to the target audio source. The :code:`delay` input, specified in milliseconds, defines the
   length of time before an audio sample is repeated. The :code:`feedback` input, specified as a ratio between :code:`0`
   and :code:`1`, defines the volume of each delayed sample relative to its original sample. A :code:`feedback` of
   :code:`0` would result in no audible delay, and a :code:`feedback` of :code:`1` would result in an infinite delay.

   **Optional Inputs**

   .. organic:input:: mix
      :type: number
      :default: 1

      Use this input to control the overall volume of the delayed samples, regardless of the value of :code:`feedback`.

.. organic:function:: comb
   :required: delay ~ number, feedback ~ number
   :return: effect

   Applies a `comb filter <https://en.wikipedia.org/wiki/Comb_filter>`__ to the target audio source. This is similar to
   a delay effect, although comb filters are better suited for shorter delays that result in timbral effects, rather
   than longer delays that result in audibly repeated audio. The :code:`delay` input, specified in milliseconds, defines
   the length of time before an audio sample is repeated. The :code:`feedback` input, specified as a ratio between
   :code:`0` and :code:`1`, defines the volume of each delayed sample relative to its original sample. A
   :code:`feedback` of :code:`0` would result in no audible filtering, and a :code:`feedback` of :code:`1` would result
   in a harsher metallic effect.

   **Optional Inputs**

   .. organic:input:: mix
      :type: number
      :default: 1

      Use this input to control the overall volume of the delayed samples, regardless of the value of :code:`feedback`.

.. organic:function:: all-pass
   :required: delay ~ number, feedback ~ number
   :return: effect

   Applies an `all-pass filter <https://en.wikipedia.org/wiki/All-pass_filter>`__ to the target audio source.

   **Optional Inputs**

   .. organic:input:: mix
      :type: number
      :default: 1

      Use this input to control the ratio of filtered audio to original audio. A value of :code:`0` would result in only
      the original audio being played, and a value of :code:`1` would result in only the filtered audio being played.

.. organic:function:: low-pass
   :required: threshold ~ number
   :return: effect

   Applies a `low-pass filter <https://en.wikipedia.org/wiki/Low-pass_filter>`__ to the target audio source. The
   :code:`threshold` input defines the maximum frequency in Hz that is allowed to pass through the filter.

.. organic:function:: reverb
   :required: length ~ number
   :return: effect

   Applies a `reverb effect <https://en.wikipedia.org/wiki/Reverb_effect>`__ to the target audio source. The
   :code:`length` input defines the length of the echoes resulting from an initial sound.

   **Optional Inputs**

   .. organic:input:: mix
      :type: number
      :default: 1

      Use this input to control the ratio of echoes to original audio. A value of :code:`0` would result in only the
      original audio being played, and a value of :code:`1` would result in only the echoes being played.

.. organic:function:: effect-group
   :required: effects ~ list of effect
   :return: effect

   Creates a composite audio effect from the specified effects. Inputs to this function will be applied to the group as
   a whole, so any existing inputs on the specified effects will still be applied to those effects.

   **Optional Inputs**

   .. organic:input:: mix
      :type: number
      :default: 1

      Use this input to control the ratio of effects to original audio. A value of :code:`0` would result in only the
      original audio being played, and a value of :code:`1` would result in only the composite effect being played.

-----------------
Special Functions
-----------------

These functions do not fall into another category, either due to their usage or their effect on the output of a program.

.. organic:function:: include

   This function allows you to include the contents of another Organic program file in the current program. You can
   include more than one Organic program using multiple :code:`include` instructions, however all includes must be
   placed before any other instructions in the program. The :code:`include` function is not used like a normal function;
   it is written with the desired file path written as a string directly in the function's parentheses. For example:

   .. code-block::

      include("another-file.organic")
