// Akshay Chitale
// Assignment 2 - Part 3 - tokenizer.c
// CS 3376.0W1 (Summer 2017)
// Email: aac141430@utdallas.edu
//
// NOTE: This is a modified version of the code used for myshell5.c,
// the tokenizer written for Assignment 1, Part 4
// This is so the input in Assignment 2, Part 3 can be easily parsed.

#include "tokenizer.h"

void initializeInput(Input* in, int length) {
	in->readmore = 0;
	in->size = 0;
	in->length = length;
	in->buff = (char*)calloc(length, sizeof(char));
	return;
}

void readInput(Input* in) {
	// Load chars, realloc if needed
	char cin = 'x'; // Initial value used only to enter loop
	while(cin != EOF) {
		// Resize buffer if needed
		if(in->size >= in->length - 1) {
			int i = in->length;
			in->length *= 2;
			in->buff = (char*)realloc(in->buff, in->length);
			for( ; i < in->length; i++) {
				in->buff[i] = '\0';
			}
		}
		cin = getchar();
		if(cin == '\n') {
			break; // Do not include newline in input string
		}
		in->buff[in->size++] = cin;
	}
	setReadMore(in);
	return;
}

void setReadMore(Input* in) {
	// Read through chars, keeping track of pipes and file redirects
	// If pipe or file redirect is left unfinished, or \, readmore is true
	int i;
	for(i = 0; i < in->size; i++) {
		if(in->buff[i] == '|' || in->buff[i] == '>' || in->buff[i] == '<') {
			in->readmore = 1;
		}
		else if(in->readmore && in->buff[i] != ' ' && in->buff[i] != '\t') {
			in->readmore = 0;
		}
	}
	if(in->size > 0 && in->buff[in->size - 1] == '\\') {
		in->buff[in->size-- - 1] = '\0';
		in->readmore = 1;
	}
	return;
}

void destroyInput(Input* in) {
	free(in->buff);
	in->buff = NULL;
	free(in);
	in = NULL;
	return;
}

void initializeParsedInput(ParsedInput* pin, Input* in) {
	// First, make space for all the whitespace-delimited input 
	pin->numInputs = 0;
	int i, onWS, currLen;
	onWS = 1;
	currLen = 0;
	for(i = 0; i < in->size; i++) {
		if(onWS && in->buff[i] != ' ' && in->buff[i] != '\t') {
			// Start new word
			pin->numInputs++;
			if(pin->numInputs == 1) {
				pin->inputs = (char**)calloc(1, sizeof(char*));
			}
			else {
				pin->inputs = (char**)realloc(pin->inputs, pin->numInputs * sizeof(char*));
			}
			currLen = 0;
			onWS = 0;
		}
		else if(!onWS && (in->buff[i] == ' ' || in->buff[i] == '\t')) {
			// End last word, use 5 extra chars as cushion
			pin->inputs[pin->numInputs - 1] = (char*)calloc(currLen + 5, sizeof(char));
			onWS = 1;
		}
		currLen++;
	}
	// Account for if last char is not whitespace, end that last word
	if(!onWS) {
		pin->inputs[pin->numInputs - 1] = (char*)calloc(currLen + 5, sizeof(char));
	}
	// Then fill in the inputs
	int inputNum = 0;
	onWS = 1;
	currLen = 0;
	for(i = 0; i < in->size; i++) {
		if(onWS && in->buff[i] != ' ' && in->buff[i] != '\t') {
			// Start a new word
			inputNum++;
			currLen = 0;
			onWS = 0;
		}
		else if(!onWS && (in->buff[i] == ' ' || in->buff[i] == '\t')) {
			// End last word
			onWS = 1;
		}
		if(!onWS) {
			pin->inputs[inputNum - 1][currLen] = in->buff[i];
		}
		currLen++;
	}
	return;
}

void determineInputTypes(ParsedInput* pin) {
	// Possible types:
	// Command: first word
	// Option(s): starts with '-' and is after command
	// Arguments: are after command
	// File Redirection: '<', '>', or '>>'
	// File: is after file redirection
	// Pipe: '|'
	pin->types = (char**)calloc(pin->numInputs, sizeof(char*));
	int i, state;
	state = 0; // 0 is looking for command, 1 is looking for args/opts, 2 is looking for file
	for(i = 0; i < pin->numInputs; i++) {
		// Make space for string, max len < 20
		pin->types[i] = (char*)calloc(20, sizeof(char));
		if(state == 0) {
			strcpy(pin->types[i], "Command");
			state = 1;
		}
		else if(state == 1) {
			char start = pin->inputs[i][0]; // initialize functions guarantees at least one char
			switch(start) {
				case '<':
				case '>':
					strcpy(pin->types[i], "File Redirection");
					state = 2;
					break;
				case '|':
					strcpy(pin->types[i], "Pipe");
					state = 0;
					break;
				case '-':
					strcpy(pin->types[i], "\tOption(s)");
					break;
				default:
					strcpy(pin->types[i], "\tArgument");
					break;
			}
		}
		else if(state == 2) {
			strcpy(pin->types[i], "\tFile");
			state = 1;
		}
	}
	return;
}

void printInputs(ParsedInput* pin) {
	int i;
	for(i = 0; i < pin->numInputs; i++) {
		printf("%s: %s\n", pin->types[i], pin->inputs[i]);
	}
}

void destroyParsedInput(ParsedInput* pin) {
	int i;
	for(i = 0; i < pin->numInputs; i++) {
		free(pin->inputs[i]);
		pin->inputs[i] = NULL;
		free(pin->types[i]);
		pin->types[i] = NULL;
	}
	free(pin->inputs);
	pin->inputs = NULL;
	free(pin->types);
	pin->types = NULL;
	free(pin);
	pin = NULL;
	return;
}

void printPrompt(const char*  prompt) {
	printf(prompt);
	return;
}

int commandIsExit(ParsedInput* pin) {
	if(pin->numInputs == 1 && !(strcmp(pin->inputs[0], "exit"))) {
		return 1;
	}
	else {
		return 0;
	}
}
