SDIR = ./src
IDIR = ./include

SRC := scheduler
OBJ := $(SRC:%=$(SDIR)/%.o)
SRC := $(SRC:%=$(SDIR)/%.c)

# Build target
CC = gcc
CFLAGS = -Wall -Wextra -g -I$(IDIR)

# Program
EXE = scheduler

PHONY: all clean

all: clean $(EXE)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -r -f *.o
	@rm -f scheduler
