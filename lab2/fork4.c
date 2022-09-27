#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "tree.h"
#include "proc-common.h"
#define SLEEP_TREE_SEC 3
#define SLEEP_PROC_SEC 10


// Επειδή οι πράξεις + και * είναι αντιμεταθετικές μπορούμε να χρησιμοποιήσουμε ένα pipe για κάθε γονική διεργασία. Οι
// διεργασίες φύλλα γράφουν στο άκρο εγγραφής το ονομά τους σε ακέραιο και οι γονείς (+, *) τα διαβάζουν,
// κάνουν την πράξη και μετά το γράφουν και αυτά στο γονέα τους το αποτέλεσμα.



void fork_procs(struct tree_node *t, int parentPipe[])
{
    int i;
    printf("PID = %ld, name %s, starting...\n", (long) getpid(), t->name);
    change_pname(t->name);

    if (t->nr_children == 0)  // elegxos gia to an einai fullo
    {
        int value;
        sleep(SLEEP_PROC_SEC);	// koimountai oste na prolabei na dimiourgithei to dentro
        if (close(parentPipe[0]) < 0)
            { perror("Close"); exit(1); }
        value = atoi(t->name);	                             // metatroph xarakthra se arihtmo
        if (write(parentPipe[1], &value, sizeof(int)) < 0)   // grafei sto akro (1) ths pipe kai elegxei an graftike 
              { perror("Write"); exit(1); }
        printf("%s with PID = %ld is ready to terminate...\n%s: Exiting...\n", t->name, (long) getpid(), t->name);
        close(parentPipe[1]);
 	exit(0);
    }
    else  // I am not a leaf, I have to make a subtree (at least)
    {
        int numbers[2];	   // dilwsh ton duo arithmwn ton duo fullwn
        int myPipe[2];	   // dilwsh ton duo deikton sthn pipe. 1->gia eggrafh, 0-> gia anagnosh
        if (pipe(myPipe)) { perror("Failed pipe"); }
        int status;
        pid_t pid;
        for (i = 0; i < 2; i++)
        {
            pid = fork();
            if (pid < 0) { perror("proc: fork"); exit(1); }

            if (pid == 0)
            {
                fork_procs(t->children + i, myPipe);   // kalei anadromika thn fork_procs gia na dimiourgithei olo to dentro
                exit(1);
            }
        }

        if (close(myPipe[1]) < 0)
             { perror("Close"); exit(1); }

        for (i = 0; i < 2; i++)
        {
            if (read(myPipe[0], numbers + i, sizeof(int)) < 0)   // diabazei tous arithous ton duo fullon
                { perror("Read"); exit(1); }
        }

        int result;
        if (!strcmp(t->name, "+")) result = numbers[0] + numbers[1];	// elegxei thn praksh apo ton patera kai ektelei thn antistoixh praksh
        else                       result = numbers[0] * numbers[1];	

        if (write(parentPipe[1], &result, sizeof(int)) < 0)  // grapse to apotelesma sto pipe      
        {
            perror("Write");
            exit(1);
        }

        for (i = 0; i < 2; i++)
        {
            waitpid(pid, &status, 0);
            explain_wait_status(pid, status);
        }
    }

    printf("%s with PID = %ld is ready to terminate...\n%s: Exiting...\n", t->name, (long) getpid(), t->name);
    exit(0);
}


int main(int argc, char *argv[])
{
    pid_t pid;
    int status, pipe_fd[2];
    struct tree_node * root;

    if (argc < 2)   // elegxos an exoun dothei ligotera apo 1 orismata
    {
        fprintf(stderr, "Usage: %s<tree_file>\n", argv[0]);
        exit(1);
    }

    root = get_tree_from_file(argv[1]);   // argv[1] is a filename provided by the user, read input tree from the file, return root

    if (pipe(pipe_fd))	// elegxos gia to an h pipe dimiourgithike sosta
        { perror("Failed pipe"); }

    pid = fork();  /* Fork root of process tree */	
    if (pid < 0) { perror("main: fork"); exit(1); }

    // PROCESS ROOT
    if (pid == 0)
    {
        fork_procs(root, pipe_fd);
        exit(1);
    }

    // BACK TO main()
    sleep(SLEEP_TREE_SEC);	// koimatai o pateras oste na dimioutgithei to dentro kai na graftei to apotelesma
    show_pstree(pid);	// emfanizei to dendro diergasion pou exei dhmiourgithei
    int result;
    close(pipe_fd[1]);
    if (read(pipe_fd[0], &result, sizeof(int)) < 0)  // diabasma kai ekxorisi tou apotelesmatos sto result
         { perror("Read"); exit(1); }
    wait(&status);   /* Wait for the root of the process tree to terminate */
    explain_wait_status(pid, status);
    printf("\nThe result is %d\n", result);
    return 0;
}
