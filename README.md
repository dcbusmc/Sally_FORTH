# Sally_FORTH
This project creates a basic interpreter for our invented language: Sally FORTH

http://www.forth.org/

All code written and compiled on Microsoft Visual Studio.
  Note: running on Linux system has different output with end of line characters.  Use of extra '.' may be necessary.

This interpreter accepts numerical input and performs calculations in Reverse Polish Notation.
In addition to arithmetic operations, this interpreter can also perform comparisons, logical operations, stack and variable operations,
as well as IF statements and DO UNTIL loop constructs.

There are three .cpp drivers provided for input acceptance.  The first option allows command line typing of input.  The second option
(for which the provided test file is written) accepts command line input for the file and executes the Sally FORTH language found in
the file.  The third driver is an alternative to the second for command line file search.
