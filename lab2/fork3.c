#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "proc-common.h"

// i main gennaei ti riza, afti kalei fork_procs, genniountai oles oi diergasies, automplokarontai me SIGSTOP apo katw pros ta panw,
// mexri ti riza, i main ksipnaei me SIGCONT ti riza, afti ta paidia tis, klp mexri ta fylla, kai telos oi diergasies
// pethainoun apo katw pros ta panw, mexri ti riza kai telos ti main.

void fork_procs(struct tree_node *t)
{
    int i;
    printf("PID = %ld, name %s, starting...\n", (long) getpid(), t->name);
    change_pname(t->name);

    if (t->nr_children == 0)  // elegxos gia to an einai fullo
    {
        raise(SIGSTOP);	 // stelnei to paidi ston eauto tou minima oste na stamatisei thn leitourgia tou mexri na tou erthei shma
        printf("PID = %ld, name = %s is awake\n", (long) getpid(), t->name);
    }
    else   // I am not a leaf, I have to make a subtree (at least)
    {
        int status[t->nr_children];
        pid_t pid[t->nr_children];

        for (i = 0; i < t->nr_children; i++)    // make all subtrees
        {
            pid[i] = fork();  // for every child, call fork_procs recursively
            if (pid[i] < 0) { perror("proc: fork"); exit(1); }

            if (pid[i] == 0)
            {
                fork_procs(t->children + i);   // kalei anadromika thn fork_procs gia na dimiourgithei olo to dentro
                exit(1);
            }
        }
        wait_for_ready_children(t->nr_children);	// o kathe pateras perimenei na kanoun raise(SIGSTOP) ta paidia gia na sunexisei

        raise(SIGSTOP);	// o pateras stamataei thn leitourgia tou mexri na tou erthei kapoio shma, SIGnal STOP
        printf("PID = %ld, name = %s is awake\n", (long) getpid(), t->name);  // o pateras ksipnise

        for (i = 0; i < t->nr_children; i++)	// o pateras stelnei sta paidia tou shma na ksipnisoun
        {
            kill(pid[i], SIGCONT);   // SIGnal CONTinue
            waitpid(pid[i], &status[i], 0);
            explain_wait_status(pid[i], status[i]);
        }
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    struct tree_node * root;

    if (argc < 2)   // elegxos an exoun dothei ligotera apo 1 orismata
    {
        fprintf(stderr, "Usage: %s<tree_file>\n", argv[0]);
        exit(1);
    }

    root = get_tree_from_file(argv[1]);   // argv[1] is a filename provided by the user, read input tree from the file, return root

    pid = fork();  /* Fork root of process tree */	
    if (pid < 0) { perror("main: fork"); exit(1); }

    // PROCESS ROOT
    if (pid == 0)
    {
        fork_procs(root);
        exit(1);
    }

    // BACK TO main()
    wait_for_ready_children(1);	// stamataei mexri na koimithoun ta paidia
    show_pstree(pid);  // emfanizei to dendro diergasion pou exei dhmiourgithei
    kill(pid, SIGCONT);	// i main() ksipnaei ti riza, afti ta paidia tis, klp mexri ta fylla
    wait(&status);   /* Wait for the root of the process tree to terminate */
    explain_wait_status(pid, status);
    return 0;
}
