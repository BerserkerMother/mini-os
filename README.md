# Mini Os
Simple and useless mini OS for learning purposes. Will see where is goes.

## What does this kernel thingy?
a Set of services part of an operating system with responsibility to manage memory, IO, handling interrupts and more.
It exposes this services throught a set of system calls. Definition of these system calls are different from kernel to kernel.

## **C libarary(libc)**
The C standard libarary. Set of functionalities(e.g., string manupilation, IO, memory managment, etc.) added to C programming language. It provides headers and binaries to be linked with user-space program. It is linked to binaries by default.
It does not exist by default. Operating systems make it available. If you want to build your own operating system, you must port it or write it yourself.
In addition to C standard functions, libc might provide more functionalities defined in some other standard. For example libc doesn't say anything about networking but it is defined in POSIX.
> It should be noted that, in order to implement its functionality, the C library must call kernel functions. So, for your own OS, you can of course take a ready-made C library and just recompile it for your OS - but that requires that you tell the library how to call your kernel functions, and your kernel to actually provide those functions.
In another word, libc relies on kernel. It is important to distinguish the kernel doesn't rely on libc.


## Dir structure

- kernel/include
Part of system headers.
Gathers all interfaces that OS exposes to user-space and kernel module.(I think)

### Kernel

- kernel/Makefile
kernel Makefile ...

- kernel/include/kernel/tty.h
(TeleTYpewriter)terminal device for input and output?
I guess this is the interface to VGA device.

- kerne/kernel/kernel.c
Kernel entry

- kernel/arch/{arch}/i386/tty.c
tty header implemntation

## System Root
The root directory for the operating system. We will build OS step by step and gradually install them(copy them) to sysroot. sysroot here is just a temparory directory that will eventually become the actual root directory of the OS. 

### GCC and system root
It appears when we do `#include <someheader.h>` the someheader.h lives in somedir elsewhere which gcc knows about.GCC has it's own includes and the based on target-triplet and GCC version, there are other include path such as /usr/include. my stdio.h is located under /usr/include which 975 lines.

### Makefile
It is feasiable to have a rule to tar files for distribution.
> Note that this is not a recursive approach; there is no Makefile in those subdirectories. Dependencies are hard enough to get right if you use one Makefile. At the bottom of this article is a link to a very good paper on the subject titled "recursive make considered harmful"; not only is a single Makefile easier to maintain (once you learned a couple of tricks), it can also be more efficient!

### Paging

### C runtime(crt*)
Things that run before and after the program main entry points to achieve some tasks.
For example:
- setting up file descriptors
- allocating the stack

### IO
We have two different kind of IO. Memory mapped IO and Port mapped IO. We can access the first one by simply writing to the memory location(e.g. VGA text mode). To access the later, We need a port address (e.g. 0x3f8 serial port one), then we can use outb assembly instruction to write a char to it.

## Descriptor Table
Has different types global descriptor table, interrupt descriptor table and local descriptor table. It contains entries that hold infomration about single object(e.g. a service, a routine, a task, chunk of code, data) that CPU might need at some point.

### Global Descriptor Table(GDT)
A binary data structure that tells CPU about memory segments. 

## to read
- MMU
- Real mode
- Protected mode
- Long mode
- Common bugs in C programs
