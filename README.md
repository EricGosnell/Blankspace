Blankspace
==========

[![Build Status](https://travis-ci.org/koturn/Whitespace.png)](https://travis-ci.org/koturn/Whitespace)

Blankspace interpreter and C-translator.


## Usage

### Run Blankspace program

```sh
$ ./blankspace [Blankspace source file]
```

### Translate Blankspace to C

Specify ```-t``` flag and ```-o``` flag.

```sh
$ ./blankspace [Blankspace source file] -tests -o out.c
```

If you don't specify output file with ```-o```, C source code will output
stdout.

### Write and execute Blankspace
```sh
./blankspace.out tests/rs.txt -s -o output.txt
```
Specify input file with first command. Input file should be written with newlines, S to represent spaces, and T to represent tabs. This command will create output.txt which will be written in Blankspace.s 

```sh
./blankspace.out output.txt
```
This will execute the Blankspace program.


### Options

Options                            | Function
-----------------------------------|------------------------------------
```-b```, ```--bytecode```         | Show code in hexadecimal
```-f```, ```--filter```           | Visualize blankspace source code
```-h```, ```--help```             | Show help and exit
```-m```, ```--mnemonic```         | Show byte code in mnemonic format
```-o FILE```, ```--output=FILE``` | Specify output filename
```-t```, ```--translate```        | Translate brainfuck to C source code
```-s```,```--convert```           | Convert input file to blankspace (S and T for space and tab)


## Build

Use [Makefile](Makefile).

```sh
$ make
```

If you want to build with MSVC, use [msvc.mk](msvc.mk).
[msvc.mk](msvc.mk) is written for nmake.

```sh
> nmake /f msvc.mk
```


## Dependent libraries

#### MSVC only

- [getopt clone](https://github.com/koturn/getopt)


## References

- [http://compsoc.dur.ac.uk/whitespace/](http://compsoc.dur.ac.uk/whitespace/)


## LICENSE

This software is released under the MIT License, see [LICENSE](LICENSE).
