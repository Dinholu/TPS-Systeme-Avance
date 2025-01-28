#ifndef HISTORY_H
#define HISTORY_H

/**
 * @brief Ajoute une commande à l'historique.
 * @param command Commande à ajouter.
 */
void add_to_history(const char *command);

/**
 * @brief Affiche l'historique des commandes.
 */
void display_history();

#endif