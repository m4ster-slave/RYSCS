#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>


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

int help(char** args)
{
  printf("RYSCS \n");
  printf("Type programs and args and hit enter.\n");
  printf("The following are builin:\n");
  printf("Use man on other programs.\n");
  return 1;
}

int shell_exit(char** args)
{
  return 0;
}

int ls(char** args)
{
    int ls_long = 0, show_hidden = 0;
    char cPath[FILENAME_MAX]; 
	struct dirent *d;
    struct stat stats;

    if (args[1] == NULL)
    {
        getcwd(cPath, sizeof(cPath));
    }
    else 
    {
        //see if -l is set
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
                printf("invalid number of arguments or not a direcotry\n");
            }
        }
    }

	DIR *dh = opendir(cPath);

	//While the next entry is not readable print directory files
	while ((d = readdir(dh)) != NULL)
	{
        if (d->d_name[0] == '.' && !show_hidden) continue;

        if (ls_long) {
            struct tm dt;
            stat(d->d_name, &stats);

            // File permissions
            if (stats.st_mode & R_OK)
                printf("read ");
            if (stats.st_mode & W_OK)
                printf("write ");
            if (stats.st_mode & X_OK)
                printf("execute ");

            // File size
            printf("size: %ld ", stats.st_size);

            // Get file creation time in seconds and 
            // convert seconds to date and time format
            dt = *(gmtime(&stats.st_ctime));
            printf("created: %d-%d-%d %d:%d:%d ", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900, 
                                              dt.tm_hour, dt.tm_min, dt.tm_sec);

            // File modification time
            dt = *(gmtime(&stats.st_mtime));
            printf("modified: %d-%d-%d %d:%d:%d ", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900, 
                                              dt.tm_hour, dt.tm_min, dt.tm_sec);
        }
        
	    printf("%s ", d->d_name);

        if(ls_long) printf("\n");
	}
    printf("\n");
    return 1;
}


int pwd(char** args)
{
    char cPath[FILENAME_MAX]; 
    getcwd(cPath, sizeof(cPath));
    printf("%s\n", cPath);
    return 1;
}

int rm(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        printf("removing %s\n", c);
        unlink(c);
    }
         
    return 1;
}

int mv(char** args)
{
    rename(args[1], args[2]);
    return 1;
}

int cp(char** args) 
{
    int in_file = open(args[1], O_RDONLY);
    if (in_file == -1) 
    {
        perror("Error opening input file");
        return 1;
    }

    int out_file = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0666); // rw-rw-rw-
    if (out_file == -1) 
    {
        perror("Error opening output file");
        close(in_file); // Close the input file descriptor before returning
        return 1;
    }

    struct stat stat_buf;
    if (fstat(in_file, &stat_buf) == -1) 
    {
        perror("Error getting input file size");
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
            unlink(args[2]);
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
        if (mkdir(c, 0777) == -1)
        {
            printf("couldnt create file\n");
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
        rmdir(c);
    }

    return 1;
}

int shell_chmod(char** args)
{
    
    return 1;
}

int cat(char** args)
{
    for (char* c = *++args; c; c=*++args) 
    {
        FILE *file;

        file = fopen(c, "r");
        if(file == NULL) {
            printf("Failed to open file %s\n", c);
        }

        char buff[100];

        while(fgets(buff, 100, file)) {
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
        int out_file = open(c, O_CREAT, 0666); // rw-rw-rw-
        
        if (out_file == -1) 
        {
            perror("Error creating file");
            return 1;
        }

        close(out_file);

    }

   return 1;
}
