#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <limits.h>
#include <time.h>

#define SIZE (long int)1e8

// Variables globales pour stocker le minimum et le maximum
int global_min, global_max;
pthread_mutex_t mutex;

// Structure pour passer les arguments aux threads
typedef struct
{
    int *tab;
    int start;
    int end;
} ThreadData;

// Fonction pour initialiser le tableau avec des valeurs aléatoires
void initializeTab(int *tab)
{
    time_t t;
    srand((unsigned)time(&t));
    for (long int i = 0; i < SIZE; i++)
    {
        tab[i] = rand();
    }
}

// Fonction exécutée par chaque thread pour trouver le min et le max dans son segment
void *find_min_max(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int local_min = INT_MAX;
    int local_max = INT_MIN;

    // Parcours du segment du tableau pour trouver le min et le max locaux
    for (int i = data->start; i < data->end; i++)
    {
        if (data->tab[i] < local_min)
            local_min = data->tab[i];
        if (data->tab[i] > local_max)
            local_max = data->tab[i];
    }

    // Protection des accès aux variables globales avec un mutex
    pthread_mutex_lock(&mutex);
    if (local_min < global_min)
        global_min = local_min;
    if (local_max > global_max)
        global_max = local_max;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

// Fonction pour créer les threads et répartir la charge
void createThreads(int num_threads, int *tab)
{
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int segment_size = SIZE / num_threads;
    int remaining = SIZE % num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        thread_data[i].tab = tab;
        thread_data[i].start = i * segment_size;
        thread_data[i].end = (i + 1) * segment_size;
        // Le dernier thread prend les éléments restants, s'il y en a
        if (i == num_threads - 1)
        {
            thread_data[i].end += remaining;
        }
        pthread_create(&threads[i], NULL, find_min_max, &thread_data[i]);
    }

    // Attente de la fin de tous les threads
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char **argv)
{
    // Allocation dynamique de mémoire pour le tableau
    int *tab = (int *)malloc(SIZE * sizeof(int));
    if (!tab)
    {
        perror("Allocation échouée");
        return EXIT_FAILURE;
    }

    // Initialisation du tableau et des variables globales
    initializeTab(tab);
    global_min = INT_MAX;
    global_max = INT_MIN;

    // Initialisation du mutex
    pthread_mutex_init(&mutex, NULL);

    struct timeval start_time, end_time;

    // Affichage des informations initiales
    printf("\nTaille du tableau : %ld \n", SIZE);

    // Test avec différents nombres de threads
    int thread_counts[] = {1, 2, 4, 8};
    for (int i = 0; i < sizeof(thread_counts) / sizeof(thread_counts[0]); i++)
    {
        int num_threads = thread_counts[i];
        global_min = INT_MAX;
        global_max = INT_MIN;

        printf("\n%d threads créés\n", num_threads);

        gettimeofday(&start_time, NULL);
        createThreads(num_threads, tab);
        gettimeofday(&end_time, NULL);

        long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000L +
                            (end_time.tv_usec - start_time.tv_usec);

        printf("-- Min = %d\n-- Max = %d\n-- Temps de recherche : %ld us\n",
               global_min, global_max, elapsed_time);
    }

    // Libération des ressources
    pthread_mutex_destroy(&mutex);
    free(tab);

    return EXIT_SUCCESS;
}
