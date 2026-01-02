# chip8-emulator

A chip-8 emulator made for educational purposes in C.

The exemple programs come from [mattmikolay's repository](https://github.com/mattmikolay/chip-8)
wich was my main source of documentation for this project.

## Launching
For now, the program path is hardcoded in `Source/main.c`.

The project was made using the [xmake](https://xmake.io/) build tool.
Install it and then use the `xmake` command to compile and run with `xmake run`.

## Todo
- Implement remaining instructions (eg. sound and inputs) ;
Was only tested with the hearth monitor exemple for now (with no sound).

- Infinite subroutine stack.

- handle sprites rendered outside the screen

- Cross platform and customizable sleeping.

- Enter program path & zoom after compilation.

- switching from char to uint8_t

- Make timers go down slower than the running clock (constant 60 Hz)
