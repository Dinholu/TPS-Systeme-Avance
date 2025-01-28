#ifndef BUILTINS_H
#define BUILTINS_H

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

#endif // BUILTINS_H
