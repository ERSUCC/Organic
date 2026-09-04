Terminology
===========

This section of the documentation provides definitions for the common terminology and concepts referenced in other
sections of the documentation. The entries are organized in alphabetic order for ease of use.

.. glossary::

   Audio Sample
      An audio sample is the shortest unit of audio data in the recording and playback of digital audio. Each audio
      sample is a number representing the amplitude of a sound wave at a particular point in time. When a series of
      audio samples are sent to a speaker in quick succession, an audible sound is generated. Standard audio playback
      rates are in the range of tens of thousands to hundreds of thousands of samples per second, which allows for a
      resolution that is effectively equivalent to physical audio.

   Audio Synthesis
      Audio synthesis is a method of creating sound by computing the oscillations of a sound wave and playing them back
      with a speaker, rather than by manually vibrating something like a string. Although it can be difficult to
      replicate existing instruments using audio synthesis, it allows for the creation of a wide range of sounds that
      would not be possible to create using a physical instrument.

   Case-sensitive
      Like most programming languages, Organic is case-sensitive, which means that it matters whether you use capital or
      lowercase letters when writing a program. For example, if something is named :code:`test` and you try to use it by
      typing :code:`TEST`, Organic will tell you that the name does not exist. Although it is not a strict requirement,
      the standard for Organic programs is to use :term:`kebab case` for all names.

   Hertz
      Hertz, often abbreviated as Hz, are a unit of measurement of the frequency or pitch of a sound. Hertz are
      equivalent to "times per second", when referring to the oscillations of a sound wave. In Organic, all numeric
      frequency values are specified in Hertz, but you can also use note names such as :code:`c3` or :code:`gf2` to
      refer to notes in the Western chromatic scale. For example, the numeric frequency :code:`440` is equivalent to the
      note name :code:`a4`, and they can be used interchangeably when specifying the frequency of an audio source.

   Kebab Case
      When writing code, there are a variety of ways to use case and separate words within names. The standard in
      Organic programs is to use kebab case, which uses all lowercase letters and separates words with hyphens. Names
      that use kebab case can have just one word, such as :code:`test`, or they can have multiple words, such as
      :code:`things-and-stuff`.

   Timbre
      Timbre is the quality, or color, of a sound. Timbre is defined by the relative strength of specific high and low
      frequencies present in a sound, and it is how our brains are able to identify what type of sound we are hearing in
      addition to its fundamental pitch.
