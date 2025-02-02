#ifndef EXECUTOR_H
#define EXECUTOR_H

/**
 * @brief Exécute une commande.
 * @param args Tableau d'arguments de la commande.
 */
void execute_command(char **args);

/**
 * @brief Vérifie si une commande est en arrière-plan.
 * @param command Commande à vérifier.
 * @return 1 si la commande est en arrière-plan, 0 sinon.
 */
int contains_redirection(char **args);

/**
 * @brief Exécute une commande logique.
 * @param command Commande logique.
 * @return 0 si la commande est vraie, -1 sinon.
 */
int execute_logic_command(const char *command);

/**
 * @brief Exécute une série de commandes séparées par des pipes.
 * @param commands Tableau de commandes.
 */
void execute_piped_commands(char **commands);

/**
 * @brief Exécute une série de commandes séparées par des opérateurs logiques.
 * @param input Commandes séparées par des opérateurs logiques.
 */
int check_background_execution(char *command);

/**
 * @brief Exécute une série de commandes séparées par des opérateurs logiques.
 * @param input Commandes séparées par des opérateurs logiques.
 */
void execute_commands_with_logic(const char *input);

#endif // EXECUTOR_H
