SDIR = ./src
IDIR = ./include

SRC := scheduler sys ff rr mem sjf smlswp
OBJ := $(SRC:%=$(SDIR)/%.o)
SRC := $(SRC:%=$(SDIR)/%.c)

# Memory sanitiser
SMEM = -fsanitize=address

ifeq ($(s), 1)
	# Debug target
	CC := clang
	CFLAGS := -Wall -Wextra -g -I$(IDIR) $(SMEM)
else
	# Build target
	CC = gcc
	CFLAGS = -Wall -Wextra -g -I$(IDIR)
endif

# Program
EXE = scheduler

PHONY: all clean

all: clean $(EXE)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)
	@rm -r -f $(SDIR)/*.o

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -r -f $(SDIR)/*.o
	@rm -f scheduler
