#ifndef EXECUTOR_H
#define EXECUTOR_H

/**
 * @brief Exécute une commande.
 * @param args Tableau d'arguments de la commande.
 */
void execute_command(char **args);

/**
 * @brief Exécute une série de commandes séparées par des pipes.
 * @param commands Tableau de commandes.
 */
void execute_piped_commands(char **commands);

#endif // EXECUTOR_H
