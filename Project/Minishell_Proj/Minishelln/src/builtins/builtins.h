#ifndef BUILTINS_H
#define BUILTINS_H

/**
 * Change le répertoire courant.
 * @param args Tableau d'arguments de la commande.
 * @return 1 si la commande a été exécutée, 0 sinon.
 */
int builtin_cd(char **args);

/**
 * Affiche le répertoire courant.
 * @return 1 si la commande a été exécutée, 0 sinon.
 */
int builtin_pwd();

/**
 * Quitte le shell.
 * @return 1 si la commande a été exécutée, 0 sinon.
 */
int builtin_exit();

/**
 * Affiche les arguments passés en paramètre.
 * @param args Tableau d'arguments de la commande.
 * @return 1 si la commande a été exécutée, 0 sinon.
 */
int builtin_echo(char **args);

#endif // BUILTINS_H
