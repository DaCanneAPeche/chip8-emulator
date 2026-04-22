# chip8-emulator

A chip-8 emulator made for educational purposes in C.

The exemple programs come from [mattmikolay's repository](https://github.com/mattmikolay/chip-8)
wich was my main source of documentation for this project.

## Launching
For now, the program path is hardcoded in `Source/main.c`.

The project was made using the [xmake](https://xmake.io/) build tool.
Install it and then use the `xmake` command to compile and run with `xmake run`.

## Debugger
Press the `Space` key to toogle the debugger. It shows information concerning 
the registers, the pointer counter, the timers...

Screenshot (with the `chipquarium` exemple) :
![Debugger screenshot](./img/debugger.png)

## Todo
- Enter program path & zoom after compilation.

- Change input map
