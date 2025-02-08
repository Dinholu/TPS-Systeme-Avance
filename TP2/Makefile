# Nom de l'exécutable
TARGET = tp2_systeme

# Fichiers source
SRC = src/main.c

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Exécuter l'exercice 1 : "Fork Yourself"
run1: $(TARGET)
	./$(TARGET) 1

# Exécuter l'exercice 2 : Affichage message
run2: $(TARGET)
	./$(TARGET) 2 Bonjour

# Exécuter l'exercice 3 : Redirection de flux (nécessite un programme externe)
run3: $(TARGET)
	./$(TARGET) 3 ./affiche

# Exécuter l'exercice 4 : Pipe Redirection
run4: $(TARGET)
	./$(TARGET) 4

# Nettoyage des fichiers compilés
clean:
	rm -f $(TARGET)
