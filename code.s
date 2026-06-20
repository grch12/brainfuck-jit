.data

# %r12 = tape pointer
# according to SysV ABI, it is callee-saved

.global code_prolog_start
.global code_prolog_end
code_prolog_start:
  push %r12 # saves %r12
  movq %rdi, %r12 # moves the address of the tape into %r12
code_prolog_end:

.global code_add_start
.global code_add_end
code_add_start:
  incb (%r12)
code_add_end:

.global code_sub_start
.global code_sub_end
code_sub_start:
  decb (%r12)
code_sub_end:

.global code_ptr_left_start
.global code_ptr_left_end
code_ptr_left_start:
  decq %r12
code_ptr_left_end:

.global code_ptr_right_start
.global code_ptr_right_end
code_ptr_right_start:
  incq %r12
code_ptr_right_end:

.global code_output_start
.global code_output_end
code_output_start:
  movq $1, %rax # sys_write
  movq $1, %rdi # stdout
  movq %r12, %rsi # tape pointer
  movq $1, %rdx # count
  syscall
code_output_end:

.global code_input_start
.global code_input_end
code_input_start:
  movq $0, %rax # sys_read
  movq $0, %rdi # stdin
  movq %r12, %rsi # tape pointer
  movq $1, %rdx # count
  syscall
code_input_end:

.global code_lbracket_start
.global code_lbracket_end
code_lbracket_start:
  cmpb $0, (%r12)
  je 0x00000000
code_lbracket_end:

.global code_rbracket_start
.global code_rbracket_end
code_rbracket_start:
  cmpb $0, (%r12)
  jne 0x00000000
code_rbracket_end:

.global code_epilog_start
.global code_epilog_end
code_epilog_start:
  pop %r12 # restores %r12
  ret
code_epilog_end:

.section .note.GNU-stack,"",@progbits
