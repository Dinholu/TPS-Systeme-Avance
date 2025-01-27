CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/main.c src/parser/parser.c src/executor/executor.c src/builtins/builtins.c
OBJ = $(SRC:.c=.o)
EXEC = minishell

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
