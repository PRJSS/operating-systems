//
//  main.c
//  part 3 var 1
//
//  Created by Бернат Данила on 15.11.2021.
//
//  Thanks to https://github.com/GulshirinBerdiyeva

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char **argv)
{
    int pNum=0;
    
    if(fork()==0){
        pNum = 1;
        printf("1 process %d and his parent is %d \n",
               getpid(), getppid());
    }
    
    if (pNum == 1){
        
        if(fork()==0){
        char *argv[] = {"/bin/ls", "-l", 0};
        execvp(argv[0], argv);
        }
        
        if(fork()==0){
            pNum = 2;
            printf("2 process %d and his parent is %d \n",
                   getpid(), getppid());
        }
        
        if (pNum == 1){
            if(fork()==0){
                pNum = 3;
                printf("3 process %d and his parent is %d \n",
                       getpid(), getppid());
                
                if (pNum == 3){
                    if(fork()==0){
                        pNum = 5;
                        printf("5 process %d and his parent is %d \n",
                               getpid(), getppid());
                        
                        if (pNum == 5){
                            if(fork()==0){
                                pNum = 7;
                                printf("7 process %d and his parent is %d \n",
                                       getpid(), getppid());
                            }
                        }
                    }
                    if (pNum == 3){
                        if(fork()==0){
                            pNum = 6;
                            printf("6 process %d and his parent is %d \n",
                                   getpid(), getppid());
                        }
                    }
                }
            }
            
            if (pNum == 1){
                if(fork()==0){
                    pNum = 3;
                    printf("4 process %d and his parent is %d \n",
                           getpid(), getppid());
                }
            }
        }
    }
}

//log:
//1 process 4449 and his parent is 4447
//2 process 4451 and his parent is 4449
//3 process 4452 and his parent is 4449
//4 process 4453 and his parent is 4449
//5 process 4454 and his parent is 4452
//6 process 4455 and his parent is 4452
//7 process 4456 and his parent is 4454
//total 136
//-rwxr-xr-x  1 bernat  staff  69440 Nov 15 01:38 part 3 var 1
//Program ended with exit code: 0
