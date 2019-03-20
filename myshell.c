/****************************************************************
 * Name        :  Thomas Sechrist                               *
 * Class       :  CSC 415                                       *
 * Date        :                                                *
 * Description :  Writting a simple bash shell program          *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)

//This is a simple function that will parse a string given to it by
//a space dimiliter. It will return how large the array is.
int parseString(char * newmyargv[BUFFERSIZE], char * newinput)
{
    int count = 0;
    char* token = strtok(newinput, " ");
    for(int i = 0; token != NULL; i++)
    {
        newmyargv[i] = token;
        token = strtok(NULL, " ");
        count++;
    }
    return count;
}

//This function will execute a passed in argv array list and execute
//it with all of its arguments. It will check for redirection.
void executeLine(char * passedMyargv[BUFFERSIZE])
{

    pid_t id;
    id = fork();

    if (id > 0)
    {
//        printf("Parent\n");
        wait(0);
    }
    else if (id == 0)
    {
//        printf("Child\n");

        int inputBool = 0;
        int truncOutputBool = 0;
        int outputBool = 0;
        char inputFile[BUFFERSIZE];
        char outputFile[BUFFERSIZE];

        /*
         * The logic for this redirect function is taken from my
         * file copy assignment and I used some posts on stack
         * overflow to fix it to make it work.
         */

        for (int i = 0; passedMyargv[i] != '\0'; i++)
        {
            if (strcmp(passedMyargv[i], "<") == 0)
            {
//                printf("Inside input redirection bool change\n");
                inputBool = 1;
                passedMyargv[i] = NULL;
                strcpy(inputFile, passedMyargv[i + 1]);
            }
            else if (strcmp(passedMyargv[i], ">") == 0)
            {
//                printf("Inside truncate output redirection bool change\n");
                truncOutputBool = 1;
                passedMyargv[i] = NULL;
                strcpy(outputFile, passedMyargv[i + 1]);
            }
            else if (strcmp(passedMyargv[i], ">>") == 0)
            {
//                printf("Inside output redirection bool change\n");
                outputBool = 1;
                passedMyargv[i] = NULL;
                strcpy(outputFile, passedMyargv[i + 1]);
            }
        }

            if (inputBool)
            {
                int fin;
                if (0 > (fin = open(inputFile, O_RDONLY))) {
                    perror("open for read failed\n");
                    exit(EXIT_FAILURE);
                }
                dup2(fin, STDIN_FILENO);
                close(fin);
            }

            if (truncOutputBool)
            {
                int truncfout;
                if (0 > (truncfout = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666))) {
                    perror("open for write failed\n");
                    exit(EXIT_FAILURE);
                }
                dup2(truncfout, STDOUT_FILENO);
                close(truncfout);
            }

            if (outputBool)
            {
                int fout;
                if (0 > (fout = open(outputFile, O_WRONLY | O_CREAT | O_APPEND, 0666))) {
                    perror("open for write failed\n");
                    exit(EXIT_FAILURE);
                }
                dup2(fout, STDOUT_FILENO);
                close(fout);
            }

            execvp(passedMyargv[0], passedMyargv);
            perror("exec failed");
            exit(EXIT_FAILURE);

    }
    else
    {
        perror("Error with fork");
        exit(52);
    }
}

//This function will be called if there is a pipe in the inputted command.
//It will seperate the two sides of the pipe into seperate argv arrays
//and execute them with a pipe connecting them.
void executeLinePipe(char * passedMyargv[BUFFERSIZE], int passedMyargc, int pos)
{

    char * leftArgv[BUFFERSIZE];
    char * rightArgv[BUFFERSIZE];

    for(int i = 0; i < pos; i++)
    {
        leftArgv[i] = passedMyargv[i];
    }
    for(int i = pos + 1; i < passedMyargc; i++)
    {
        rightArgv[i - (pos + 1)] = passedMyargv[i];
    }

    leftArgv[pos] = NULL;

    pid_t id;
    int pipe_fd[2], status; //0 read end and 1 write end
    pipe(pipe_fd);
    id = fork();

    if(id < 0)
    {
        perror("Error Forking");
        exit(0);
    }
    else if(id == 0)
    {
//        printf("pid in Child: %d\n", id);
        close(0);
        dup(pipe_fd[0]);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(rightArgv[0], rightArgv);
        perror("Right side failure");
    }
    else
    {
        id = fork();

        if(id < 0)
        {
            perror("Error Forking");
            exit(0);
        }
        else if (id == 0) {
            //        printf("In parent\n");
            close(1);
            dup(pipe_fd[1]);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            execvp(leftArgv[0], leftArgv);
            perror("Left side failure");
        }
        else
        {
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            waitpid(id, &status, 0);
        }
    }
}

//Calling the getcwd command to tell our current working directory
void printDir()
{
    char currdir[BUFFERSIZE];
    getcwd(currdir, sizeof(currdir));
    printf("current working directory: %s\n", currdir);
}

//Changing our current working directory.
void changeDir(char * passedmyargv[BUFFERSIZE])
{
    if(passedmyargv[1] != NULL)
    {
        chdir(passedmyargv[1]);
    }
    else
    {
        chdir(getenv("HOME"));
    }
}

//Main function that has an endless while loop to keep gathering user input.
//You can end the loop by typing exit. We check user input and call the
//functions needed.
int
main(int argc, char** argv)
{

    char username[BUFFERSIZE];
    getlogin_r(username, BUFFERSIZE);

    while(1)
    {
        wait(NULL);
        int myargc = 0;
        int pipeBool = 0;
        char * myargv[BUFFERSIZE] = {};
        char input[BUFFERSIZE] = "";
        char currdir[BUFFERSIZE];
        getcwd(currdir, sizeof(currdir));

        printf("%s-myshell %s >> ", username, currdir);
        fgets(input, BUFFERSIZE, stdin);
        strtok(input, "\n");

        //This will quit the program
        if(strcmp(input, "exit") == 0)
        {
            break;
        }

        myargc = parseString(myargv, input);

        if(strcmp(myargv[0], "\n") == 0)
        {
            continue;
        }
        else if(strcmp(myargv[0], "pwd") == 0)
        {
            printDir();
        }
        else if(strcmp(myargv[0], "cd") == 0)
        {
            changeDir(myargv);
        }
        else
        {
            //Checking input for a pipe
            for(int i = 0; i < myargc; i++)
            {
                if(strcmp(myargv[i], "|") == 0)
                {
                    pipeBool = 1;
                    executeLinePipe(myargv, myargc, i);
                }
            }
            if(pipeBool == 0)
            {
                executeLine(myargv);
            }
        }
    }

return 0;
}
