# opcoder

Outputs the machine code from Assembly instructions

## Install dependencies

    - http://www.keystone-engine.org

## Build

    make

## Usage

    $ ./opcoder -aarm 'mov r0, r0'
    0x0000a0e1
    \x00\x00\xa0\xe1
    00 00 a0 e1
    Compiled: 4 bytes, statements: 1

## Other tools

Thanks to @psylinux, I was made aware we can actually get similar results using rasm2 tool from [r2](https://radare.org/r/) framework:

    $ rasm2 -aarm nop
    0000a0e1
    
    $ rasm2 -aarm -d 0000a0e1
    mov r0, r0
