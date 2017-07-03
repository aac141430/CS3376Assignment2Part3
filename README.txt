Akshay Chitale
Assignment 2 - README.txt
CS 3376.0W1 (Summer 2017)
Email: aac141430@utdallas.edu


=== COMPILATION INSTRUCTIONS ===
To compile and get the myfile executable, compile at a *nix terminal by changing to the
directory of this README and using the command:
$ make && make clean
To compile and remove the in-between object files. To retain the object files after
compiling, you can instead run:
$ make


=== EXECUTION INSTRUCTIONS ===
To run the myfile executable, first change to the directory of this README and
compile. Run in the provided test directory as follows:
$ cd ./test/
$ ../myfile
To run without echo the command entered, use the --noecho option:
$ cd ./test/
$ ../myfile --noecho


=== FILE DESCRIPTIONS ===
- README.txt - this file
- Makefile - Makefile to compile the *.c and *.h files in this directory
test - directory in which to test myfile
- tokenizer.c - structs to parse and tokenize input (modified from Assignment 1)
    -> tokenizer.h - header file for tokenizer.c
- redirect.c - funcitons to perform the read, write, and append commands
    -> redirect.h - header file for redirect.c
- myfile.c - program to interactively redirect file contents
    -> myfile - compiled executable of myfile.c, made using "make && make clean"
    -> myfile.log - log of test run of myfile

