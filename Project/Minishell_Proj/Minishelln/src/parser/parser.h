#ifndef PARSER_H
#define PARSER_H

/**
 * @brief Parse une commande en arguments.
 *
 * @param command Commande à parser.
 * @return Tableau de chaînes représentant les arguments.
 */
char **parse_command(const char *command);

/**
 * @brief Sépare une commande en segments séparés par des pipes.
 *
 * @param command Commande à séparer.
 * @return Tableau de chaînes représentant les segments.
 */
char **split_pipes(const char *command);

#endif // PARSER_H
