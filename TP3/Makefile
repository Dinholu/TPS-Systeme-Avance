# Nom des exécutables
EXEC1 = thread_sort
EXEC2 = threaded_sort

# Options de compilation
CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
LDFLAGS = -lpthread

# Fichiers sources
SRC1 = src/thread_sort.c
SRC2 = src/threaded_sort.c

# Compilation des programmes
all: $(EXEC1) $(EXEC2)

$(EXEC1): $(SRC1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(SRC1) $(LDFLAGS)

$(EXEC2): $(SRC2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(SRC2) $(LDFLAGS)

# Exécuter les programmes
run_thread_sort: $(EXEC1)
	./$(EXEC1)

run_threaded_sort: $(EXEC2)
	./$(EXEC2)

# Nettoyer les fichiers binaires
clean:
	rm -f $(EXEC1) $(EXEC2)

# Nettoyer et recompiler
rebuild: clean all
