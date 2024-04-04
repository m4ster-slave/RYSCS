int cd(char** args);
int help(char** args);
int shell_exit(char** args);
int ls(char** args);
int pwd(char** args);
int rm(char** args);
int mv(char** args);
int cp(char** args);
int shell_mkdir(char** args);
int shell_rmdir(char** args);
int shell_chmod(char** args);
int cat(char** args);
int touch(char** args);


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

char *builtin_str[] = 
{
    "cd",
    "help",
    "exit",
    "ls",
    "pwd",
    "rm",
    "mv",
    "cp",
    "mkdir",
    "rmdir",
    "chmod",
    "cat",
    "touch",
}; 
