# Setup a variable to reference the desired compiler
CC = gcc
SRC = src
BIN = bin

# Setup a variable to hold desired compiler flags
CFLAGS = -Wall -Wextra -Wpointer-arith -Wstrict-prototypes -std=c99 -g

# Declaring the build, test and clean rules as phony means that no artifact
#   is created by these rules with a cooresponding name.  Therefore, the
#   rule should run when executed.
.PHONY: build test clean memtest-myps memtest-mytests

# The build rule depends upon the myprog artifact existing.  If it does not, make
#   will look for a rule to create it.
build: $(BIN)/myps $(BIN)/mytests


$(BIN)/myps: $(SRC)/myps.c $(SRC)/ProcEntry.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $^ -o $@ 

memtest-myps: $(BIN)/myps
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes $(BIN)/myps > /dev/null

$(BIN)/mytests: $(SRC)/mytests.c $(SRC)/ProcEntry.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $^ -o $@ 

memtest-mytests: $(BIN)/mytests
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes $(BIN)/mytests

clean:
	rm -f *.o *.d a.out $(BIN)/*
