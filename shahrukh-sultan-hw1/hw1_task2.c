
#include <iostream>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#include<unistd.h>

#include<stdio.h>

int main()
{
    pid_t parent , c1 , c2, gc1;

    printf("Parent process (N1), pid: %d\n", getpid());

  c1 = fork();
        if (c1 == 0)
        {
            // Getting child_1
            printf("Child_1 (N2), pid: %d  from parent pid: %d\n", getpid(),getppid());

        }
       else if (c1 > 0)
        {

            c2 = fork();
            if (c2 == 0)
            {
                // Getting child_2
                printf("Child_2 (N3), pid: %d and parent pid: %d\n", getpid(),getppid());


            gc1 = fork();
            if (gc1 == 0)
            {
                printf("Child_1 (N3) as parent of grandchildren_1 (N4), pid: %d\n",getppid());
                // Getting grandchild_1
                printf("Grandchild_1 (N4), pid: %d\n", getpid());
            }
            }
           }
    }

