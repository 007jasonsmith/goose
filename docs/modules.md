# Modules #

This document outlines the main modules in Goose.

## klib ##

K-Lib is the base library for simplifying C++ development. Since the standard
C and C++ libraries require things like files, I/O, and other kernel-level
services to already be available, klib provides common functionality for
everything but that.

It is in no way intended to be compatible with the C or C++ standard libraries.
Not invented here, bro.

- No requirements other than to other klib files.

## sys ##

sys is the piece of the kernel that is bound to the CPU. Interrupts, IO, etc.

- Depends on hal (specific IRQ handlers)

## hal ##

hal is the hardware abstraction layer. Device drivers go here.

- Depends on sys (IO)

## kernel ##

The kernel is where actual kernel-level features. Paging, processes, etc.

- Depends on sys

## shell ##

The shell is the command-line interface to Goose. It will provide all of the
diagnostic services and interactions to the user.

- Requires klib, sys, hal, kernel
