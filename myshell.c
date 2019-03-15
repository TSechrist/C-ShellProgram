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

void executeLine(char * passedmyargv[BUFFERSIZE])
{
    pid_t id;
    id = fork();

    if(id > 0)
    {
//        printf("Parent\n");
        wait(0);
    }else if (id == 0){

//        printf("Child\n");
        execvp(passedmyargv[0], passedmyargv);
        perror("exec failed");
        exit(52);

    }else{
        perror("Error with fork");
        exit(52);
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
        char * myargv[BUFFERSIZE] = {};
        char input[BUFFERSIZE] = "";

        printf("%s-%s ", username, PROMPT);
        fgets(input, BUFFERSIZE, stdin);
        strtok(input, "\n");

        if(strcmp(input, "exit") == 0)
        {
            exit(0);
        }

        myargc = parseString(myargv, input);

        executeLine(myargv);

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
////    id = fork();
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
////        execvp(&myargv[0], myargv);
////        strtok()
//
////        close(0);
////        dup(pfd(0));
//        close(pfd(0));
//        close(pfd(1));


return 0;
}
