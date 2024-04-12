#include "builtins.h"
//TODO
//fix Whitespace seperating args + ""
//(history)

//Function reads in a charcater, if the string exceeds a buffer realocate it
char* read_line(void)
{
    int bufsize = BUFSIZE;
    int pos = 0;
    char* buffer = malloc(sizeof(char)*bufsize);
    int c;

    if(!buffer)
    {
        fprintf(stderr, "allocation error when trying to read the input\n");
        exit(0);
    }

    while(1)
    {
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
                fprintf(stderr, "allocation error when reallocating\n");
                exit(0);
            }
        }
    }
}

char** split_line(char *line)
{
    int bufsize = TOK_BUFSIZE; 
    int pos = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    if (!tokens)
    {
        fprintf(stderr, "allocation error when trying to spilt the arguments\n");
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
                fprintf(stderr, "reallocation error\n");
                exit(0);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[pos] = NULL;
    return tokens;
}

int launch(char** args) {
#ifdef _WIN32
    // Windows specific code
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (!CreateProcess(
            NULL, 
            args[0], 
            NULL, 
            NULL, 
            FALSE, 
            0, 
            NULL, 
            NULL, 
            &si, 
            &pi
    )) 
    {
        fprintf(stderr, "CreateProcess failed (%lu)\n", GetLastError());
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    int status = 0;
    //fork the process parent waits while child executes the given program
    pid_t pid = fork();

    if (pid == 0) 
    {
        // Child process
        if (execvp(args[0], args) == -1) 
        {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) 
    {
        perror("shell");
    } 
    else 
    {
        // Parent process
        do 
        {
            waitpid(pid, &status, WUNTRACED);
        } 
        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
#endif

    return 1;
}

int execute(char** args)
{
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
        &cat,
        &touch,
#ifdef __linux__ 
        &shell_chmod,
        &clear,
#endif
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
        (char*)"cat",
        (char*)"touch",
#ifdef __linux__ 
        (char*)"chmod",
        (char*)"clear",
#endif
    }; 

    if (args[0] == NULL)
    {
        return 1;
    }

    //loop thru the buitlin_str to see if we call a builtin programc and call it via its function pointer
    for (int i = 0; i < (int)(sizeof(builtin_str) / sizeof(builtin_str[0])); i++)
    {
        if(strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    //else exec the program 
    return launch(args);
}

void loop(void)
{
    char* line;
    char** args;
    int status;
    char cCurrentPath[FILENAME_MAX]; 

    do 
    {
        if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
        {
            perror("dir name error");
        }

        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; //not really required 

#ifdef _WIN32
        printf("%s $ ", cCurrentPath);
#else
        char *username = getlogin();
        if (username == NULL) {
            perror("getlogin");
            exit(EXIT_FAILURE);
        }

        struct utsname unameData;
        if (uname(&unameData) == -1) {
            perror("uname");
            exit(EXIT_FAILURE);
        }
        char *hostname = unameData.nodename;

        printf("\x1b[34;1;3m%s@%s \x1b[0m\x1b[31m\x1b[1m%s $ \x1b[0m", username, hostname, cCurrentPath);
#endif
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
    return EXIT_SUCCESS;
}
