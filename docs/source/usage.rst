Program Usage
=============

This section of the documentation details the options that Organic provides when running the command in a
:doc:`command line <cmd-line>`. These options allow for finer control over the internal behavior of Organic, and are not
likely to be useful in standard use cases. For instructions on the basic usage of Organic, see :ref:`first-program`.

#############
General Usage
#############

.. code-block::

   organic [--version] [--info] [--time <num>] [--fast-forward <num>]
           [--export <path>] [--channels <num>] [--sample-rate <num>]
           [--buffer-length <num>] [--seed <num>] <path>

##############
Option Details
##############

.. option:: --version

   Display the current version and exit.

.. option:: --info

   Display configuration info before running the program. This includes the values of each internal option listed here,
   as well as the version number.

.. option:: --time <time>

   Set the runtime of the program in milliseconds. If unspecified, the program will run infinitely. Note that this does
   not change the behavior of any audio sources specified in the program itself; sources will not repeat if they stop
   before the specified runtime, and sources that are still running when the specified runtime is reached will be cut
   short.

.. option:: --fast-forward <time>

   Skip to the provided time in milliseconds before starting audio output. Note that this will silently simulate each
   frame of the program up to the specified time, rather than skipping directly to that time. This is required to
   enforce continuity of the program, but can result in a moment of silence before the audio begins in programs with
   higher computational requirements.

.. option:: --export <path>

   Render the program to the specified audio file instead of playing back in time. Must be used in conjunction with
   :code:`--time`. The only format currently supported for export is WAV, and specifying a different file extension may
   result in undefined behavior.

.. option:: --mono

   Use mono audio for the program. If not specified, the program will run in stereo.

.. option:: --seed <number>

   Use the provided seed for random number generation.

.. option:: --buffer-length <number>

   Use the provided buffer length for audio output. If not specified, the buffer length will be 128 samples. Specifying
   a shorter buffer length can improve latency, usually at the cost of performance.

.. option:: --sample-rate <number>

   Use the provided sample rate. If not specified, the sample rate will be 44100 :term:`Hz <hertz>`. Specifying a lower
   sample rate can improve performance significantly, but it will also decrease the quality of the resulting audio
   output.
