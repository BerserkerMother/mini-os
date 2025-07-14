#ifndef IO_H
#define IO_H

// TODO: why can't I inline here
void outb(unsigned short port, unsigned char c);
unsigned char inb(unsigned short port);
void io_wait();

#endif
