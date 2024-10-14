/**
 * \file skeleton.c
 * \brief Basic parsing options skeleton.
 * \author Pierre L. <pierre1.leroy@orange.com>
 * \version 0.1
 * \date 10 septembre 2016
 *
 * Basic parsing options skeleton exemple c file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <fcntl.h>


#define STDOUT 1
#define STDERR 2

#define MAX_PATH_LENGTH 4096


#define USAGE_SYNTAX "[OPTIONS] -i INPUT -o OUTPUT -p "
#define USAGE_PARAMS "OPTIONS:\n\
  -i, --input  INPUT_FILE  : input file\n\
  -o, --output OUTPUT_FILE : output file\n\
  -p, --options \n\
***\n\
  -p c : Copying a file\n\
  -p r : Reverse a file\n\
  -p l : ls like\n\
***\n\
  -v, --verbose : enable *verbose* mode\n\
  -h, --help    : display this help\n\
"

/**
 * Procedure which displays binary usage
 * by printing on stdout all available options
 *
 * \return void
 */
void print_usage(char* bin_name)
{
  dprintf(1, "USAGE: %s %s\n\n%s\n", bin_name, USAGE_SYNTAX, USAGE_PARAMS);
}


/**
 * Procedure checks if variable must be free
 * (check: ptr != NULL)
 *
 * \param void* to_free pointer to an allocated mem
 * \see man 3 free
 * \return void
 */
void free_if_needed(void* to_free)
{
  if (to_free != NULL) free(to_free);  
}


/**
 *
 * \see man 3 strndup
 * \see man 3 perror
 * \return
 */
char* dup_optarg_str()
{
  char* str = NULL;

  if (optarg != NULL)
  {
    str = strndup(optarg, MAX_PATH_LENGTH);
    
    // Checking if ERRNO is set
    if (str == NULL) 
      perror(strerror(errno));
  }

  return str;
}


/**
 * Binary options declaration
 * (must end with {0,0,0,0})
 *
 * \see man 3 getopt_long or getopt
 * \see struct option definition
 */
static struct option binary_opts[] = 
{
  { "help",    no_argument,       0, 'h' },
  { "verbose", no_argument,       0, 'v' },
  { "input",   required_argument, 0, 'i' },
  { "output",  required_argument, 0, 'o' },
  { "option",  required_argument, 0, 'p' },
  { 0,         0,                 0,  0  } 
};

/**
 * Binary options string
 * (linked to optionn declaration)
 *
 * \see man 3 getopt_long or getopt
 */ 
const char* binary_optstr = "hvi:o:p:";



/**
 * Binary main loop
 *
 * \return 1 if it exit successfully 
 */
int main(int argc, char** argv)
{
  /**
   * Binary variables
   * (could be defined in a structure)
   */
  short int is_verbose_mode = 0;
  char* bin_option_param = NULL;
  char* bin_input_param = NULL;
  char* bin_output_param = NULL;

  // Parsing options
  int opt = -1;
  int opt_idx = -1;

  while ((opt = getopt_long(argc, argv, binary_optstr, binary_opts, &opt_idx)) != -1)
  {
    switch (opt)
    {
      case 'i':
        //input param
        if (optarg)
        {
          bin_input_param = dup_optarg_str();         
        }
        break;
      case 'o':
        //output param
        if (optarg)
        {
          bin_output_param = dup_optarg_str();
        }
        break;
      case 'p':
        //option param
        if (optarg)
        {
          bin_option_param = dup_optarg_str();
        }
        break;
      case 'v':
        //verbose mode
        is_verbose_mode = 1;
        break;
      case 'h':
        print_usage(argv[0]);

        free_if_needed(bin_input_param);
        free_if_needed(bin_output_param);
 
        exit(EXIT_SUCCESS);
      default :
        break;
    }
  } 

  /**
   * Checking binary requirements
   * (could defined in a separate function)
   */
  if (bin_input_param == NULL || bin_output_param == NULL)
  {
    dprintf(STDERR, "Bad usage! See HELP [--help|-h]\n");

    // Freeing allocated data
    free_if_needed(bin_input_param);
    free_if_needed(bin_output_param);
    // Exiting with a failure ERROR CODE (== 1)
    exit(EXIT_FAILURE);
  }


  // Printing params
  dprintf(1, "** PARAMS **\n%-8s: %s\n%-8s: %s\n%-8s: %s\n%-8s: %d\n", 
          "input",   bin_input_param, 
          "output",  bin_output_param,
          "option",  bin_option_param,
          "verbose", is_verbose_mode);

  // Business logic must be implemented at this point
  int temp = *bin_option_param;
  char tempo = (char)temp;
  int result = 0;

  if(tempo == 'c'){
    result = copie(bin_input_param, bin_output_param);
  }
  else if(tempo == 'r')
  {
    result = reverse(bin_input_param);
  }
  else if(tempo == 'l')
  {
    result = lsLike(bin_input_param);
  }

  if(result != 1){

      exit(EXIT_FAILURE);

  }

  // Freeing allocated data
  free_if_needed(bin_input_param);
  free_if_needed(bin_output_param);


  return EXIT_SUCCESS;
}

int copie(char* bin_input_param, char* bin_output_param){
  
  printf("Copying %s in %s\n",bin_input_param, bin_output_param);
  
  FILE* fichier = NULL;
  FILE* copie = NULL;
  char buffer[512]; 
  int NbLus; 

  // ouvrir le fichier source et le fichier de destination
  fichier = fopen(bin_input_param, "r+");
  copie = fopen(bin_output_param, "a+");
  
  if(fichier != NULL){
      
          while ((NbLus = fread(buffer, 1, 512, fichier)) != 0) 
      fwrite(buffer, 1, NbLus, copie); 
      
  }
  
  fclose(fichier);
  fclose(copie);
  
  return 1;
  
}

int reverse(char* bin_input_param){

  // regarde si fichier est ouvert avec errno
  int input = open(bin_input_param, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
  int i=0;

  if (input >= 0 ) {

    // recupere le nb de caractère du fichier
    off_t input_size = lseek(input, 0, SEEK_END);
    lseek(input, 0, SEEK_SET);
    char* buffer = (char*)malloc(input_size);
    read(input, buffer, input_size);

    for(i=input_size; i>=0; i--){
      dprintf(1,"%c", buffer[i]);

    }
    dprintf(1,"\n");  
    
  }
  close(input);

}

int lsLike(char* bin_input_param){

	struct stat s;
	int err = stat(bin_input_param, &s);
	if(-1 == err) {
		if(ENOENT == errno) {
			dprintf(STDERR, "Error : The path does not match to a dir nor file\n");
			perror("stat");
			// Exiting with a failure ERROR CODE (== 1)
			exit(EXIT_FAILURE);
		} 
		else {
			perror("stat");
			exit(1);
		}
	} 
	else {
		if(S_ISDIR(s.st_mode)) {
			DIR* dir = opendir(bin_input_param) ;
			struct dirent *dirStruct ;
			dprintf(STDOUT,"Début dossier\n");
			while(dirStruct=readdir(dir))
			{
				stat((dirStruct->d_name), &s);
				printf("%-30s",(dirStruct->d_name));
				printf("  ");         
				printf( (S_ISDIR(s.st_mode)) ? "d" : "-");
				printf( (s.st_mode & S_IRUSR) ? "r" : "-");
				printf( (s.st_mode & S_IWUSR) ? "w" : "-");
				printf( (s.st_mode & S_IXUSR) ? "x" : "-");
				printf( (s.st_mode & S_IRGRP) ? "r" : "-");
				printf( (s.st_mode & S_IWGRP) ? "w" : "-");
				printf( (s.st_mode & S_IXGRP) ? "x" : "-");
				printf( (s.st_mode & S_IROTH) ? "r" : "-");
				printf( (s.st_mode & S_IWOTH) ? "w" : "-");
				printf( (s.st_mode & S_IXOTH) ? "x" : "-");
				printf("  ");         
				struct passwd *pwd;
				pwd = getpwuid(s.st_uid);
				printf("%s",getpwuid(pwd->pw_name));
				printf("  ");         
				printf("%d",s.st_uid);
				printf(" : ");         
				printf("%d",s.st_gid);
				printf("  ");         

				time_t rawtime;
				struct tm * timeinfo;
				char buffer[80];
				time ( &s.st_mtime );
				timeinfo = localtime ( &s.st_mtime );
				strftime(buffer,80,"%x - %I:%M%p", timeinfo);
				printf("|%s| ", buffer );



				printf("%d bytes\n",s.st_size);
			}
			dprintf(STDOUT,"Fin dossier\n");

		} 
		else {
			dprintf(STDERR, "Error : The path does not match to a dir but a file\n");
			perror("stat");
			// Exiting with a failure ERROR CODE (== 1)
			exit(EXIT_FAILURE);
		}
	}

	return 1;
}