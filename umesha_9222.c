#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_NAME 2048
#define MAX_FILES 1024


void printDirectory();
void printTime();
void freeMem(char **, int);
char **getFilesInDir(unsigned char , int *);
void printMenu();
char *findFile(unsigned char);



void printDirectory() {
    char s[256];
    getcwd(s, 200);
    printf("Current Directory: %s\n", s); //print the current working directory.
}


void printTime() {
    time_t t;
    t = time(NULL);
    printf( "Current time: %s\n", ctime( &t )); // print the current time.
}


void freeMem(char **files, int size) {
    int i;
    for (i = 0; i < size; i++) {
    free(files[i]);
    }
    free(files);
}

char **getFilesInDir(unsigned char type, int *fileCounter) {
    DIR *d;
    struct dirent *de;
    char **files = NULL;
    int counter = 0;
    d = opendir(".");   //open current working directory.
    int i = 0;
    while ((de = readdir(d))) {

        if (strlen(de->d_name) > MAX_NAME) {
            printf("ERROR! At less one file in the current directory exceed the max \n"
                    "of %d characters allowed as a name.\n\nExiting program...\n\n", MAX_NAME);
            exit(1);
        }

        if (de->d_type == type) {
            counter++;
            files = realloc(files, counter * sizeof(char *));
            files[i] = malloc(strlen(de->d_name) + 1);
            strcpy(files[i], de->d_name);
            i++;
        }
    }

    closedir(d);
    *fileCounter = counter;
    return files;
}

void printMenu() {
    char **files;
    char **directories;
    int fileCounter, dirCounter = 0;
    int i;
    printDirectory();
    printTime();
    files = getFilesInDir(DT_REG, &fileCounter);
    directories = getFilesInDir(DT_DIR, &dirCounter);
    if ((fileCounter + dirCounter) > MAX_FILES) {
        printf("ERROR! Number of files and directories in the\n"
                "current directory exceed the max of %d.\n\nExiting program...\n\n", MAX_FILES);
        exit(1);
    }

    if (fileCounter == 0) {
        printf("Files:There is no file in current directory.\n");
    } else 
{
       
         printf( "\n================================Files====================================\n");
        i = 0;
        while (i < fileCounter) {
            printf("%d. %s\n", i, files[i]);
            i++;

            if ((i % 10) == 0) { 
                printf("\nEnter any key for next, 'p' for previous,'s' to skip: ");
                char c = getchar();
                while (getchar() != '\n' && c != EOF);
                switch(c) {
                    case 'p':i -= 20;
                             if (i < 0) { i = 0; }
                             break;
                    case 's':i = fileCounter;
                             break;
                }
            }
        }
    }

    printf("\nMoving on to listing directories.\nEnter to continue.");
    while (getchar() != '\n');

    if (dirCounter == 0) {
        printf("\nDirectory:There is no sub-directory in current directory.");
    } else {
        // print sub-directories in current working directory
        printf( "\n====================================Directories=========================================\n");
        i = 0;
        while (i < dirCounter) {
            printf("%d. %s\n", i, directories[i]);
            i++;
                if ((i % 10) == 0) {
                printf("\nEnter any key for next, 'p' for previous, or 's' to skip: ");
                char c = getchar();
                while (getchar() != '\n' && c != EOF);   
                switch(c) {
               case 'p':i -= 20;
                        if (i < 0) { i = 0; }
                        break;
               case 's':i = dirCounter;
                        break;
                }
            }
        }
    }

    printf("=================Operation=====================\n");
    printf( "     'e'  Edit\n"
            "     'r'  Run\n"
            "     'c'  Change directory\n"
            "     'd'  Display\n"
            "     'q'  Quit\n"
            "     's'  Sort\n"
	    "     'f'  Remove\n"
	    "     'm'  Move\n");
    printf("===============================================\n");

    freeMem(files, fileCounter);
    freeMem(directories, dirCounter);
}

char *findFile(unsigned char type) {
    char *input = malloc(sizeof(char *));
    char buffer[256];
    int i ,counter;
    char **files;
    files = getFilesInDir(type, &counter);
	if (fgets(buffer, sizeof(buffer), stdin)) {
        if (1 == sscanf(buffer, "%d", &i))
	 {  
            if (i < counter) 
	    {     
                strcpy(input, files[i]);
                freeMem(files, counter);
                return input;
            }
        }
        strtok(buffer, "\n"); // remove trailing newline.
        strcpy(input, buffer);
    }

    freeMem(files, counter);
    return input;
}

int main(void)
{
    int c, l;
    char *s;
    char cmd[256],var[256],dir[256];
    const char EDITOR[] = "nano ";
    system("clear");
    printMenu();
    printf("Choose an Operation: ");

    while (1) {
        c = getchar();
        while (getchar() != '\n' && c != EOF);  
        switch (c) {
            case 'q': exit(0);     
		     // quit program

            case 'c': printf("Change To?: ");
                      s = findFile(DT_DIR);
                      strcpy(cmd, s);
                      if (chdir(cmd) == -1) {
                          printf("No such directory.Try again.\n\n");
                      } else {
                          system("clear");
                          printMenu();
                      }
                      printf("Choose an Operation:> ");
                      break;
			// change directory 

            case 'e': printf("Edit what?: ");
                      s = findFile(DT_REG);
                      strcpy(cmd, EDITOR);
                      strcat(cmd, s);
                      system(cmd);
                      printf("Choose an Operation:> ");
                      break;
			//Edit files

            case 'r': printf("Run what?: ");
                      s = findFile(DT_REG);
                      strcpy(cmd, "./");
                      strcat(cmd, s);
                      system(cmd);
                      printf("Choose an Operation:> ");
                      break;
			//Run exe files

            case 'd': system("clear");
                      printMenu();
                      printf("Choose an Operation:> ");
			// Display Files 

	    case 's': system("ls -laS");
                      break;
			//Sort files

	    case 't': system("ls -lt");
	              break;

	    case 'f': printf( "Remove file?:" );
		      scanf( "%s", cmd );
		      remove( cmd );
  		      break;
			// Remove file 
	    case 'm': printf( "Source file to be moved:" );
		      scanf("%s" ,cmd);
		      printf( "Destination file to be moved:" );
		      scanf( "%s", dir );
		      strcpy(var,"mv ");
		      strcpy(var, cmd );
		      strcpy(var," ");
		      strcpy(var, dir );
		      system(dir);
  		      break;
			// Move directory 

            default: printf("Unknown operation\n Try again...\n");
                     printf("Choose an Operation:> ");
                     continue;
        }

    return 0;
}
}