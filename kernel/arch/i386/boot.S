# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 8
.global gdt_start
gdt_start:
    .skip 40 # 5 entries
.global gdt_end
gdt_end:

.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data
.align 4096
.global page_directory
page_directory:
  .rept 1024
    .long 0x2
  .endr

.align 4096
.global page_table_0
page_table_0:
  .rept 1024
    .long 0x2
  .endr

gdtr:
    .word 0
    .long 0 

# The kernel entry point.
.section .text

.global set_gdt
.type set_gdt, @function
set_gdt:
  movw 4(%esp), %ax
  movw %ax, gdtr
  movl 8(%esp), %eax
  movl %eax, gdtr+2
  lgdt gdtr
  ret
  .size set_gdt, . - set_gdt

.global reload_segments
.type reload_segments, @function
reload_segments:
  ljmp   $0x08, $1f

1:
  movw   $0x10, %ax
  movw   %ax, %ds
  movw   %ax, %es
  movw   %ax, %fs
  movw   %ax, %gs
  movw   %ax, %ss

  ret
  .size reload_segments, .-reload_segments


.type setup_paging, @function
setup_paging:
  movl page_directory, %eax
  orl $3, %eax
  orl $page_table_0, %eax
  movl %eax, page_directory

  # movl $page_table_0, %eax
  movl $0, %ebx
  movl $page_table_0, %edi
  mov $1024, %ecx

fill_pt_table:
  movl %ebx, %eax
  orl $0x3, %eax
  movl %eax, (%edi)
  addl $0x1000, %ebx
  addl $4, %edi
  loop fill_pt_table

  ret

.type _paging, @function
_paging:
  # mov page_directory, %eax
  movl $page_directory, %eax
  movl %eax, %cr3

  movl %cr0, %eax
  orl  $0x80000001, %eax
  movl %eax, %cr0
  ret

.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Call the global constructors.
	call _init

  call setup_paging

  call _paging

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
  cli
1:	hlt
	jmp 1b
.size _start, . - _start
