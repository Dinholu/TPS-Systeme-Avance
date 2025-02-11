#ifndef TYPEDEF_H
#define TYPEDEF_H

#define MAX_ARGS 128
#define MAX_ENV_VARS 128
#define MAX_ALIASES 64
#define MAX_PIPE_SEGMENTS 16
#define MAX_COMMANDS 3

/**
 * @brief Enumération des types de commandes logiques.
 */
typedef enum { CMD_NONE, CMD_AND, CMD_OR } CommandType;

/**
 * @brief Structure représentant un noeud de commande.
 */
typedef struct {
  char *command;
  CommandType type;
} CommandNode;

/**
 * @brief Structure représentant une variable d'environnement.
 */
typedef struct {
  char *name;
  char *value;
} EnvVar;

/**
 * @brief Structure représentant un alias.
 */
typedef struct {
  char *alias;
  char *command;
} Alias;

#endif // TYPEDEF_H
