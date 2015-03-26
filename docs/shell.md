# Goose Shell #

The Goose shell is the user interface into the operating system. However it is
important to know the scope of the shell.

The Goose shell will NOT be a complete, fully featured shell you are used to.
It will never have a scripting language, sophsticated command-line syntax, or
even allow you to interactively run programs.

Instead, the Goose shell is simply a way for a kernel developer to inspect the
state of the system while it is running. Nothing more.

The longer-term plan for the Goose user interface is to have the kernel host
a web server, and allow users to interact through HTTP. A rich shell is a
stop-gap solution until this can become a reality.

## Overview ##

The Goose shell works similar to Emacs, in that it is a collection of "buffers".

The Shell provides a mechanism to switch between these buffers, to quickly
switch between different diagnostic outputs, visualizing system state, etc.

Buffers are managed by a window manager. The window manager is responcible
for actually displaying the contents of a buffer on the screen, and managing
any virtualized scrolling.

Finally, the Shell executor actually operates the shell. It's primary role
is to handle keyboard input, and send it to the Window manager or active
buffer as necessary.

Note that buffers do not accept keyboard input directly. Only the shell
executor deals with that. It may appear that a buffer is directly responding
to keyboard commands, but in actuality an adapter between the executor
and the buffer is what is processing that.

## Window Manager ##

The window manager controls which buffers are actively being displayed. The
window manager determines how the screen real-estate gets doled out between
buffers.

The window manager may choose to display multiple buffers at a time, e.g. a
two, three, or four-way display.

It is assumed that the window manager has complete control over the display.
It is also responcibile for printing data to the screen.

### Active window ###

The window manager designates one buffer as being the "active window"
(regardless of whether or not it shares the screen with other buffers).

The active window will be the recipient of keyboard input from the executor.

## Buffers ##

A buffer is a fixed size of characters (to avoid allocations since paging
may not be enabled yet). As a buffer is written to the character stream is
added to accordingly.

### Linebreaks ###

If all buffers were displayed at the same width, then the underlying `char`
buffer could simply be the width of the screen. However, in order to
allow display at different widths, special case is needed for line breaks.

Whenever a newline is written into a buffer, its location, and the length of
the previous line is stored in a special array. When the buffer gets draw
this array is used to determine how to actually render each line. Since
a single line in the buffer may take up two or more lines on the screen, due
to scrolling.

These linebreak markers are also used to enable scrolling.

### Adapter ###

Each buffer is paired with an adapter. An adapter determines what data, if
any, gets written to the buffer.

For example, a buffer which contains logging output from the kernel may be
registered with an adapter that accepts log messages from other parts of the
system.

A more sophisticated adapter would completely overwrite the contents of the
buffer periodically. For example, a display that showed the contents of a
region of memory. The adapter may refresh the buffer's contents via a timer.

The most sophsticated example of an adapter is the core shell adapter, which
will process lines of keyboard, execute the command, and render its output
to the buffer.

## Executor #

The executor handles input from the system and manages the window manager and
available buffers.

For the most part, the executor responds to keyboard events and fowards them
onto the window manager. For example, asking the window manager to scroll
the active window with arrow or page up/down keypresses.

Similarly, the executor may pass printable keyboard letters to the buffer
as the user types data. (Specifically, passing it to the buffer's adapter.)

# Buffers #

The shell isn't designed for arbitrary numbers of buffers as a general-purpose
experience. Since the shell will exist before paging is implemented, it needs
to be statically allocated. So the number and type of buffers is fixed.

Although the adapters used to power the buffers may be swapped out at runtime.

## Core shell ##

The core shell's adapter is the most sophisticated of them all. It mimics a
common shell, providing a command-line prompt. The user may type text, which
in turn advances the cursor. The user may move the cursor, delete text from
the middle of the current command, or backspace through it.

When the user presses enter, the adapter will execute a pre-defiend command
if it is known. Otherwise an error message will be displayed.

## Debug log ##

The debug log buffer simply receives data written to klib::Debug and renders
it to the buffer.

## Memory ##

The memory buffer is similar to the Core shell in that it provides an
interactive prompt. But rather than executing commands, it allows users to
show the coments of memory.

The buffer supports different modes (settable by specific keys) for displaying
data in hex, decimal, etc.
