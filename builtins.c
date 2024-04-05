#include "builtins.h"

int cd(char** args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "expected argument to \"cd\"\n");
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

int help()
{
    printf("Type programs and args then hit enter.\n");
    printf("The following functions are builtin:\n");
    printf("\tcd \n\thelp \n\tshell_exit \n\tls \n\tpwd \n\trm \n\tmv \n\tcp \n\tshell_mir \n\tshell_rmdir \n\tshell_chmod \n\tcat \n\ttouch");
    printf("\nenjoy using the shell :)\n");
    return 1;
}

int shell_exit()
{
  return 0;
}

int ls(char** args)
{
    int ls_long = 0, show_hidden = 0;
    char cPath[FILENAME_MAX]; 
    struct stat stats;

    if (args[1] == NULL)
    {
        if (getcwd(cPath, sizeof(cPath)) == NULL)
        {
            perror("ls error");
        }
    }
    else 
    {
        for (char* c = *++args; c; c=*++args) 
        {
            if (strcmp("-l", c) == 0)
            {
                ls_long = 1;
            }
            else if (strcmp("-a", c) == 0)
            {
                show_hidden = 1;
            } 
            else if (strcmp("-la", c) == 0 || strcmp("-al", c) == 0)
            {
                show_hidden = 1;
                ls_long = 1;
            }
            else if (stat(c ,&stats) == 0) 
            {
                strcpy(cPath, c); 
            }
            else 
            {
                fprintf(stderr, "ls error: invalid arguments");
            }
        }
    }

	struct dirent *d;
	DIR *dh = opendir(cPath);
    if (dh == NULL) 
    {
        perror("error opening directory");
        return 1;
    }

	//while the next entry is not readable print directory files
	while ((d = readdir(dh)) != NULL)
	{
        //if the user didnt enable hidden skip files that begin with '.'
        if (d->d_name[0] == '.' && !show_hidden) continue;

        //if the user enabled long output print file info
        if (ls_long) 
        {
            if (stat(d->d_name, &stats) == -1) {
                perror("stat error");
                return 1;
            }


            //permissions
            mode_t perms = stats.st_mode;
            printf( (perms & S_IRUSR) ? "r" : "-");
            printf( (perms & S_IWUSR) ? "w" : "-");
            printf( (perms & S_IXUSR) ? "x" : "-");
            printf( (perms & S_IRGRP) ? "r" : "-");
            printf( (perms & S_IWGRP) ? "w" : "-");
            printf( (perms & S_IXGRP) ? "x" : "-");
            printf( (perms & S_IROTH) ? "r" : "-");
            printf( (perms & S_IWOTH) ? "w" : "-");
            printf( (perms & S_IXOTH) ? "x" : "-");

            //size
            int bytes = stats.st_size;

            if (bytes < 1000) 
                printf(" %4dB", bytes);
            else if (bytes < 1000000) 
                printf(" %4dK", bytes / 1000);
            else if (bytes < 1000000000) 
                printf(" %4dM", bytes / 1000000);
            else 
                printf(" %4dG", bytes / 1000000000);


            //file creation time in seconds then convert to date and time format
            struct tm dt;
            time_t current_time;
            char* month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
            dt = *(gmtime(&stats.st_ctime));
            time(&current_time);
            struct tm* curr_time_info = localtime(&current_time);

            printf(" %2d %s", dt.tm_mday, month[dt.tm_mon]);
            if (curr_time_info->tm_year - dt.tm_year == 0)
                printf(" %2d:%02d", dt.tm_hour, dt.tm_min);
            else
                printf(" %5d", dt.tm_year + 1900);                  

        }
	    printf(" %s ", d->d_name);
        if(ls_long) printf("\n");

	}
    if(!ls_long) printf("\n");

    return 1;
}


int pwd()
{
    char cPath[FILENAME_MAX]; 
    if (getcwd(cPath, sizeof(cPath)) == NULL)
    {
        perror("pwd error");
    }
    printf("%s\n", cPath);
    return 1;
}

int rm(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        printf("removing %s\n", c);

        if (unlink(c) == -1) 
        {
            perror("remove error");
            return 1;
        }
    }
         
    return 1;
}

int mv(char** args)
{
   
    if (rename(args[1], args[2]) == -1) 
    {
        perror("mv error");
        return 1;
    }

    return 1;
}

int cp(char** args) 
{
    int in_file = open(args[1], O_RDONLY);
    if (in_file == -1) 
    {
        perror("error opening input file");
        return 1;
    }

    int out_file = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, READ_PERMISSIONS); // rw-rw-rw-
    if (out_file == -1) 
    {
        perror("error opening output file");
        close(in_file); //close the input file descriptor before returning
        return 1;
    }

    struct stat stat_buf;
    if (fstat(in_file, &stat_buf) == -1) 
    {
        perror("error getting input file size");
        close(in_file);
        close(out_file);
        return 1;
    }

    off_t offset = 0;
    while (offset < stat_buf.st_size) 
    {
        ssize_t bytes_sent = sendfile(out_file, in_file, &offset, stat_buf.st_size - offset);
        if (bytes_sent == -1) {
            perror("Error copying file");
            close(in_file);
            close(out_file);
            unlink(args[2]); //remove file we wrote to if error occurs
            return 1;
        }
    }

    close(in_file);
    close(out_file);

    return 1;
}


int shell_mkdir(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        printf("creating %s\n", c);
        if (mkdir(c, EXEC_PERMISSIONS) == -1)
        {
            perror("mkdir error");
            return 1;
        }
    }

    return 1;
}

int shell_rmdir(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        printf("removing %s\n", c);
        if (rmdir(c) == -1) 
        {
            perror("rmdir error");
            return 1;
        }
    }

    return 1;
}

int shell_chmod(char** args)
{
    int permissions = atoi(args[1]);
    chmod(args[2], permissions);
    
    return 1;
}

int cat(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        FILE *file;
        file = fopen(c, "r");
        if(file == NULL) 
        {
            perror("file open error");
            return 1;
        }

        char buff[BUFSIZE];
        while(fgets(buff, BUFSIZE, file)) 
        {
          printf("%s", buff);
        }

        fclose(file);    
    }

    return 1;
}

int touch(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        printf("creating %s\n", c);
        int out_file = open(c, O_CREAT, READ_PERMISSIONS); // rw-rw-rw-

        if (out_file == -1) 
        {
            perror("Error creating file");
            return 1;
        }

        close(out_file);

    }

   return 1;
}

