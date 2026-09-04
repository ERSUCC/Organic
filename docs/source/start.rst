Getting Started
===============

This section of the documentation will walk you through a first-time setup of Organic. If you have prior experience
with another programming language, some of the information here may be familiar to you. But if not, don't worry; each
step will be explained in detail so that anyone can follow regardless of skill level. Terms or concepts that may be
unfamiliar or confusing are explained on the :doc:`terms` page, and the corresponding entries are linked here.

.. _installation:

############
Installation
############

The installation process is the same regardless of which operating system you are using, although you will need to
download the correct installer. You can download the latest version of Organic for your operating system
`here <http://github.com/ERSUCC/Organic/releases>`__. Once you have downloaded the installer, run it and follow the
instructions on the screen to complete the installation.

To make sure that Organic is installed correctly, run the following command in a :doc:`command line <cmd-line>`:

.. code-block::

   organic --version

If everything was installed correctly, you should see "Organic v\ |release|" printed below your command. If nothing
happens or if you see an error message instead, first ensure that you installed the correct version of Organic for your
operating system. If you are using a Mac, also make sure that you installed the version that is made for your processor
architecture. If you don't know what that means or you aren't sure which one you have, you can read the short article
`here <https://macreports.com/how-to-know-if-your-mac-has-apple-silicon-or-intel-chip>`__ for more information.

If this is your first time programming, continue on in this section for instructions on setting up a text editor, which
you will need to use Organic. If you already have a text editor, you can skip the rest of this section and move on to
:ref:`first-program`.

Now that you've installed Organic, you'll also need to install a text editor that is suitable for writing code. Text
editors come in many shapes and sizes, but a good place to start for most users is Visual Studio Code, which you can
download `here <https://code.visualstudio.com/download>`__. Unlike standard text document editors, editors like Visual
Studio Code display your code in a more readable font for coding, and they can highlight your code in different colors
to make it easier to tell what's what.

If you chose to install Visual Studio Code, you also have the option to install the Organic extension, which provides
code highlighting information and file icons for Organic code files. To do so, open Visual Studio Code, find the
Extensions tab on the left, and search for "Organic". Install the extension whose icon matches the Organic logo seen on
this website.

.. _first-program:

##################################
Writing your first Organic program
##################################

If you've made it here, congratulations! You're now ready to write your first Organic program. As a first example,
we'll make a simple sine wave at 440 :term:`Hz <hertz>`. In Organic, this looks like the following:

.. code-block::

   sine(frequency: 440)

In your text editor, make a new file called "hello.organic" and type the code above into the file. Make sure to save
the file in a location you can remember, and make sure your :doc:`command line <cmd-line>` is pointing to that location.
When you're ready, run the following command:

.. code-block::

   organic hello.organic

This tells Organic to read the code in the file you just created and play the resulting audio. When you've heard
enough, you can stop the command. Now that you're able to create and run Organic programs, the next step is to learn the
syntax of the Organic language, so that you can interpret existing programs and write your own. When you're ready,
continue on to :doc:`syntax`.
