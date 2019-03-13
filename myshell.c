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

/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)

int * parseString(char * newmyargv[256], char * newinput)
{
    int count = 0;
    char* token = strtok(newinput, " ");
    for(int i = 0; token != NULL; i++)
    {
//            printf("token: %s\n", token);
        newmyargv[i] = token;
        token = strtok(NULL, " ");
        count++;
    }
//    printf("%d\n", ((int)newmyargc / 4));
    return count;
}





int
main(int argc, char** argv)
{
    int * myargc;
    char * myargv[256];
    char input[256];
    char username[256];
    getlogin_r(username, 256);

    printf("%s-MYSHELL$ ", username);
    fgets(input, 256, stdin);
    while(strcmp(input, "exit\n"))
    {

        myargc = parseString(myargv, input);

        for(int i = 0; myargv[i] != NULL; i++)
        {
            printf("myargv[%d]: %s\n", i, myargv[i]);
        }

        printf("myargc: %d\n", (int)myargc);

        printf("%s-MYSHELL$ ", username);
        fgets(input, 256, stdin);

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
