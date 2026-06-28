.data

# %r12 = tape pointer
# according to SysV ABI, it is callee-saved

.global code_prolog_start
.global code_prolog_end
.global code_prolog_end_safe
code_prolog_start:
  push %r12 # saves %r12
  movq %rdi, %r12 # moves the address of the tape into %r12
code_prolog_end:
  # In safe mode
  # %r13 = pointer to the start of the tape
  # %r14 = pointer to the end of the tape
  push %r13 # saves %r13
  push %r14
  movq %rdi, %r13 # moves the address of the tape into %r13
  # this dummy number will be replaced by TAPE_SIZE
  leaq 0x12345678(%r13), %r14
code_prolog_end_safe:

.global code_add_start
.global code_add_end
code_add_start:
  incb (%r12)
code_add_end:

.global code_add_n_start
.global code_add_n_end
code_add_n_start:
  addb $0, (%r12)
code_add_n_end:

.global code_sub_start
.global code_sub_end
code_sub_start:
  decb (%r12)
code_sub_end:

.global code_sub_n_start
.global code_sub_n_end
code_sub_n_start:
  subb $0, (%r12)
code_sub_n_end:

.global code_ptr_left_start_safe
.global code_ptr_left_start
.global code_ptr_left_end
code_ptr_left_start_safe:
  # Bound check
  cmpq %r13, %r12
  jle code_ptr_left_end
code_ptr_left_start:
  decq %r12
code_ptr_left_end:

.global code_ptr_left_n_start_safe
.global code_ptr_left_n_safe_imm
.global code_ptr_left_n_start
.global code_ptr_left_n_end
code_ptr_left_n_start_safe:
  # Using %r11, a temporary register to hold the address - no need to save
  leaq 0x12345678(%r12), %r11
code_ptr_left_n_safe_imm:
  cmpq %r13, %r11
  jl code_ptr_left_n_end
code_ptr_left_n_start:
  # This assembles to:
  # REX.W + 81 /5 id
  # SUB r/m64, imm32
  # forcing a 32-bit immediate
  subq $0x12345678, %r12
code_ptr_left_n_end:

.global code_ptr_right_start_safe
.global code_ptr_right_start
.global code_ptr_right_end
code_ptr_right_start_safe:
  cmpq %r14, %r12
  jge code_ptr_right_end
code_ptr_right_start:
  incq %r12
code_ptr_right_end:

.global code_ptr_right_n_start_safe
.global code_ptr_right_n_safe_imm
.global code_ptr_right_n_start
.global code_ptr_right_n_end
code_ptr_right_n_start_safe:
  leaq 0x12345678(%r12), %r11
code_ptr_right_n_safe_imm:
  cmpq %r14, %r11
  jg code_ptr_right_n_end
code_ptr_right_n_start:
  addq $0x12345678, %r12
code_ptr_right_n_end:

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
  je 0x12345678
code_lbracket_end:

.global code_rbracket_start
.global code_rbracket_end
code_rbracket_start:
  cmpb $0, (%r12)
  jne 0x12345678
code_rbracket_end:

.global code_epilog_start_safe
.global code_epilog_start
.global code_epilog_end
code_epilog_start_safe:
  # In safe mode, restore %r13 and %r14
  pop %r14
  pop %r13
code_epilog_start:
  pop %r12 # restores %r12
  ret
code_epilog_end:

.section .note.GNU-stack,"",@progbits
