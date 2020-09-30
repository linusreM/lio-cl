<br>

# Lab IO card test package


This project contains testing/utility firmware for the **GD32VF** based development board and for the accompanying peripheral IO-card, designed for educational purposes at KTH.

The firmware consists of a command line interface from which you can test different functionality of the board.

## Using the firmware

To use the application load the program into the board under test. Then connect the board to your computer using the USB connector. Oopen up a serial terminal and connect to the board, usually /dev/ttyACM0 in linux. Baud rate should not matter since it is only emulating a serial adapter. 

Then press **k** when prompted to start the interpreter.

Run a command by typing it's name and pressing enter.

Arguments to functions are added by typing them after the command with a space as a separator.

Type **help** to see the available commands and their usage.
<br><br>

## Running scripts

Also included in this package (**scripts/**) are some example scripts using the command line to automate tasks.

To run a script you need **python 3** and **pySerial** installed.

When writing scripts make sure that each line sent is terminated by **\r** to trigger execution. Also make sure not to send commands faster than they can be consumed. The interpreter only buffers one command.
<br><br>

## Adding commands

The firmware is written so that it is easy to add new commands. To add a new command simply write a function in the form of
```c
    int example_command(int argc, char* argv[]);
```
**argc** will be the number of arguments supplied with the command. Arguments to the function are supplied as separate strings in **argv[]**. The first argument is always the name of the command. If you need for example an integer just parse the appropriate index in **argv[]** like:
```c
    n = itoa(argv[i]);
```
When you have written the command function add it to the **vector_table[]** array in the following form:
```c
    {"command-name", &example_command, 
     "[argument 1] [argument 2] How to use the command"},
```
Preferrably don't make the usage descriptions longer than a line. (~80 chars)
If you need to be able to convey more information on the usage consider using a -h flag in your command function to supply more info

