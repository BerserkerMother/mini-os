#!/bin/sh
set -e

source ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -kernel isodir/boot/os.kernel -d int,mmu -D qemu.log -device isa-debug-exit,iobase=0xf4,iosize=0x04 -serial stdio
