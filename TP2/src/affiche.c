#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Utilisation: %s <mot>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Affichage sur la sortie standard
    printf("Message standard: %s\n", argv[1]);

    // Affichage sur la sortie d'erreur
    fprintf(stderr, "Message d'erreur: Ceci est une erreur sur stderr\n");

    return EXIT_SUCCESS;
}
