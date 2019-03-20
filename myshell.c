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

void executeLinePipe(char * passedMyargv[BUFFERSIZE], int passedMyargc, int pos)
{


    char * leftArgv[BUFFERSIZE] = {"ls", "-l", NULL};
    char * rightArgv[BUFFERSIZE]= {"wc", "-l", NULL};

    pid_t id;
    int pipe_fd[2]; //0 read end and 1 write end

    pipe(pipe_fd);

    id = fork();
    if(id < 0)
    {
        perror("Error Forking");
        exit(0);
    }
    else if(id == 0) {

        printf("In Child\n");
        close(0);
        dup2(pipe_fd[0], 0);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(rightArgv[0], rightArgv);
    }

    id = fork();
    if(id < 0)
    {
        perror("Error Forking");
        exit(0);
    }
    else if(id == 0)
    {
        wait(0);
        printf("In Parent\n");
        close(1);
        dup2(pipe_fd[1], 1);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(leftArgv[0], leftArgv);
    }

    printf("***Before wait\n");
    wait(0);
    printf("***After wait\n");

    /*
    printf("***DEBUG 1\n");

    char * leftArgv[BUFFERSIZE];
    char * rightArgv[BUFFERSIZE];

    leftArgv[pos] = NULL;
    rightArgv[passedMyargc] = NULL;

    for(int i = 0; i < pos; i++)
    {
        leftArgv[i] = passedMyargv[i];
        printf("leftarg: %s\n", leftArgv[i]);
    }

    for(int i = pos + 1; i < passedMyargc; i++)
    {
        rightArgv[i] = passedMyargv[i];
        printf("rightarg: %s\n", rightArgv[i]);
    }

    printf("***DEBUG 2\n");

    pid_t id;
    int pipe_fd[2]; //0 read end and 1 write end

    pipe(pipe_fd);

    id = fork();
    if(id == 0) {

        printf("***DEBUG 3\n");

        close(0);
        dup(pipe_fd[0]);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(rightArgv[0], rightArgv);
    }

    id = fork();
    if(id == 0) {

        printf("***DEBUG 4\n");

        close(1);
        dup(pipe_fd[1]);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(leftArgv[0], leftArgv);
    }

    printf("***DEBUG 5\n");

    wait(0);
*/
}


void printDir()
{
    char currdir[BUFFERSIZE];
    getcwd(currdir, sizeof(currdir));
    printf("current working directory: %s\n", currdir);
}

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

int
main(int argc, char** argv)
{

    char username[BUFFERSIZE];
    getlogin_r(username, BUFFERSIZE);

    while(1)
    {

        int myargc = 0;
        int pipeBool = 0;
        char * myargv[BUFFERSIZE] = {};
        char input[BUFFERSIZE] = "";
        char currdir[BUFFERSIZE];
        getcwd(currdir, sizeof(currdir));

        printf("%s-myshell %s >> ", username, currdir);
        fgets(input, BUFFERSIZE, stdin);
        strtok(input, "\n");

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
            for(int i = 0; i < myargc; i++)
            {
                if(strcmp(myargv[i], "|") == 0)
                {
                    pipeBool = 1;
                    executeLinePipe(myargv, myargc, i);
                }
            }
            if(!pipeBool)
            {
                executeLine(myargv);
            }
        }
    }



    //int execvp

//    pid_t id;
//    int pipe_fd[2]; //0 read end and 1 write end
//
//    char* left_side[] = {"ls", "-l", "/", NULL};
//    char* right_side[] = {"wc", "-l", NULL};
//    char* right_right_side[] = {"wc", "-c", NULL};
//
//    pipe(pipe_fd);
//
//    id = fork();
//    if(id == 0) {
//        close(0);
//        dup(pipe_fd[0]);
//        close(pipe_fd[0]);
//        close(pipe_fd[1]);
//        execvp(right_side[0], right_side);
//    }
//
//    id = fork();
//    if(id == 0) {
//        close(1);
//        dup(pipe_fd[1]);
//        close(pipe_fd[0]);
//        close(pipe_fd[1]);
//        execvp(left_side[0], left_side);
//    }
//    wait(0);
//
//    id = fork();
//
//    if(id > 0)
//    {
//        printf("Parent: %d\n", getpid());
//        wait(0);
//
//    }
//    else if(id == 0)
//    {
//        printf("Child: %d\n", getpid());
//        execlp("ls", "ls", NULL);
//        execvp(&myargv[0], myargv);
//        strtok()
//
//        close(0);
//        dup(pfd(0));
//        close(pfd(0));
//        close(pfd(1));


return 0;
}
