#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* ============================
   Exercice 1: "Fork Yourself"
   ============================ */
void fork_yourself()
{
    int status;
    pid_t f = fork();

    if (f < 0)
    {
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    }

    if (f == 0)
    {
        // Processus fils
        printf("Processus fils -> PID: %d, PPID: %d\n", getpid(), getppid());
        exit(getpid() % 10); // Retourner le dernier chiffre du PID
    }
    else
    {
        // Processus père
        printf("Processus père -> PID: %d, PID fils: %d\n", getpid(), f);
        if (wait(&status) == -1)
        {
            perror("Erreur wait");
            exit(EXIT_FAILURE);
        }
        printf("Processus père -> Code retour fils: %d\n", WEXITSTATUS(status));
    }
}

/* ============================
   Exercice 2: Redirection de flux
   ============================ */

// Partie 1: Affichage simple
void affiche_message(char *message)
{
    if (message == NULL)
    {
        fprintf(stderr, "Erreur: Argument manquant\n");
        return;
    }
    printf("Message standard: %s\n", message);
    fprintf(stderr, "Message d'erreur: Ceci est une erreur\n");
}

// Partie 2: Création d'un fils et redirection vers fichier
void redirection_flux(char *prog)
{
    if (prog == NULL)
    {
        fprintf(stderr, "Erreur: Spécifiez un programme à exécuter\n");
        return;
    }

    int status;
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Processus fils
        printf("Fils -> PID: %d\n", getpid());

        // Étape 3 : Fermer STDOUT (descripteur 1)
        close(STDOUT_FILENO);

        // Ouvrir le fichier en écriture
        int fd = open("/tmp/proc-exercise", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            fprintf(stderr, "Erreur ouverture fichier (%s)\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Étape 4 : Afficher le numéro du descripteur ouvert
        printf("Descripteur fichier ouvert: %d\n", fd);

        // Rediriger STDOUT vers le fichier ouvert
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("Erreur lors de dup2 stdout");
            exit(EXIT_FAILURE);
        }

        // Étape 5 : Exécuter le programme `affiche`
        execlp(prog, prog, "Redirigé", NULL);

        // En cas d'échec
        perror("Erreur exec");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Processus père
        printf("Père -> PID: %d\n", getpid());

        // Attendre la fin du fils
        if (wait(&status) == -1)
        {
            perror("Erreur wait");
            exit(EXIT_FAILURE);
        }

        // Message final
        printf("Père -> That's All Folks!\n");
    }
}

/* ============================
   Exercice 3: Redirection via pipe
   ============================ */
void pipe_redirection()
{
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1)
    {
        fprintf(stderr, "Erreur création pipe (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((pid1 = fork()) < 0)
    {
        perror("Erreur lors du fork pour ps");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0)
    {
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            perror("Erreur dup2 ps");
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);
        execlp("ps", "ps", "aux", NULL);
        perror("Erreur exec ps");
        exit(EXIT_FAILURE);
    }

    if ((pid2 = fork()) < 0)
    {
        perror("Erreur lors du fork pour grep");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0)
    {
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            perror("Erreur dup2 grep");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);
        execlp("grep", "grep", "^root ", NULL);
        perror("Erreur exec grep");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    int status1, status2;
    if (waitpid(pid1, &status1, 0) == -1 || waitpid(pid2, &status2, 0) == -1)
    {
        perror("Erreur waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status2) && WEXITSTATUS(status2) == 0)
    {
        if (write(STDOUT_FILENO, "root est connecté\n", 18) == -1)
        {
            perror("Erreur lors de l'écriture");
        }
    }
}

/* ============================
   Programme principal
   ============================ */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <numéro d'exercice> [arguments supplémentaires]\n", argv[0]);
        fprintf(stderr, "1 -> Fork Yourself\n");
        fprintf(stderr, "2 -> Affiche Message (nécessite un argument)\n");
        fprintf(stderr, "3 -> Redirection Flux (nécessite un programme à exécuter)\n");
        fprintf(stderr, "4 -> Pipe Redirection\n");
        return EXIT_FAILURE;
    }

    int choix = atoi(argv[1]);

    switch (choix)
    {
    case 1:
        fork_yourself();
        break;
    case 2:
        if (argc < 3)
        {
            fprintf(stderr, "Erreur: Donnez un mot à afficher\n");
            return EXIT_FAILURE;
        }
        affiche_message(argv[2]);
        break;
    case 3:
        if (argc < 3)
        {
            fprintf(stderr, "Erreur: Donnez un programme à exécuter\n");
            return EXIT_FAILURE;
        }
        redirection_flux(argv[2]);
        break;
    case 4:
        pipe_redirection();
        break;
    default:
        fprintf(stderr, "Erreur: Choix invalide\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
