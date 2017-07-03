CC=gcc
CFLAGS=-Wall -I.

# Main, compiles the actual shell
target: myfile.o
	$(CC) -o myfile myfile.o redirect.o tokenizer.o
	@echo "Done"

myfile.o: myfile.c redirect.o tokenizer.o
	$(CC) -o myfile.o -c myfile.c $(CFLAGS)

redirect.o: redirect.c redirect.h
	$(CC) -o redirect.o -c redirect.c $(CFLAGS)

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) -o tokenizer.o -c tokenizer.c $(CFLAGS)

clean:
	rm -f tokenizer.o || :
	rm -f redirect.o || :
	rm -f myfile.o || :
	@echo "Done Cleaning"
