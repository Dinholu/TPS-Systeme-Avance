#ifndef BUILTINS_H
#define BUILTINS_H

/**
 * @brief Change le répertoire courant.
 * @param args Liste des arguments. args[1] doit être le répertoire cible.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int builtin_cd(char **args);

/**
 * @brief Affiche le répertoire courant.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int builtin_pwd();

/**
 * @brief Affiche l'argument passé en paramètre.
 * @param args Liste des arguments. args[1] doit être le texte à afficher.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int builtin_echo(char **args);

/**
 * @brief Sortir du shell.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int builtin_exit();

/**
 * @brief Affiche la liste des variables d'environnement.
 * @param name Nom de la variable d'environnement à chercher.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int find_env_var(const char *name);

/**
 * @brief Change la valeur d'une variable d'environnement.
 * @param name Nom de la variable d'environnement.
 * @param value Valeur de la variable d'environnement.
 */
void set_env_var(const char *name, const char *value);

/**
 * @brief Récupère la valeur d'une variable d'environnement.
 * @param name Nom de la variable d'environnement.
 * @return Valeur de la variable d'environnement.
 */
char *get_env_var(const char *name);

/**
 * @brief Supprime une variable d'environnement.
 * @param name Nom de la variable d'environnement.
 */
void unset_env_var(const char *name);

/**
 * @brief Attribution de la variable d'environnement.
 * @param args Liste des arguments.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
void expand_env_variables(char **args);

/**
 * @brief Affiche la liste des variables d'environnement.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int builtin_env();

/**
 * @brief Change la valeur d'un alias.
 * @param alias Nom de l'alias.
 * @param command Commande associée à l'alias.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
void set_alias(const char *alias, const char *command);

/**
 * @brief Récupère la valeur d'un alias.
 * @param alias Nom de l'alias.
 * @return Commande associée à l'alias.
 */
char *get_alias(const char *alias);

/**
 * @brief Supprime un alias.
 * @param alias Nom de l'alias.
 */
void unset_alias(const char *alias);

/**
 * @brief Affiche la liste des alias.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
int builtin_alias();

/**
 * @brief Attribution de l'alias.
 * @param args Liste des arguments.
 * @return 0 si réussi, -1 si une erreur est survenue.
 */
void expand_alias(char **args);

/**
 * @brief Vérifie si la commande est un built-in.
 * @param args Liste des arguments.
 * @return 0 si la commande est un built-in, -1 sinon.
 */
int is_builtin(char **args);

#endif // BUILTINS_H
