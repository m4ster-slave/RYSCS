int cd(char** args);
int help();
int shell_exit();
int ls(char** args);
int pwd();
int rm(char** args);
int mv(char** args);
int cp(char** args);
int shell_mkdir(char** args);
int shell_rmdir(char** args);
int cat(char** args);
int touch(char** args);
#ifdef __linux__ 
int shell_chmod(char** args);
int clear();
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/sendfile.h>
    #include <sys/stat.h>
    #include <grp.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <pwd.h>
#endif

#define BUFSIZE             1024
#define TOK_BUFSIZE         64
#define TOK_DELIM           " \t\r\n\a"
#define READ_PERMISSIONS    0666
#define EXEC_PERMISSIONS    0777

