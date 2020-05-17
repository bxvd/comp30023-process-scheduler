SDIR = ./src
IDIR = ./include

SRC := scheduler
OBJ := $(SRC:%=$(SDIR)/%.o)
SRC := $(SRC:%=$(SDIR)/%.c)

# Program options
OPT := -f processes.txt -a rr -m v -d -s 256

# Memory sanitiser
SMEM = -fsanitize=address -g

ifeq ($(s), 1)
	# Debug target
	CC = clang
	CFLAGS = -Wall -Wextra -I$(IDIR) $(SMEM)
	s := on
else
	# Build target
	CC = gcc
	CFLAGS = -Wall -Wextra -I$(IDIR)
	s := off
endif

# Program
EXE = scheduler

PHONY: all debug clean

all: clean $(EXE)

debug: clean $(EXE)
	./$(EXE) $(OPT)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -r -f *.o
	@rm -f scheduler
