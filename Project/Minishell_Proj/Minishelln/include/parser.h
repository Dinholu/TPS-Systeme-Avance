#include "typedef.h"
#ifndef PARSER_H
#define PARSER_H

/**
 * @brief Parse une chaîne de caractères en une liste de commandes.
 * @param input Chaîne de caractères à parser.
 * @param commands Tableau de commandes.
 * @return Nombre de commandes.
 */
int parse_control_operators(const char *input, CommandNode commands[]);

/**
 * @brief Parse une chaîne de caractères en une liste d'arguments.
 * @param command Chaîne de caractères à parser.
 * @return Tableau d'arguments.
 */
char **parse_command(const char *command);

/**
 * @brief Découpe une chaîne de caractères en fonction des pipes.
 * @param command Chaîne de caractères à découper.
 * @return Tableau de commandes.
 */
char **split_pipes(const char *command);

/**
 * @brief Vérifie si une commande est en arrière-plan.
 * @param args Tableau d'arguments de la commande.
 * @return 1 si la commande est en arrière-plan, 0 sinon.
 */
int is_background_command(char **args);

#endif // PARSER_H
