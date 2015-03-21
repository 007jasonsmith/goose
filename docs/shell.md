# Goose Shell #

The Goose shell is the user interface into the operating system. However it is
important to know the scope of the shell.

The Goose shell will NOT be a complete, fully featured shell you are used to.
It will never have a scripting language, sophsticated command-line syntax, or
even allow you to interactively run programs.

Instead, the Goose shell is simply a way for a kernel developer to inspect the
state of the system while it is running. Nothing more.

The longer-term plan for the Goose user interface is to have the kernel host
a web server, and allow users to interact through HTTP.

## Overview ##

The Goose shell works by intercepting keystrokes and processing them. The
majority of keystokes to go the current window, typing out the next command.
However, some keys switch between different windows. This is how it is possible
to switch between the outputs of two different programs. And possibly monitor
their memory usage, in a third window.

The shell is some chrome over a series of "windows". Each window is a text
buffer.
