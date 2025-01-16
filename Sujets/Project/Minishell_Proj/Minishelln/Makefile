CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = main.c parser.c executor.c builtins.c
OBJ = $(SRC:.c=.o)
EXEC = my_shell

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
