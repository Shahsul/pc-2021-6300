#include<unistd.h>

#include<stdio.h>

void main()
{
    pid_t child_n2, grandchild_n3, grandchild_n4;

    printf("Parent process n1 and pid %d\n", getpid());

    child_n2 = fork();
    if(child_n2 == 0)
    {
        // child process n2
        printf("Child process n2 and pid is %d\n", getpid());

        grandchild_n3 = fork();
        if (grandchild_n3 == 0)
        {
            // First grandchild n3
            printf("First grandchild n3 and pid is %d parent pid %d\n", getpid(),getppid());

        }
       else if (grandchild_n3 > 0)
        {

            grandchild_n4 = fork();
            if (grandchild_n4 == 0)
            {
                // Second grandchild n4
                printf("Second grandchild n4 and pid is %d parent pid %d \n", getpid() , getppid());

            }

           }
    }

}
