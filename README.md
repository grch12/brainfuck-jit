# brainfuck-jit

This is a handwritten Brainfuck JIT compiler for x86-64 Linux. Zero dependencies. Directly compiles Brainfuck code into x86-64 machine code. No IR, LLVM, libgccjit, etc.

## Usage

To use the compiler, you can provide a Brainfuck source file as an argument when running the program:

```bash
./bfjit <file.bf>
```

Alternatively, you can enter interactive mode by running the program without any arguments. In this mode, you can enter Brainfuck code one line at a time and see the output.

Available options are:

- `-s`: Safe mode (performs bounds checking)
- `-d <DUMP>`: Dump compiled machine code to `<DUMP>`

## Compilation

To compile the program, use the following command:

```bash
make
```

This will create an executable named `bfjit` in the current directory.

## Execution

The compiled program will execute the Brainfuck code and output the result. If an error occurs during execution, an error message will be displayed.

## Performance

|   | brainfuck-jit | brainfuck-jit (safe mode) | [qdb](https://brainfuck.org/qdb.c) | [jitbf](https://github.com/none-None1/jitbf) |
|---|---|---|---|---|
| [hanoi.bf](https://esoteric.sange.fi/brainfuck/bf-source/prog/hanoi.bf) | 4.646s | **4.558s** | 11.397s | 6.747s |
| [mandelbrot.b](https://esoteric.sange.fi/brainfuck/bf-source/prog/mandelbrot.b) | **1.223s** | 1.484s | 9.871s | 2.391s |

 - brainfuck-jit: This project
 - qdb: A simple interpreter implemented in C
 - jitbf: A JIT compiler written in Python, using llvmlite

As you can see, brainfuck-jit is the fastest, and safe mode does not significantly impact performance.

## License

This program is licensed under the MIT License. See LICENSE.txt for more details.
