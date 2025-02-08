#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <limits.h>

#define SIZE (long int)1e8

typedef struct
{
  int *tab;
  int start;
  int size;
} ThreadData;

int maxVal = INT_MIN;
int minVal = INT_MAX;
pthread_mutex_t min_mutex, max_mutex;

void initializeTab(int *tab)
{
  time_t t;
  srand((unsigned)time(&t));
  for (long int i = 0; i < SIZE; i++)
  {
    tab[i] = rand();
  }
}

void *find_min(void *arg)
{
  ThreadData *data = (ThreadData *)arg;
  int local_min = INT_MAX;

  for (int i = 0; i < data->size; i++)
  {
    if (data->tab[i] < local_min)
      local_min = data->tab[i];
  }

  pthread_mutex_lock(&min_mutex);
  if (local_min < minVal)
    minVal = local_min;
  pthread_mutex_unlock(&min_mutex);
  return NULL;
}

void *find_max(void *arg)
{
  ThreadData *data = (ThreadData *)arg;
  int local_max = INT_MIN;

  for (int i = 0; i < data->size; i++)
  {
    if (data->tab[i] > local_max)
      local_max = data->tab[i];
  }

  pthread_mutex_lock(&max_mutex);
  if (local_max > maxVal)
    maxVal = local_max;
  pthread_mutex_unlock(&max_mutex);
  return NULL;
}

void createThreads(int num_threads, int *tab, void *(*func)(void *))
{
  pthread_t threads[num_threads];
  ThreadData thread_data[num_threads];
  int segment_size = SIZE / num_threads;

  for (int i = 0; i < num_threads; i++)
  {
    thread_data[i].tab = tab + i * segment_size;
    thread_data[i].size = (i == num_threads - 1) ? (SIZE - i * segment_size) : segment_size;

    if (pthread_create(&threads[i], NULL, func, &thread_data[i]) != 0)
    {
      perror("Erreur lors de la création du thread");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], NULL);
  }
}

int main()
{
  int *tab;
  if (posix_memalign((void **)&tab, 64, SIZE * sizeof(int)) != 0)
  {
    perror("Échec de l'allocation mémoire");
    return EXIT_FAILURE;
  }

  initializeTab(tab);
  pthread_mutex_init(&min_mutex, NULL);
  pthread_mutex_init(&max_mutex, NULL);

  struct timeval start_time, end_time;
  printf("\nTaille du tableau : %ld\n", SIZE);

  int thread_counts[] = {1, 2, 4, 8};
  for (int i = 0; i < sizeof(thread_counts) / sizeof(thread_counts[0]); i++)
  {
    int num_threads = thread_counts[i];
    minVal = INT_MAX;
    maxVal = INT_MIN;

    printf("\n%d threads créés pour min\n", num_threads);
    gettimeofday(&start_time, NULL);
    createThreads(num_threads, tab, find_min);
    gettimeofday(&end_time, NULL);
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000L +
                        (end_time.tv_usec - start_time.tv_usec);
    printf("-- Min = %d\n-- Temps de recherche : %ld us\n", minVal, elapsed_time);

    printf("\n%d threads créés pour max\n", num_threads);
    gettimeofday(&start_time, NULL);
    createThreads(num_threads, tab, find_max);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000L +
                   (end_time.tv_usec - start_time.tv_usec);
    printf("-- Max = %d\n-- Temps de recherche : %ld us\n", maxVal, elapsed_time);
  }

  pthread_mutex_destroy(&min_mutex);
  pthread_mutex_destroy(&max_mutex);
  free(tab);
  return EXIT_SUCCESS;
}
