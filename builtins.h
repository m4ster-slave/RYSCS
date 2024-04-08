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
int shell_chmod(char** args);
int cat(char** args);
int touch(char** args);
int clear();

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> //waitpid
#include <unistd.h>  
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define BUFSIZE             1024
#define TOK_BUFSIZE         64
#define TOK_DELIM           " \t\r\n\a"
#define READ_PERMISSIONS    0666
#define EXEC_PERMISSIONS    0777

