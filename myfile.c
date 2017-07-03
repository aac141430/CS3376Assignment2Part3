// Akshay Chitale
// Assignment 2 - Part 3 - myfile.c
// CS 3376.0W1 (Summer 2017)
// Email: aac141430@utdallas.edu
//
// This is the main file that the assignment asks to be written

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include "tokenizer.h" // for custom input parsing/tokenizing
#include "redirect.h"  // for read, write, and append commands

// All input getting/parsing handled in tokenizer.c, except command name:
int getCommandType(ParsedInput*, int);
void printUsage(FILE*);
void printMainUsage();

int main(int argc, char **argv) {
	int exitCmd = 0;
	pid_t pid;
	int status;

	// Check if echo commands
	int echo = 1;
	if(argc == 2 && !strcmp(argv[1], "--noecho")) {
		echo = 0;
	}
	else if(argc > 1) {
		// Usage for whole file, not for within shell
		printMainUsage();
		exit(1);
	}

	// Make temp file
	int tempfd = openTempFile("temp.txt");

	// Keep testing commands until exit true
	while (!exitCmd) {
		// Read input line while more input
		// i.e. when line ends in \,|,etc
		Input* in = (Input*)malloc(sizeof(Input*));
		initializeInput(in, 128);
		do {
			// Prompt for input and read
			printPrompt(in->readmore ? "> " : "{myfile}$ ");
			readInput(in);
		} while (in->readmore);

		// Parse input
		ParsedInput* pin = (ParsedInput*)malloc(sizeof(ParsedInput*));
		initializeParsedInput(pin, in);
		determineInputTypes(pin);

		/* Place your code to check "exit". If so, then exit */
		// Exit if command is exit
		exitCmd = commandIsExit(pin);
		// Otherwise, print input and execute
		if(echo) {
			printInputs(pin);
		}
		if(!exitCmd) {
			if((pid=fork()) == -1) {
				fprintf(stderr, "shell: can't fork: %s\n",
					strerror(errno));
				continue;
			} else if (pid == 0) {
				/* child process to do each command
				   – place your code here to handle
				   read, write, append */
				
				// Go through parsed info
				int pos = 0;
				int problems = 0;
				while(pos < pin->numInputs) {
					switch(getCommandType(pin, pos)) {
					case 0:
						// Read from file
						problems = readToTemp(tempfd, pin->inputs[pos+2]);
						pos += 3;
						break;
					case 1:
						// Write to file
						problems = writeFromTemp(tempfd, pin->inputs[pos+2]);
						pos += 3;
						break;
					case 2:
						// Append to file
						problems = appendFromTemp(tempfd, pin->inputs[pos+2]);
						pos += 3;
						break;
					case 3:
						// Read from stdin
						lseek(tempfd, 0, SEEK_SET);
						problems = ftruncate(tempfd, 0);
						problems |= blind_copy(STDIN_FILENO, tempfd);
						pos += 1;
						break;
					case 4:
						// Write to stdout
						lseek(tempfd, 0, SEEK_SET);
						problems = blind_copy(tempfd, STDOUT_FILENO);
						pos += 1;
						break;
					case 5:
						// Append to stdout (i.e.write)
						lseek(tempfd, 0, SEEK_SET);
						problems = blind_copy(tempfd, STDOUT_FILENO);
						pos += 1;
						break;
					case -3:
						// Is a pipe, go to next command
						pos += 1;
						break;
					case -2:
						// Bad index error
						fprintf(stderr, "Index Error: while parsing inputs\n");
						problems = 1;
						break;
					case -1:
						// Incorrect command error
						fprintf(stderr, "Command Error: Command format not recognized\n");
						printUsage(stderr);
						problems = 1;
						break;
					}
					// Stop inputs
					if(problems) {
						pos = pin->numInputs;
					}
				}
				
				// Cleanup
				destroyParsedInput(pin);
				pin = NULL;
				destroyInput(in);
				exit(EX_OK);
			}
		
			if ((pid=waitpid(pid, &status, 0)) < 0) {
				fprintf(stderr, "shell: waitpid error: %s\n",
					strerror(errno));
			}
		}

		// Cleanup
		destroyParsedInput(pin);
		pin = NULL;
		destroyInput(in);
		in = NULL;
	}
	close(tempfd);
	unlink("temp.txt");
	exit(EX_OK);
}

int getCommandType(ParsedInput* pin, int pos) {
	// Return 0 if read, 1 if write, 2 if append
	// Return 3 if read, 4 if write, 5 if append if from stdin/stdout
	// Return -1 if invalid command format
	// Return -2 if index out of range
	// Return -3 if pipe (ignore for now, next command)
	if(pos < 0 || pos >= pin->numInputs) {
		return -2;
	}
	if(!strcmp(pin->types[pos], "Pipe")) {
		return -3;
	}
	if(strcmp(pin->types[pos], "Command")) {
		return -1;
	}
	// Check if format is command only (i.e. to stdin/stdout)
	if(pos + 2 >= pin->numInputs) {
		if(pos + 1 == pin->numInputs) {
			if(!strcmp(pin->inputs[pos], "read")) {
				return 3;
			}
			else if(!strcmp(pin->inputs[pos], "write")) {
				return 4;
			}
			else if(!strcmp(pin->inputs[pos], "append")) {
				return 5;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	// Make sure command is followed by pipe or end of command
	if(pos + 3 == pin->numInputs || !strcmp(pin->types[pos+3], "Pipe")) {
		// Reached here, then format is command redirect fname
		if(strcmp(pin->types[pos+1], "File Redirection") ||
		   strcmp(pin->types[pos+2], "\tFile")) {
			return -1;
		}
		if(!strcmp(pin->inputs[pos], "read") &&
		   !strcmp(pin->inputs[pos+1], "<")) {
			return 0;
		}
		else if(!strcmp(pin->inputs[pos], "write") &&
			!strcmp(pin->inputs[pos+1], ">")) {
			return 1;
		}
		else if(!strcmp(pin->inputs[pos], "append") &&
			!strcmp(pin->inputs[pos+1], ">>")) {
			return 2;
		}
		else {
			return -1;
		}
	}
	else if(pos + 1 == pin->numInputs || !strcmp(pin->types[pos+1], "Pipe")) {
		// Reached here, then format is command
		if(!strcmp(pin->inputs[pos], "read")) {
			return 3;
		}
		else if(!strcmp(pin->inputs[pos], "write")) {
			return 4;
		}
		else if(!strcmp(pin->inputs[pos], "append")) {
			return 5;
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}

void printUsage(FILE* file) {
	fprintf(file, "usage: command [redirect file]\n");
	fprintf(file, "\tcommand: read, write, append, or exit\n");
	fprintf(file, "\tredirect: < for read, > for write, >> for append\n");
	fprintf(file, "\tfile: regular file name of source/dest of redirect\n");
}

void printMainUsage() {
	fprintf(stderr, "usage: myfile [--noecho]\n");
	fprintf(stderr, "\t--noecho: Do not echo command upon parsing\n");
}
