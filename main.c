#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define CODE_BUF_SIZE 4096
#define TAPE_SIZE 4096

#define MAX_BRACKET_DEPTH 256

extern char code_add_start;
extern char code_add_end;
extern char code_sub_start;
extern char code_sub_end;
extern char code_ptr_left_start;
extern char code_ptr_left_end;
extern char code_ptr_right_start;
extern char code_ptr_right_end;
extern char code_output_start;
extern char code_output_end;
extern char code_input_start;
extern char code_input_end;
extern char code_lbracket_start;
extern char code_lbracket_end;
extern char code_rbracket_start;
extern char code_rbracket_end;

extern char code_prolog_start;
extern char code_prolog_end;
extern char code_epilog_start;
extern char code_epilog_end;

void emit_code_add(char* codebuf, size_t* pc);
void emit_code_sub(char* codebuf, size_t* pc);
void emit_code_ptr_left(char* codebuf, size_t* pc);
void emit_code_ptr_right(char* codebuf, size_t* pc);
void emit_code_output(char* codebuf, size_t* pc);
void emit_code_input(char* codebuf, size_t* pc);
void emit_code_lbracket(char* codebuf, size_t* pc, size_t* bracket_stack,
                        size_t* brackets);
bool emit_code_rbracket(char* codebuf, size_t* pc, size_t* bracket_stack,
                        size_t* brackets);

void emit_code_prolog(char* codebuf, size_t* pc);
void emit_code_epilog(char* codebuf, size_t* pc);

bool emit_code(char* codebuf, const char* source) {
  size_t pc = 0;  // program counter
  size_t bracket_stack[MAX_BRACKET_DEPTH];
  size_t brackets = 0;

  emit_code_prolog(codebuf, &pc);

  for (size_t i = 0; source[i] != '\0'; i++) {
    switch (source[i]) {
      case '+':
        emit_code_add(codebuf, &pc);
        break;
      case '-':
        emit_code_sub(codebuf, &pc);
        break;
      case '<':
        emit_code_ptr_left(codebuf, &pc);
        break;
      case '>':
        emit_code_ptr_right(codebuf, &pc);
        break;
      case '.':
        emit_code_output(codebuf, &pc);
        break;
      case ',':
        emit_code_input(codebuf, &pc);
        break;
      case '[':
        emit_code_lbracket(codebuf, &pc, bracket_stack, &brackets);
        break;
      case ']':
        bool result =
            emit_code_rbracket(codebuf, &pc, bracket_stack, &brackets);
        if (!result) return false;
        break;
      default:
        break;
    }
  }

  if (brackets != 0) return false;

  emit_code_epilog(codebuf, &pc);

  return true;
}

void emit_code_add(char* codebuf, size_t* pc) {
  size_t code_add_sz = &code_add_end - &code_add_start;
  memcpy(codebuf + *pc, &code_add_start, code_add_sz);
  *pc += code_add_sz;
}

void emit_code_sub(char* codebuf, size_t* pc) {
  size_t code_sub_sz = &code_sub_end - &code_sub_start;
  memcpy(codebuf + *pc, &code_sub_start, code_sub_sz);
  *pc += code_sub_sz;
}

void emit_code_ptr_left(char* codebuf, size_t* pc) {
  size_t code_ptr_left_sz = &code_ptr_left_end - &code_ptr_left_start;
  memcpy(codebuf + *pc, &code_ptr_left_start, code_ptr_left_sz);
  *pc += code_ptr_left_sz;
}

void emit_code_ptr_right(char* codebuf, size_t* pc) {
  size_t code_ptr_right_sz = &code_ptr_right_end - &code_ptr_right_start;
  memcpy(codebuf + *pc, &code_ptr_right_start, code_ptr_right_sz);
  *pc += code_ptr_right_sz;
}

void emit_code_output(char* codebuf, size_t* pc) {
  size_t code_output_sz = &code_output_end - &code_output_start;
  memcpy(codebuf + *pc, &code_output_start, code_output_sz);
  *pc += code_output_sz;
}

void emit_code_input(char* codebuf, size_t* pc) {
  size_t code_input_sz = &code_input_end - &code_input_start;
  memcpy(codebuf + *pc, &code_input_start, code_input_sz);
  *pc += code_input_sz;
}

void emit_code_lbracket(char* codebuf, size_t* pc, size_t* bracket_stack,
                        size_t* brackets) {
  size_t code_lbracket_sz = &code_lbracket_end - &code_lbracket_start;
  memcpy(codebuf + *pc, &code_lbracket_start, code_lbracket_sz);
  *pc += code_lbracket_sz;
  bracket_stack[(*brackets)++] = *pc;
}

bool emit_code_rbracket(char* codebuf, size_t* pc, size_t* bracket_stack,
                        size_t* brackets) {
  size_t code_rbracket_sz = &code_rbracket_end - &code_rbracket_start;
  memcpy(codebuf + *pc, &code_rbracket_start, code_rbracket_sz);
  *pc += code_rbracket_sz;

  if (*brackets <= 0) {
    return false;
  }

  size_t matching_bracket = bracket_stack[--*brackets];
  int32_t* displacement_r = (int32_t*)(codebuf + *pc - 4);
  *displacement_r = (int32_t)(matching_bracket - *pc);
  int32_t* displacement_l = (int32_t*)(codebuf + matching_bracket - 4);
  *displacement_l = (int32_t)(*pc - matching_bracket);
  return true;
}

void emit_code_prolog(char* codebuf, size_t* pc) {
  size_t code_prolog_sz = &code_prolog_end - &code_prolog_start;
  memcpy(codebuf + *pc, &code_prolog_start, code_prolog_sz);
  *pc += code_prolog_sz;
}

void emit_code_epilog(char* codebuf, size_t* pc) {
  size_t code_epilog_sz = &code_epilog_end - &code_epilog_start;
  memcpy(codebuf + *pc, &code_epilog_start, code_epilog_sz);
  *pc += code_epilog_sz;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    puts("Brainfuck JIT compiler");
    puts("Usage:");
    printf("%s <code>\n", argv[0]);
    return 1;
  }

  char* codebuf = mmap(NULL, CODE_BUF_SIZE, PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (codebuf == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  void* tape = calloc(1, TAPE_SIZE);
  if (tape == NULL) {
    perror("calloc");
    munmap(codebuf, CODE_BUF_SIZE);
    return 1;
  }

  if (!emit_code(codebuf, argv[1])) {
    fprintf(stderr, "Unbalanced brackets\n");
    munmap(codebuf, CODE_BUF_SIZE);
    free(tape);
    return 1;
  }

  // FILE* fp = fopen("dump.bin", "wb");
  // fwrite(codebuf, 1, CODE_BUF_SIZE, fp);
  // fclose(fp);

  mprotect(codebuf, CODE_BUF_SIZE, PROT_READ | PROT_EXEC);

  // When compiled with -Wpedantic, the following line causes a warning:
  // ISO C forbids conversion of object pointer to function pointer type
  // Suppress the warning by using __extension__
  __extension__ void (*code)(void*) = (void (*)(void*))codebuf;

  code(tape);

  free(tape);
  munmap(codebuf, CODE_BUF_SIZE);

  return 0;
}
