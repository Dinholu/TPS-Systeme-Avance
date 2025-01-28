#ifndef BUILTINS_H
#define BUILTINS_H

/**
 * Change le répertoire courant.
 * @param args Tableau d'arguments de la commande.
 * @return 0 si la commande a été exécutée, 1 sinon.
 */
int builtin_cd(char **args);

/**
 * Affiche le répertoire courant.
 * @return 0 si la commande a été exécutée, 1 sinon.
 */
int builtin_pwd();

/**
 * Quitte le shell.
 * @return 0 si la commande a été exécutée, 1 sinon.
 */
int builtin_exit();

/**
 * Affiche les arguments passés en paramètre.
 * @param args Tableau d'arguments de la commande.
 * @return 0 si la commande a été exécutée, 1 sinon.
 */
int builtin_echo(char **args);

/**
 * Recherche une variable d'environnement.
 * @param name Nom de la variable à rechercher.
 * @return Indice de la variable dans le tableau d'environnement, -1 si non
 * trouvée.
 */
int find_env_var(const char *name);

/**
 * Ajoute ou modifie une variable d'environnement.
 * @param name Nom de la variable.
 * @param value Valeur de la variable.
 */
void set_env_var(const char *name, const char *value);

/**
 * Récupère la valeur d'une variable d'environnement.
 * @param name Nom de la variable à récupérer.
 * @return Valeur de la variable, NULL si non trouvée.
 */
char *get_env_var(const char *name);

/**
 * Supprime une variable d'environnement.
 * @param name Nom de la variable à supprimer.
 */
void unset_env_var(const char *name);

/**
 * Remplace les variables d'environnement dans les arguments.
 * @param args Tableau d'arguments de la commande.
 */
void expand_variables(char **args);

/**
 * Affiche toutes les variables d'environnement.
 * @return 0 si la commande a été exécutée, 1 sinon.
 */
int builtin_env();

/**
 * Recherche un alias.
 * @param alias Alias à rechercher.
 * @return Commande associée à l'alias, NULL si non trouvé.
 */
void set_alias(const char *alias, const char *command);

/**
 * Ajoute ou modifie un alias.
 * @param alias Alias à ajouter ou modifier.
 * @param command Commande associée à l'alias.
 */
char *get_alias(const char *alias);

/**
 * Supprime un alias.
 * @param alias Alias à supprimer.
 */
void unset_alias(const char *alias);

/**
 * Affiche tous les alias.
 * @return 0 si la commande a été exécutée, 1 sinon.
 */
int builtin_alias();

#endif // BUILTINS_H
