.section .text
# macro to generate interrupt stubs
.macro ISR_STUB int_no, has_err
.global isr\int_no\()_handler
.type isr\int_no\()_handler, @function
isr\int_no\()_handler:
  # in case of no error num
  .if \has_err == 0
    pushl $0
  .endif
  # for debug
  pushl \int_no
  # all general purpose registers
  pushal 
  push %esp
  call _isr\int_no\()_handler
  addl $4, %esp
  popal
  # pop interrupt num and err code
  addl $8, %esp

  iret
.endm

ISR_STUB 8, 1
ISR_STUB 13, 1
ISR_STUB 14, 1
