# Variables
CC=gcc
CFLAGS=-Wall -c -I $(INC_DIR)
LDFLAGS=-I $(INC_DIR)
GCOVFLAGS=-O0 --coverage -lgcov -Wall -g

SRC_DIR=./src
INC_DIR=./include
BIN_DIR=./bin
DOC_DIR=./doc
GCOV_DIR=./gcov

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:.c=.o)
EXEC=$(BIN_DIR)/skeleton

GEXEC=$(GCOV_DIR)/skeleton.cov
AR_NAME=archive_skeleton.tar.gz
LCOV_REPORT=report.info

# Règle principale pour construire le projet
all: $(EXEC)

# Compilation des fichiers .o depuis src/ vers src/
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

# Création de l'exécutable dans bin/
$(EXEC): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(EXEC) $(OBJ) $(LDFLAGS)

# Génération de l'exécutable pour gcov dans gcov/
$(GEXEC): $(SRC)
	@mkdir -p $(GCOV_DIR)
	$(CC) $(GCOVFLAGS) -o $(GEXEC) $(SRC) $(LDFLAGS)

# Génération de la documentation
doc:
	doxygen $(DOC_DIR)/doxygen.conf

# Exécution de l'instrumentation gcov et génération du rapport de couverture
gcov: $(GEXEC)
	$(GEXEC) -h
	$(GEXEC) -i input -o output -v

	# Déplacer les fichiers de couverture dans gcov/
	find . -name "*.gcno" -exec mv {} $(GCOV_DIR) \;
	find . -name "*.gcda" -exec mv {} $(GCOV_DIR) \;

	# Générer les rapports gcov et lcov
	gcov -o $(GCOV_DIR) $(GEXEC)
	lcov -o $(GCOV_DIR)/$(LCOV_REPORT) -c -f -d $(GCOV_DIR)
	genhtml -o $(GCOV_DIR)/report $(GCOV_DIR)/$(LCOV_REPORT)

# Créer une archive du projet
package: gcov doc all
	rm -rf $(AR_NAME)
	tar cvfz $(AR_NAME) ./*

# Nettoyage des fichiers objets
clean:
	rm -rf $(SRC_DIR)/*.o

# Nettoyage complet
mrproper: clean
	rm -rf $(BIN_DIR)/*
	rm -rf $(DOC_DIR)/latex/
	rm -rf $(DOC_DIR)/html/
	rm -rf $(GCOV_DIR)/*

.PHONY: doc gcov package clean mrproper
