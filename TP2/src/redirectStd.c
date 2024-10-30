#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/types.h>

#define STDOUT 1
#define STDERR 2

int main(int argc, char** argv){
    
	if(argc <= 1 )
    { 
        dprintf(STDERR, "Too few arguments. One minimum is required.\n");
       	exit(EXIT_FAILURE);
    }
    else
    {
        dprintf(STDOUT, "Le premiere argument saisi est : %s\n", argv[1]);   
    }

	int f;
	int status;
	f = fork();
	if(f==0)
	{
		printf("PID du fils : %d\n", getpid());
	    close(STDERR);
	    static char template[] = "/tmp/proc-exerciceXXXXXX";
	    char fname[25];
	    strcpy(fname, template);
	    int fd = mkstemp(fname);
	    printf("Le file descriptor du fils %s est : %d \n", fname, fd);
	    dprintf(STDERR, "Message quelconque !\n");
	    execlp (argv[1], argv[1], (void*)0);
	    perror("Erreur : ");
	    exit(1);
	}
	else
	{
		int retour = 0;
    	printf("PID du père : %d\n", getpid());
    	wait(&retour);
    	dprintf(STDERR, "Message quelconque !\n");
    	dprintf(STDOUT, "That's All Folks !\n");
	}

	pid_t getpid(void);
	pid_t getppid(void);
    
    return EXIT_SUCCESS;
}