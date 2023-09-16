#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // fork exec pid_t 
#include <sys/wait.h> //waitpid
#define LSH_RL_BUFSIZE 1024

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

//TODO
//fix Whitespace seperating args + ""
//piping 
//other standart builtins 
//clear with ctrl + l 
//display current file path in name 
//make emulator
//history

int lsh_cd(char** args);
int lsh_help(char** args);
int lsh_exit(char** args);

char *builint_str[] = 
{
  "cd",
  "help",
  "exit"
};  

int (*builtin_func[]) (char**) = 
{
  &lsh_cd, 
  &lsh_help, 
  &lsh_exit
};

int lsh_num_builtins()
{
  return sizeof(builint_str) / sizeof(char*);
}


//builint function definition
int lsh_cd(char** args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char** args)
{
  int i;
  printf("RPG-Shell \n");
  printf("Type programs and args and hit enter.\n");
  printf("The following are builin:\n");

  for (i = 0; i < lsh_num_builtins(); i++)
  {
    printf(" %s\n", builint_str[i]);
  }

  printf("Use man on other programs.\n");
  return 1;
}

int lsh_exit(char** args)
{
  return 0;
}

//Function reads in a charcater, if the strin exceeds a buffer realocate it
char* lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int pos = 0;
  char* buffer = malloc(sizeof(char)*bufsize);
  int c;

  if(!buffer)
  {
    fprintf(stderr, "allocation error\n");
    exit(0);
  }

  while(1)
  {
    //get a character
    c = getchar();

    //If EOF or \n --> end of string
    if(c == EOF || c == '\n')
    {
      buffer[pos] = '\0';
      return buffer;
    }
    else
    {
      buffer[pos] = c;
    }
    pos++;

    //reallocate 
    if (pos >= bufsize)
    {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) 
      {
        fprintf(stderr, "allocation error\n");
        exit(0);
      }
    }
  }
}

char** lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, pos = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if (!tokens)
  {
    fprintf(stderr, "allocation error\n");
    exit(0);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL)
  {
    tokens[pos] = token;
    pos++;

    if (pos >= bufsize)
    {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens)
      {
        fprintf(stderr, "allocation error\n");
        exit(0);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);

  }

  tokens[pos] = NULL;
  return tokens;
}

int lsh_launch(char** args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0)
  {
    //child process
    if (execvp(args[0], args) == -1)
    {
      perror("lsh");
    }
    exit(0);
  }
  else if(pid < 0)
  {
    //something went wrong forking
    perror("lsh");
  }
  else 
  {
    //parent process
    do 
    {
    wpid = waitpid(pid, &status, WUNTRACED);
    }while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int lsh_execute(char** args)
{
  int i;

  if (args[0] == NULL)
  {
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++)
  {
    if(strcmp(args[0], builint_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do 
  {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  }
  while(status);
}

int main(int argc, char *argv[])
{
  lsh_loop();
  return 0;
}
