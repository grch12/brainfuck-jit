# brainfuck-jit

This is a handwritten Brainfuck JIT compiler for x86-64 Linux. Zero dependencies. Directly compiles Brainfuck code to x86-64 machine code. No IR, LLVM, libgccjit, etc.

## Usage

To use the compiler, you can provide a Brainfuck source file as an argument when running the program:

```bash
./bfjit <file.bf>
```

Alternatively, you can enter interactive mode by running the program without any arguments. In this mode, you can enter Brainfuck code one line at a time and see the output.

## Compilation

To compile the program, use the following command:

```bash
make
```

This will create an executable named `bfjit` in the current directory.

## Execution

The compiled program will execute the Brainfuck code and output the result. If an error occurs during execution, an error message will be displayed.

## License

This program is licensed under the MIT License. See LICENSE.txt for more details.
