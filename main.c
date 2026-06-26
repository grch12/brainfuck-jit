#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// If you got segfaults, it's probably because the buffer is too small
// Try increasing these values
#define CODE_BUF_SIZE 1048576  // 1MB should be enough for most programs
#define TAPE_SIZE 1048576

#define MAX_BRACKET_DEPTH 256

// Declarations of symbols from code.s
// Type is irrelevant, it's just to make the linker happy
extern char code_add_start;
extern char code_add_end;
extern char code_add_n_start;
extern char code_add_n_end;
extern char code_sub_start;
extern char code_sub_end;
extern char code_sub_n_start;
extern char code_sub_n_end;
extern char code_ptr_left_start_safe;
extern char code_ptr_left_start;
extern char code_ptr_left_end;
extern char code_ptr_left_n_start_safe;
extern char code_ptr_left_n_safe_imm;
extern char code_ptr_left_n_start;
extern char code_ptr_left_n_end;
extern char code_ptr_right_start_safe;
extern char code_ptr_right_start;
extern char code_ptr_right_end;
extern char code_ptr_right_n_start_safe;
extern char code_ptr_right_n_safe_imm;
extern char code_ptr_right_n_start;
extern char code_ptr_right_n_end;
extern char code_output_start;
extern char code_output_end;
extern char code_input_start;
extern char code_input_end;
extern char code_lbracket_start;
extern char code_lbracket_end;
extern char code_rbracket_start;
extern char code_rbracket_end;

extern char code_prolog_start_safe;
extern char code_prolog_start;
extern char code_prolog_end;
extern char code_epilog_start_safe;
extern char code_epilog_start;
extern char code_epilog_end;

void emit_code_add(char* codebuf, size_t* pc);
void emit_code_add_n(char* codebuf, size_t* pc, uint8_t n);
void emit_code_sub(char* codebuf, size_t* pc);
void emit_code_sub_n(char* codebuf, size_t* pc, uint8_t n);
void emit_code_ptr_left(char* codebuf, size_t* pc, bool safe_mode);
void emit_code_ptr_left_n(char* codebuf, size_t* pc, uint32_t n,
                          bool safe_mode);
void emit_code_ptr_right(char* codebuf, size_t* pc, bool safe_mode);
void emit_code_ptr_right_n(char* codebuf, size_t* pc, uint32_t n,
                           bool safe_mode);
void emit_code_output(char* codebuf, size_t* pc);
void emit_code_input(char* codebuf, size_t* pc);
void emit_code_lbracket(char* codebuf, size_t* pc, size_t* bracket_stack,
                        size_t* brackets);
bool emit_code_rbracket(char* codebuf, size_t* pc, size_t* bracket_stack,
                        size_t* brackets);

void emit_code_prolog(char* codebuf, size_t* pc, bool safe_mode);
void emit_code_epilog(char* codebuf, size_t* pc, bool safe_mode);

bool emit_code(char* codebuf, const char* source, bool safe_mode) {
  size_t pc = 0;  // program counter
  size_t bracket_stack[MAX_BRACKET_DEPTH];
  size_t brackets = 0;

  emit_code_prolog(codebuf, &pc, safe_mode);

  for (size_t i = 0; source[i] != '\0'; i++) {
    switch (source[i]) {
      case '+': {
        uint8_t n = 1;
        // merge consecutive '+'s into a single add instruction
        while (source[i + 1] == '+') {
          n++;
          i++;
        }
        if (n > 1)
          emit_code_add_n(codebuf, &pc, n);
        else
          emit_code_add(codebuf, &pc);
        break;
      }
      case '-': {
        uint8_t n = 1;
        while (source[i + 1] == '-') {
          n++;
          i++;
        }
        if (n > 1)
          emit_code_sub_n(codebuf, &pc, n);
        else
          emit_code_sub(codebuf, &pc);
        break;
      }
      case '<': {
        uint32_t n = 1;
        while (source[i + 1] == '<') {
          n++;
          i++;
        }
        if (n > 1)
          emit_code_ptr_left_n(codebuf, &pc, n, safe_mode);
        else
          emit_code_ptr_left(codebuf, &pc, safe_mode);
        break;
      }
      case '>': {
        uint32_t n = 1;
        while (source[i + 1] == '>') {
          n++;
          i++;
        }
        if (n > 1)
          emit_code_ptr_right_n(codebuf, &pc, n, safe_mode);
        else
          emit_code_ptr_right(codebuf, &pc, safe_mode);
        break;
      }
      case '.': {
        emit_code_output(codebuf, &pc);
        break;
      }
      case ',': {
        emit_code_input(codebuf, &pc);
        break;
      }
      case '[': {
        emit_code_lbracket(codebuf, &pc, bracket_stack, &brackets);
        break;
      }
      case ']': {
        bool result =
            emit_code_rbracket(codebuf, &pc, bracket_stack, &brackets);
        if (!result) return false;
        break;
      }
      default:
        break;
    }
  }

  if (brackets != 0) return false;

  emit_code_epilog(codebuf, &pc, safe_mode);

  return true;
}

void emit_code_add(char* codebuf, size_t* pc) {
  size_t code_add_sz = &code_add_end - &code_add_start;
  memcpy(codebuf + *pc, &code_add_start, code_add_sz);
  *pc += code_add_sz;
}

void emit_code_add_n(char* codebuf, size_t* pc, uint8_t n) {
  size_t code_add_n_sz = &code_add_n_end - &code_add_n_start;
  memcpy(codebuf + *pc, &code_add_n_start, code_add_n_sz);
  *pc += code_add_n_sz;
  int8_t* imm8 = (int8_t*)(codebuf + *pc - 1);
  *imm8 = n;
}

void emit_code_sub(char* codebuf, size_t* pc) {
  size_t code_sub_sz = &code_sub_end - &code_sub_start;
  memcpy(codebuf + *pc, &code_sub_start, code_sub_sz);
  *pc += code_sub_sz;
}

void emit_code_sub_n(char* codebuf, size_t* pc, uint8_t n) {
  size_t code_sub_n_sz = &code_sub_n_end - &code_sub_n_start;
  memcpy(codebuf + *pc, &code_sub_n_start, code_sub_n_sz);
  *pc += code_sub_n_sz;
  int8_t* imm8 = (int8_t*)(codebuf + *pc - 1);
  *imm8 = n;
}

void emit_code_ptr_left(char* codebuf, size_t* pc, bool safe_mode) {
  size_t code_ptr_left_sz;
  if (safe_mode) {
    code_ptr_left_sz = &code_ptr_left_end - &code_ptr_left_start_safe;
    memcpy(codebuf + *pc, &code_ptr_left_start_safe, code_ptr_left_sz);
  } else {
    code_ptr_left_sz = &code_ptr_left_end - &code_ptr_left_start;
    memcpy(codebuf + *pc, &code_ptr_left_start, code_ptr_left_sz);
  }
  *pc += code_ptr_left_sz;
}

void emit_code_ptr_left_n(char* codebuf, size_t* pc, uint32_t n,
                          bool safe_mode) {
  size_t code_ptr_left_n_sz;
  if (safe_mode) {
    code_ptr_left_n_sz = &code_ptr_left_n_end - &code_ptr_left_n_start_safe;
    memcpy(codebuf + *pc, &code_ptr_left_n_start_safe, code_ptr_left_n_sz);
    int32_t* lea_imm32 =
        (int32_t*)(codebuf + *pc +
                   (&code_ptr_left_n_safe_imm - &code_ptr_left_n_start_safe) -
                   4);
    *lea_imm32 = -n;
  } else {
    code_ptr_left_n_sz = &code_ptr_left_n_end - &code_ptr_left_n_start;
    memcpy(codebuf + *pc, &code_ptr_left_n_start, code_ptr_left_n_sz);
  }
  *pc += code_ptr_left_n_sz;
  int32_t* sub_imm32 = (int32_t*)(codebuf + *pc - 4);
  *sub_imm32 = n;
}

void emit_code_ptr_right(char* codebuf, size_t* pc, bool safe_mode) {
  size_t code_ptr_right_sz;
  if (safe_mode) {
    code_ptr_right_sz = &code_ptr_right_end - &code_ptr_right_start_safe;
    memcpy(codebuf + *pc, &code_ptr_right_start_safe, code_ptr_right_sz);
  } else {
    code_ptr_right_sz = &code_ptr_right_end - &code_ptr_right_start;
    memcpy(codebuf + *pc, &code_ptr_right_start, code_ptr_right_sz);
  }
  *pc += code_ptr_right_sz;
}

void emit_code_ptr_right_n(char* codebuf, size_t* pc, uint32_t n,
                           bool safe_mode) {
  size_t code_ptr_right_n_sz;
  if (safe_mode) {
    code_ptr_right_n_sz = &code_ptr_right_n_end - &code_ptr_right_n_start_safe;
    memcpy(codebuf + *pc, &code_ptr_right_n_start_safe, code_ptr_right_n_sz);
    int32_t* lea_imm32 =
        (int32_t*)(codebuf + *pc +
                   (&code_ptr_right_n_safe_imm - &code_ptr_right_n_start_safe) -
                   4);
    *lea_imm32 = n;
  } else {
    code_ptr_right_n_sz = &code_ptr_right_n_end - &code_ptr_right_n_start;
    memcpy(codebuf + *pc, &code_ptr_right_n_start, code_ptr_right_n_sz);
  }
  *pc += code_ptr_right_n_sz;
  int32_t* add_imm32 = (int32_t*)(codebuf + *pc - 4);
  *add_imm32 = n;
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

void emit_code_prolog(char* codebuf, size_t* pc, bool safe_mode) {
  size_t code_prolog_sz;
  if (safe_mode) {
    code_prolog_sz = &code_prolog_end - &code_prolog_start_safe;
    memcpy(codebuf + *pc, &code_prolog_start_safe, code_prolog_sz);
    int32_t* imm32 =
        (int32_t*)(codebuf + *pc + (&code_prolog_start - &code_prolog_start_safe) -
                   4);
    *imm32 = TAPE_SIZE;
  } else {
    code_prolog_sz = &code_prolog_end - &code_prolog_start;
    memcpy(codebuf + *pc, &code_prolog_start, code_prolog_sz);
  }
  *pc += code_prolog_sz;
}

void emit_code_epilog(char* codebuf, size_t* pc, bool safe_mode) {
  size_t code_epilog_sz;
  if (safe_mode) {
    code_epilog_sz = &code_epilog_end - &code_epilog_start_safe;
    memcpy(codebuf + *pc, &code_epilog_start_safe, code_epilog_sz);
  } else {
    code_epilog_sz = &code_epilog_end - &code_epilog_start;
    memcpy(codebuf + *pc, &code_epilog_start, code_epilog_sz);
  }
  *pc += code_epilog_sz;
}

bool execute_bf_code(const char* source, bool safe_mode,
                     const char* dump_name) {
  char* codebuf = mmap(NULL, CODE_BUF_SIZE, PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (codebuf == MAP_FAILED) return false;

  void* tape = calloc(1, TAPE_SIZE);
  if (tape == NULL) {
    munmap(codebuf, CODE_BUF_SIZE);
    return false;
  }

  bool result = emit_code(codebuf, source, safe_mode);
  if (dump_name) {
    FILE* dump = fopen(dump_name, "w");
    if (dump) {
      fwrite(codebuf, CODE_BUF_SIZE, 1, dump);
      fclose(dump);
    }
  }
  if (result) {
    // Make the code executable
    mprotect(codebuf, CODE_BUF_SIZE, PROT_READ | PROT_EXEC);

    // When compiled with -Wpedantic, the following line causes a warning:
    // ISO C forbids conversion of object pointer to function pointer type
    // Suppress the warning by using __extension__
    __extension__ void (*code)(void*) = (void (*)(void*))codebuf;

    code(tape);
  } else {
    fprintf(stderr, "Compilation failed, bracket mismatch\n");
  }

  free(tape);
  munmap(codebuf, CODE_BUF_SIZE);
  return result;
}

void print_usage_and_exit(const char* name, bool error) {
  FILE* stream = error ? stderr : stdout;
  int code = error ? 1 : 0;
  fprintf(stream, "Brainfuck JIT Compiler\n");
  fprintf(stream, "Usage:\n");
  fprintf(stream, "  %s [<FILE>] [options]\n", name);
  fprintf(stream, "Available options:\n");
  fprintf(stream, "  -d <DUMP>   Dump compiled machine code to <DUMP>\n");
  fprintf(stream, "  -s          Safe mode (perform bounds checking)\n");
  fprintf(stream, "  -h, --help  Display this help message\n");
  exit(code);
}

int main(int argc, char* argv[]) {
  // Display help message
  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    print_usage_and_exit(argv[0], false);
  }

  char* source_name = NULL;
  char* dump_name = NULL;
  bool safe_mode = false;

  // Parse command line
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (!source_name)
        source_name = argv[i];
      else
        print_usage_and_exit(argv[0], true);
    } else {
      bool next_arg = false;
      for (int j = 1; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 's') {
          safe_mode = true;
        } else if (argv[i][j] == 'd' && i + 1 < argc) {
          dump_name = argv[i + 1];
          next_arg = true;
        } else {
          print_usage_and_exit(argv[0], true);
        }
      }
      if (next_arg) i++;
    }
  }

  // Execute specified file
  if (source_name) {
    FILE* file = fopen(source_name, "r");
    if (file == NULL) {
      fprintf(stderr, "Failed to open file: %s\n", source_name);
      return 1;
    }

    long file_size = 0;
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = malloc(file_size);
    if (source == NULL) {
      fclose(file);
      perror("malloc");
      return 1;
    }

    size_t count = fread(source, file_size, 1, file);
    if (count != 1) {
      free(source);
      fclose(file);
      perror("fread");
      return 1;
    }
    fclose(file);

    bool result = execute_bf_code(source, safe_mode, dump_name);
    free(source);

    if (!result) {
      if (errno != 0) perror("Execution failed");
      return 1;
    } else {
      return 0;
    }
  } else {
    // No file specified, enter interactive mode
    printf("Brainfuck JIT Compiler\n");
    printf("Press Ctrl+C to exit\n");
    while (true) {
      printf("\n> ");
      fflush(stdout);
      char line[1024];
      if (fgets(line, sizeof(line), stdin) == NULL) break;
      bool result = execute_bf_code(line, safe_mode, dump_name);
      if (!result) {
        if (errno != 0) perror("Execution failed");
      }
    }
  }
}
