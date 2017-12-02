Unlimited Register Machine
=====================

An implementation of an interpreted unlimited register machine (URM),
as described in Cutland, N, "Computability", Cambridge Univ Press.
The program takes input from a file if a filename is given, otherwise
from the standard input.

The URM has the following instruction format:

OPCODE OP1 [OP2 [OP3]]

where OPCODE is either 1, 2, 3, or 4, and OP1-3 are
the operands.

Each instruction is terminated by a newline.

Instructions
----------

* Z (OPCODE = 1): zero the register indicated by OP1, increment
program count.

* S (OPCODE = 2): increment the register indicated by OP1, increment
program count.

* T (OPCODE = 3): transfer the value in register OP2 to
register OP1, increment program count.

* J (OPCODE = 4): if the value in register OP1 equals the
value in register OP2, set program count to the value in
register OP3, otherwise increment program count.

Register Initial State
-----------

All register locations are initialised to zero.

Program Termination
------------

Execution terminates when the program count is set to value
greater or equal to the number of instructions in the program.

The program prints its result (the contents of the first register)
on exit.


Example
-------------

Adapted from Cutland, p.18, here's a program to add two numbers.
The only modification is the addition of register setting lines.

2 1        1: set register 1 to 1
2 1        2: set register 1 to 2
2 2        3: set register 2 to 1
2 2        4: set register 2 to 2 
4 3 2 9    5: if reg 3 = reg 2 jump to 9:
2 1        6: incr reg 1
2 3        7: incr reg 3
4 1 1 5    8: jump to 5:

$ ./urm test.prg 
4

Testing
-------------

In need.



