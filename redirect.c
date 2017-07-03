// Akshay Chitale
// Assignment 2 - Part 3 - redirect.c
// CS 3376.0W1 (Summer 2017)
// Email: aac141430@utdallas.edu
//
// This is file implements the reading, writing,
// and appending functions with a temporary file

#include "redirect.h"

int openTempFile(const char* tempf) {
	// If file not found, create with 600 permissions
	// Open and return file descriptor
	return open(tempf, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
}

// All below take file descriptor of temp file, name of other file
int readToTemp(int tempfd, const char* from) {
	// Clear temp.txt first, then do read
	if(ftruncate(tempfd, 0) == -1) {
		fprintf(stderr, "Read Error: Cannot clear temp file\n");
		return -1;
	}
	int fromfd;
	if((fromfd=open(from, O_RDONLY)) == -1) {
		fprintf(stderr, "Read Error: Cannot open %s\n", from);
		return -1;
	}
	lseek(tempfd, 0, SEEK_SET);
	return blind_copy(fromfd, tempfd);
}
int writeFromTemp(int tempfd, const char* to) {
	int tofd;
	if((tofd=open(to, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
		fprintf(stderr, "Write Error: Cannot open %s\n", to);
		return -1;
	}
	// Clear file to write first, then do write
	if(ftruncate(tofd, 0) == -1) {
		fprintf(stderr, "Write Error: Cannot clear %s\n", to);
		return -1;
	}
	lseek(tempfd, 0, SEEK_SET);
	return blind_copy(tempfd, tofd);
}
int appendFromTemp(int tempfd, const char* to) {
	int tofd;
	if((tofd=open(to, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR)) == -1) {
		fprintf(stderr, "Append Error: Cannot open %s\n", to);
		return -1;
	}
	lseek(tempfd, 0, SEEK_SET);
	return blind_copy(tempfd, tofd);
}

int blind_copy(int from, int to) {
	int n;
	char buf[BUFFSIZE];

	while ((n = read(from, buf, BUFFSIZE)) > 0) {
		if (write(to, buf, n) != n) {
			fprintf(stderr, "Redirect Error: Write Error\n");
			return -1;
		}
	}

	if (n < 0) {
		fprintf(stderr, "Redirect Error: Read Error\n");
		return -1;
	}
	return 0;
}
