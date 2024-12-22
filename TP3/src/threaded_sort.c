#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define SIZE (long int) 1e8

struct arg_struct {
    int * tab;
    int size;
};

int maxVal = 0x80000000;
int minVal = 0x7FFFFFFF;

void initializeTab(int * tab)
{
  int i;
  time_t t;
  srand((unsigned) time(&t));

  for(i=0; i < SIZE; i++ )
  {
    tab[i] = rand();
  }
}

void *min(void * arguments)
{
  struct arg_struct *args = (struct arg_struct *)arguments;
  int i;
  for(i=0; i < args->size; i++ )
  {
    if(minVal > args->tab[i])
      minVal = args->tab[i];
  }
}

void *max(void * arguments)
{
  struct arg_struct *args = (struct arg_struct *)arguments;
  int i;
  for(i=0; i < args->size; i++ )
  {
    if(maxVal < args->tab[i])
      maxVal = args->tab[i];
  }
}

void createThread(int nbThread, int * tab, void  *(*f)(void *))
{
  pthread_t tabThread[nbThread];
  struct arg_struct args;

  if(nbThread <= 0)
    exit(1);

  args.size = SIZE/nbThread;
  args.tab = tab;
  int i=0, md = SIZE%nbThread;
  if ( md != 0)
  {
    pthread_create(&tabThread[i], NULL, &min,(void *) &args);
    i=1;
  }
  for(; i < nbThread; i++)
  {
    args.tab = tab+(i*args.size+md);
    pthread_create(&tabThread[i], NULL, &min,(void *) &args);
  }
  for(i = 0; i < nbThread; i++)
  {
    pthread_join(tabThread[i], NULL);
  }
}

int main(int argc, char** argv)
{
  //initialisation sur le tas (sur la pile segfault pas assez de place)
  int * tab = (int *) malloc(SIZE*sizeof(int));
  initializeTab(tab);

  struct arg_struct args;
  args.size = SIZE;
  args.tab = tab;

  struct timeval temps_avant, temps_apres;

  printf("\nTaille :  %ld \n",SIZE );

  printf("\n0 thread créé\n");
  gettimeofday (&temps_avant, NULL);
  min((void * )&args);
  gettimeofday (&temps_apres, NULL);
  printf("-- Min = %d \n---- Temps de recherche : %ld us\n", minVal,  ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);
  gettimeofday (&temps_avant, NULL);
  max((void * )&args);
  gettimeofday (&temps_apres, NULL);
  printf("-- Max = %d \n---- Temps de recherche : %ld us\n", maxVal, ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);

  printf("\n2 threads créés\n");
  gettimeofday (&temps_avant, NULL);
  createThread(2, tab, min);
  gettimeofday (&temps_apres, NULL);
  printf("-- Min = %d \n---- Temps de recherche : %ld us\n", minVal,  ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);
  gettimeofday (&temps_avant, NULL);
  createThread(2, tab, max);
  gettimeofday (&temps_apres, NULL);
  printf("-- Max = %d \n---- Temps de recherche : %ld us\n", maxVal, ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);

  printf("\n4 threads créés\n");
  gettimeofday (&temps_avant, NULL);
  createThread(4, tab, min);
  gettimeofday (&temps_apres, NULL);
  printf("-- Min = %d \n---- Temps de recherche : %ld us\n", minVal,  ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);
  gettimeofday (&temps_avant, NULL);
  createThread(4, tab, max);
  gettimeofday (&temps_apres, NULL);
  printf("-- Max = %d \n---- Temps de recherche : %ld us\n", maxVal, ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);

  printf("\n8 threads créés\n");
  gettimeofday (&temps_avant, NULL);
  createThread(8, tab, min);
  gettimeofday (&temps_apres, NULL);
  printf("-- Min = %d \n---- Temps de recherche : %ld us\n", minVal,  ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);
  gettimeofday (&temps_avant, NULL);
  createThread(8, tab, max);
  gettimeofday (&temps_apres, NULL);
  printf("-- Max = %d \n---- Temps de recherche : %ld us\n\n", maxVal, ((temps_apres.tv_sec - temps_avant.tv_sec) * 1000000 + temps_apres.tv_usec) - temps_avant.tv_usec);
 
  free(tab);
  return EXIT_SUCCESS;
}