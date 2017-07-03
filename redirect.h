// Akshay Chitale
// Assignment 2 - Part 3 - redirect.h
// CS 3376.0W1 (Summer 2017)
// Email: aac141430@utdallas.edu
//
// This is a header for redirect.c, which implements the reading,
// writing, and appending functions with a temporary file

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFFSIZE 32768

int openTempFile(const char*); // pass in "temp.txt"
// Close temp file with close(tempfd), delete file if desired

// All below take file descriptor of temp file, name of other file
int readToTemp(int, const char*);
int writeFromTemp(int, const char*);
int appendFromTemp(int, const char*);
int blind_copy(int, int);
