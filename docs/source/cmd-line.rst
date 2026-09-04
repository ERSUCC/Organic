Command Line Guide
==================

This section of the documentation explains how to use a command line, which is the primary method of interacting with
Organic and running Organic programs.

#######################
What is a Command Line?
#######################

A command line, also known as a command prompt or a terminal, is an application that allows you to use programs like
Organic that don't provide a visual user interface. These programs are commonly known as command line programs, or more
simply, commands. Command line programs have all the same capabilities as visual applications, but instead of
interacting with them by clicking buttons, you type what you want with text. It can seem intimidating at first, but you
only need to know a few simple commands to use Organic!

##########
File Paths
##########

Before using a command line, it is beneficial to understand the concept of file paths. A file path is a way of
representing the location of a specific file or folder on your computer, using a sequence of directions that tell the
computer how to find it. Typically, these directions are names of folders, specified in top-down order. When using
Organic, understanding how to use file paths will allow you to tell Organic where to find the program you want to run.

When writing a file path, the sequence of folders leading up to the target file or folder are usually separated by
forward slashes (:code:`/`), or sometimes backslashes (:code:`\\`) on Windows. For example, if you have a folder named
:code:`Files` that contains a file named :code:`hello.txt`, you could write the path to that file as
:code:`Files/hello.txt`. However, this file path is only valid if you start from a folder that contains the folder named
:code:`Files`. For example, if the folder named :code:`Files` is on your Desktop but you start from your Downloads
folder, you won't be able to find :code:`hello.txt`. This type of file path is known as a relative file path.

If you want to create a file path that you can use regardless of where you start, you can instead create an absolute
file path. Instead of starting in the current folder, absolute file paths start at the topmost folder on your computer,
such as :code:`C:` on Windows, or simply :code:`/` on Mac and Linux. For example, to represent the file
:code:`hello.txt` from the example above as an absolute file path, you would need to write it as
:code:`C:/Users/johndoe/Desktop/Files/hello.txt` on Windows, or as :code:`/Users/johndoe/Desktop/Files/hello.txt` on
Mac.

################
Running Commands
################

The first thing you need to do is find the command line application on your computer. Every computer comes with a
command line application installed, but the name of the application is different depending on which operating system you
are using. If you are using Mac or Linux, it is called Terminal. If you are using Windows, there are a variety of
options, but to get the best compatibility with this guide, you should use PowerShell.

The most important thing to know about the command line is that when you run a command, it runs in the context of a
specific folder on your computer. You should always know which folder your command line is currently in before running a
command, in case you want to use a relative file path.

To run a command, type its name into the command line and press the Enter or Return key. A useful command to test this
with is :code:`pwd`, which tells you which folder the command line is currently in. If you run the :code:`pwd` command,
you should see your home folder, which is the default command line location. For example, on Windows, you might see
something like :code:`C:\\Users\\johndoe`.

Most commands also allow you to provide one or more inputs separated by spaces after the name of the program, which can
be used to control the behavior of the program. A useful command to test this with is :code:`cd`, which tells the
command line to move to one of the folders inside the current folder. The :code:`cd` command has one input, which is the
folder that you would like to move to. For example, try running the command :code:`cd Downloads`, which will tell the
command line to move to your Downloads folder. In this example, :code:`Downloads` is a relative file path, which works
as expected because your Downloads folder is inside your home folder. You can verify that it worked by running
:code:`pwd` again, which should now display something like :code:`C:\\Users\\johndoe\\Downloads`. You can also use
:code:`cd` to move back into the folder that contains the current folder, by running the command :code:`cd ..`. If you
run that command now, it will move the command line back into your home folder. Again, you can use the :code:`pwd`
command to verify that it worked.

Sometimes, you might forget which files and folders are inside the current folder. In this situation, you can run the
:code:`ls` command, which will list everything in the current folder. If you run it now, you should see a variety of
familiar folders, such as Downloads, Desktop, and Documents, all of which you could use to begin a relative file path
from your home folder.

#################
Stopping Commands
#################

Most commands will run to completion quickly, but sometimes a command might get stuck or take longer than you were
expecting. In those cases, you can press Ctrl+C to abort the command. Note that this does not undo any actions performed
by the command before it was aborted, so it can result in incomplete output or missing files. When using Organic, you
will often use Ctrl+C to stop the command, because the default behavior is for Organic to run your program forever. Once
you get more familiar with Organic, you can read the :doc:`usage` page for more information on how to control the
running time of the program.

:doc:`Return to Getting Started <start>`
