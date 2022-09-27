#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "proc-common.h"
#define SLEEP_TREE_SEC 3
#define SLEEP_PROC_SEC 10

void fork_procs(struct tree_node *t)
{
    int i;
    printf("PID = %ld, name %s, starting...\n", (long) getpid(), t->name);
    change_pname(t->name);	//dinoume sthn diergasia to onoma pou exei sto tree

    if (t->nr_children == 0)  // an eimai fyllo
    {
    	sleep(SLEEP_PROC_SEC);  // peftw gia y// emfanizei to dendro diergasion pou exei dhmiourgitheipno
        printf("%s with PID = %ld is ready to terminate...\n%s: Exiting...\n", t->name, (long) getpid(), t->name);
        exit(0);
    }
    else  // I am not a leaf, I have to make a subtree (at least)
    {
        int status;
        pid_t pid[t->nr_children];

        for (i = 0; i < t->nr_children; i++)  // make all subtrees
        {
            pid[i] = fork();  // for every child, call fork_procs recursively	
            if (pid[i] < 0) { perror("proc: fork"); exit(1); }
            if (pid[i] == 0)
            {
                fork_procs(t->children + i);	// kalei anadromika thn fork_procs gia na dimiourgithei olo to dentro
                exit(1);
            }
        }

        for (i = 0; i < t->nr_children; i++)  // o pateras perimenei na pethanoun ta paidia tou
        {
            waitpid(pid[i], &status, 0);  
            explain_wait_status(pid[i], status);
        }
    }
    printf("%s with PID = %ld is ready to terminate...\n%s: Exiting...\n", t->name, (long) getpid(), t->name);  // father dies
    exit(0);
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    struct tree_node * root;

    if (argc < 2)  // elegxos an exoun dothei ligotera apo 1 orismata
    {
        fprintf(stderr, "Usage: %s<tree_file>\n", argv[0]);
        exit(1);
    }

    root = get_tree_from_file(argv[1]);	// argv[1] is a filename provided by the user, read input tree from the file, return root

    pid = fork();   /* Fork root of process tree */
    if (pid < 0) { perror("main: fork"); exit(1); }

    // PROCESS ROOT
    if (pid == 0)
    {
        fork_procs(root);
        exit(1);
    }

    // BACK TO main()

    sleep(SLEEP_TREE_SEC);	// koimatai i main() kai i root() gennaei to dendro

    show_pstree(pid);	// emfanizei to dendro diergasion pou exei dhmiourgithei
    waitpid(pid, &status, 0);	     /* Wait for the root of the process tree to terminate */
    explain_wait_status(pid, status);	//eksigisi thanatou
    return 0;
}
