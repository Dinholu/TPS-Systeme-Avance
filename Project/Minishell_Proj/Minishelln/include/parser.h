#include "typedef.h"
#ifndef PARSER_H
#define PARSER_H

/**
 * @brief Parse une chaîne de caractères pour obtenir les commandes et les
 * opérateurs de contrôle.
 *
 * @param input Chaîne de caractères à parser.
 * @param commands Tableau de commandes et opérateurs de contrôle.
 * @return Nombre de commandes et opérateurs de contrôle.
 */
int parse_control_operators(const char *input, CommandNode commands[]);

/**
 * @brief Parse une commande pour obtenir les arguments.
 *
 * @param command Commande à parser.
 * @return Tableau d'arguments de la commande.
 */
char **parse_command(const char *command);

/**
 * @brief Sépare une commande en segments séparés par des pipes.
 *
 * @param command Commande à séparer.
 * @return Tableau de chaînes représentant les segments.
 */
char **split_pipes(const char *command);

/**
 * @brief Vérifie si une commande contient des opérateurs de redirection.
 *
 * @param args Tableau d'arguments de la commande.
 * @return 1 si la commande contient des opérateurs de redirection, 0 sinon.
 */
int is_background_command(char **args);

#endif // PARSER_H
