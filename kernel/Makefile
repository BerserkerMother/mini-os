# reads djfault host by running the script, set HOST to it of not set by something else.
# gets the arch from a script with the host as input. Basically these 3 lines do
# HOST -> HOSTARCH or ! -> HOSTARCH
DEFAULT_HOST!= ../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)

# CFLAGS
CFLAGS?= -O2 -g
# not setting them
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDE_DIR?=$(PREFIX)/include  

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc


# this allows to config based on the host arch. for different arch we simply change the host and
# provide the kernel and other files for that particular arch.
ARCHDIR=arch/$(HOSTARCH)

# get some kernel arch configs to add to configs from before
include $(ARCHDIR)/make.config

CFLAGS:= $(CFLAGS) $(KERNEL_ARCH_CFLAGS)
LDFLAGS:= $(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:= $(LIBS) $(KERNEL_ARCH_LIBS)

KERNEL_OBJS=\
					$(KERNEL_ARCH_OBJS) \
					kernel/kernel.o \
					kernel/format.o \
					kernel/gdt.o \
					kernel/idt.o \

# possible all O files
OBJS:=\
			$(ARCHDIR)/crti.o \
			$(ARCHDIR)/crtbegin.o \
			$(KERNEL_OBJS) \
			$(ARCHDIR)/crtend.o \
			$(ARCHDIR)/crtn.o

# things that should be passed to linker probably
# yup. all object files neccessary for OS to be linked into os.kernel file
LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all os.kernel clean install install-headers install-kernel
# Tell make that filter everything based on .o .c .s. 
# If a rule matches a file with a different suffix, it gets ignore.
.SUFFIXES: .o .c .s

all: os.kernel

# The goal is to link objects files together with linked.ld script.
# for now, it backs crtS with kernel, boot, and tty objects.
# adds -lgcc -lk for gcc lib and libk.a we built on libc Makefile.
os.kernel: $(OBJS) $(ARCHDIR)/linked.ld
	@echo "[LD] linking os objects..."
	$(CC) -T $(ARCHDIR)/linked.ld -o $@ $(CFLAGS) $(LDFLAGS) $(LINK_LIST)  
	@echo "[GRUB] checking x86 multiboot compliant"
	grub-file --is-x86-multiboot os.kernel

# c runtime I guess. It intialized things and then give control to kernel entry.
# it gets the crt files from compiler and put them in ARCHDIR for building OS.
# [chatGPT] crti.o → crtbegin.o → [your object files & libraries] → crtend.o → crtn.o
$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJS=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJS" $@

# compiles C files to object
%.o: %.c
	@echo "[CC] $@"
	echo "$(CFLAGS)"
	$(CC) -MD -c $< -o $@ -std=gnu99 $(CFLAGS) 

# compiles assembly files into object
%.o: %.S
	@echo "[CC] $@"
	$(CC) -MD -c $< -o $@ $(CFLAGS)

clean:
	rm -f os.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

# just copies include headers to dest include dir
install-headers: 
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -r --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: os.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp os.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
