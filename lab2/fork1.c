#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "proc-common.h"
#define SLEEP_PROC_SEC 10

// A forks B, forks C, waits for them to die, and dies
// B forks D, waits for it to die, and dies

void fork_procs(void)
{
    // PROCESS A
    change_pname("A");	// onomazoume thnn nea diergasia "A"
    printf("A with PID = %ld is created...\n", (long) getpid());
    pid_t pid_b, pid_c, pid_d;
    int status;
    printf("A with PID = %ld: Creating child B...\n", (long) getpid());
    pid_b = fork();	// dhmiourgoume nea diergasia thn opoia tha onomasoume "B"
    if (pid_b < 0) { perror("B: fork"); exit(1); }

        // PROCESS B
        if (pid_b == 0)  // an eimai to paidi
        {
            change_pname("B");	
            printf("B with PID = %ld is created...\n", (long) getpid());
            printf("B with PID = %ld: Creating child D...\n", (long) getpid());
            pid_d = fork();	//dimiourgia neas diergasias thn opoia tha onomasoume "D"
            if (pid_d < 0) { perror("D: fork"); exit(1); }
    
                // PROCESS D
                if (pid_d == 0)  // an eimai to paidi
                {
                    change_pname("D");	//thn onomatizoume "D"
                    printf("D with PID = %ld is created...\n", (long) getpid());
                    printf("D with PID = %ld is ready to Sleep...\n", (long) getpid());
                    sleep(SLEEP_PROC_SEC);	//bazoume thn "D" pou einai fullo na koimatai oste na dimiourgithoun kai oi alles
                    printf("D with PID = %ld is ready to terminate...\nD: Exiting...\n", (long) getpid());
                    exit(13);
                }
    
            waitpid(pid_d, &status, 0);	        // perimenei o pateras mexri na pethanei to D gia na sunexisei
            explain_wait_status(pid_d, status);	// eksigei ton logo pou pethane to paidi (to id tou paidiou 1o orisma)
            printf("B with PID = %ld is ready to terminate...\nB: Exiting...\n", (long) getpid());
            exit(19);
        }

    printf("A with PID = %ld: Creating child C...\n", (long) getpid());
    pid_c = fork();	// dhmiourgia neas diergasias thn opoia tha onomasoume "C"   
    if (pid_c < 0) { perror("C: fork"); exit(1); }

        // PROCESS C
        if (pid_c == 0)  // an eimai to paidi
        {
            change_pname("C");	//thn onomazoume "C"
            printf("C with PID = %ld is created...\n", (long) getpid());
            printf("C with PID = %ld is ready to sleep...\n", (long) getpid());
            sleep(SLEEP_PROC_SEC);	//einai fullo ara tha thn baloume na koimithei
            printf("C with PID = %ld is ready to terminate...\nC: Exiting...\n", (long) getpid());
            exit(17);
        }

    waitpid(pid_c, &status, 0);	//perimenei o pateras mexri na pethanei to paidi C gia na sunexisei
    explain_wait_status(pid_c, status);	//eksigei ton logo pou pethane to paidi pou tou dinoume os 1o orisma
    waitpid(pid_b, &status, 0);	//perimenei o pateras mexri na pethanei to paidi B gia na sunexisei
    explain_wait_status(pid_b, status);	//eksigei ton logo pou pethane to B
    printf("A with PID = %ld is ready to terminate...\nA: Exiting...\n", (long) getpid());
    exit(16);
}

int main(void)
{
    pid_t pid;
    int status;

    // main forks, it generates the root of the process tree named A
    pid = fork();  // processes main() and A are both alive 

    // Upon successful completion, fork() returns 0 to the child process 
    //                                and returns the process ID of the child process to the parent process. 
    // Otherwise, -1 is returned to the parent process, no child process is created, and errno is set to indicate the error.

    if (pid < 0) { perror("main: fork"); exit(1); }

        // PROCESS A
        if (pid == 0)    // an eimai to paidi, diladi o A
        {
            fork_procs();
            exit(1);  // A dies
        }


    // PROCESS main()                   // back to main()
    show_pstree(pid);                   // emfanizei to dendro diergasion pou exei dhmiourgithei
    waitpid(pid, &status, 0);	        // H main() perimenei mexri na pethanei to paidi tis diladi o A
    explain_wait_status(pid, status);	// eksigei ton logo pou pethane o A
    return 0;                           // main() terminates
}
