#include "builtins.h"
//TODO
//fix Whitespace seperating args + ""
//history
                      
//definition of function pointers
int (*builtin_func[]) (char**) = 
{
    &cd, 
    &help, 
    &shell_exit,
    &ls,
    &pwd,
    &rm,
    &mv,
    &cp,
    &shell_mkdir,
    &shell_rmdir,
    &shell_chmod,
    &cat,
    &touch,
};

//defintion of builtins to loop thru before calling exec 
char *builtin_str[] = 
{
    (char*)"cd",
    (char*)"help",
    (char*)"exit",
    (char*)"ls",
    (char*)"pwd",
    (char*)"rm",
    (char*)"mv",
    (char*)"cp",
    (char*)"mkdir",
    (char*)"rmdir",
    (char*)"chmod",
    (char*)"cat",
    (char*)"touch",
}; 

//Function reads in a charcater, if the string exceeds a buffer realocate it
char* read_line(void)
{
  int bufsize = BUFSIZE;
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
      bufsize += BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) 
      {
        fprintf(stderr, "allocation error\n");
        exit(0);
      }
    }
  }
}

char** split_line(char *line)
{
  int bufsize = TOK_BUFSIZE, pos = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if (!tokens)
  {
    fprintf(stderr, "allocation error\n");
    exit(0);
  }

  token = strtok(line, TOK_DELIM);
  while (token != NULL)
  {
    tokens[pos] = token;
    pos++;

    if (pos >= bufsize)
    {
      bufsize += TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens)
      {
        fprintf(stderr, "allocation error\n");
        exit(0);
      }
    }

    token = strtok(NULL, TOK_DELIM);

  }

  tokens[pos] = NULL;
  return tokens;
}

int launch(char** args)
{
  pid_t pid, _wpid;
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
    //error forking
    perror("lsh");
  }
  else 
  {
    //parent process
    do 
    {
    _wpid = waitpid(pid, &status, WUNTRACED);
    }while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int execute(char** args)
{
  if (args[0] == NULL)
  {
    return 1;
  }

  for (int i = 0; i < (int)(sizeof(builtin_str) / sizeof(builtin_str[0])); i++)
  {
    if(strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}

void loop(void)
{
  char *line;
  char **args;
  int status;
  char cCurrentPath[FILENAME_MAX]; 

  do 
  {
    if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
    {
      perror("dir name error");
    }

   cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

    printf("\x1b[31m\x1b[1m%s $ \x1b[0m", cCurrentPath);
    line = read_line();
    args = split_line(line);
    status = execute(args);

    free(line);
    free(args);
  }
  while(status);
}


int main(void)
{
  loop();
  return 0;
}

