#ifndef EXECUTOR_H
#define EXECUTOR_H

/**
 * @brief Ouvre un fichier pour la redirection de la sortie standard.
 * @param filename Nom du fichier.
 * @param append 1 pour ajouter à la fin du fichier, 0 pour écraser le fichier.
 * @return Descripteur de fichier du fichier ouvert.
 */
int open_output_redirection(const char *filename, int append);

/**
 * @brief Ouvre un fichier pour la redirection de l'entrée standard.
 * @param filename Nom du fichier.
 * @return Descripteur de fichier du fichier ouvert.
 */
int open_input_redirection(const char *filename);

/**
 * @brief Gère les redirections d'entrée et de sortie.
 * @param args Tableau d'arguments de la commande.
 * @param fd_in Descripteur de fichier de l'entrée standard.
 * @param fd_out Descripteur de fichier de la sortie standard.
 */
void handle_redirections(char **args, int *fd_in, int *fd_out);

/**
 * @brief Exécute une commande.
 * @param args Tableau d'arguments de la commande.
 */
void execute_command(char **args);

/**
 * @brief Vérifie si une commande contient des opérateurs de redirection.
 * @param args Tableau d'arguments de la commande.
 * @return 1 si la commande contient des opérateurs de redirection, 0 sinon.
 */
int contains_redirection(char **args);

/**
 * @brief Exécute une commande logique.
 * @param command Commande à exécuter.
 * @return 0 si la commande a été exécutée, -1 sinon.
 */
int execute_logic_command(const char *command);

/**
 * @brief Exécute une commande avec des opérateurs de contrôle (pipe, redirection, etc.).
 * @param commands Tableau de commandes et opérateurs de contrôle.
 * @return 0 si la commande a été exécutée, -1 sinon.
 */
void execute_piped_commands(char **commands);

/**
 * @brief Vérifie si une commande nécessite une exécution en arrière-plan.
 * @param command Commande à vérifier.
 * @return 1 si la commande nécessite une exécution en arrière-plan, 0 sinon.
 */
int check_background_execution(char *command);

/**
 * @brief Exécute des commandes avec des opérateurs logiques (&&, ||).
 * @param input Commandes à exécuter.
 * @return 0 si la commande a été exécutée, -1 sinon.
 */
void execute_commands_with_logic(const char *input);

#endif // EXECUTOR_H
