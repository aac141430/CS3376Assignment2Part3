// Akshay Chitale
// Assignment 2 - Part 3 - tokenizer.h
// CS 3376.0W1 (Summer 2017)
// Email: aac141430@utdallas.edu
//
// NOTE: This is a header for tokenizer.c, a modified copy of
// the tokenizer written for Assignment 1, Part 4
// This is so the input in Assignment 2, Part 3 can be easily parsed.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Struct for input
typedef struct Input {
	// Size is amount used, length is buff size
	int readmore, size, length;
	char* buff;
} Input;
// Input struct prototypes
void initializeInput(Input*, int);
void readInput(Input*);
void setReadMore(Input*);
void destroyInput(Input*);

// Struct for parsed input
typedef struct ParsedInput {
	// 1D array of cstrings with input and input type
	int numInputs;
	char** inputs;
	char** types;
} ParsedInput;
// ParsedInput struct prototypes
void initializeParsedInput(ParsedInput*, Input*);
void determineInputTypes(ParsedInput*);
void printInputs(ParsedInput*);
void destroyParsedInput(ParsedInput*);

// Function prototypes
void printPrompt(const char*);
int commandIsExit(ParsedInput*);
