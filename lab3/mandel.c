#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

typedef struct
{
	pthread_t tid;
	int line;
	sem_t mutex;
} newThread_t;

newThread_t *thread;

int y_chars = 50;
int x_chars = 90;

double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;
	
double xstep;
double ystep;

int NTHREADS; 

void compute_mandel_line(int line, int color_val[])
{
    double x, y;
    int n;
    int val;
    y = ymax - ystep * line;
    for (x = xmin, n = 0; n < x_chars; x += xstep, n++)
    {
        val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
        if (val > 255)
            val = 255;
        val = xterm_color(val);
        color_val[n] = val;
    }
}

void output_mandel_line(int fd, int color_val[])
{
    int i;
    char point = '@';
    char newline = '\n';
    for (i = 0; i < x_chars; i++)
    {
        set_xterm_color(fd, color_val[i]);
        if (write(fd, &point, 1) != 1)
        {
            perror("compute_and_output_mandel_line: write point");
            exit(1);
        }
    }
    if (write(fd, &newline, 1) != 1)
    {
        perror("compute_and_output_mandel_line: write newline");
        exit(1);
    }
}

void *compute_and_output_mandel_line(void *arg)
{
    int line = *(int*) arg;
    int index;
    int color_val[x_chars];
    for (index = line; index < y_chars; index += NTHREADS)
    {
        compute_mandel_line(index, color_val);
        sem_wait(&thread[(index % NTHREADS)].mutex);
        output_mandel_line(STDOUT_FILENO, color_val);
        sem_post(&thread[((index % NTHREADS) + 1) % NTHREADS].mutex);
    }
    return NULL;
}

void unexpectedSignal(int sign)
{
    signal(sign, SIG_IGN);
    reset_xterm_color(1);
    exit(1);
}

int main(void)
{
    int line;
    signal(SIGINT, unexpectedSignal);
    xstep = (xmax - xmin) / x_chars;
    ystep = (ymax - ymin) / y_chars;
    printf("Enter Number of Threads: ");
    scanf("%d", &NTHREADS);
    if (NTHREADS < 1 || NTHREADS > y_chars)
    {
        printf("Input is not valid\n");
        return 0;
    }
    thread = (newThread_t*) malloc(NTHREADS* sizeof(newThread_t));
    if (thread == NULL)
    {
        perror("");
        exit(1);
    }
    if ((sem_init(&thread[0].mutex, 0, 1)) == -1)
    {
    	// The sem_init function returns 0 on success and -1 on error
        perror("");
        exit(1);
    }
    for (line = 1; line < NTHREADS; line++)
    {
        if ((sem_init(&thread[line].mutex, 0, 0)) == -1)
        {
            perror("");
            exit(1);
        }
    }
    for (line = 0; line < NTHREADS; line++)
    {
        thread[line].line = line;
        if ((pthread_create(&thread[line].tid, NULL, compute_and_output_mandel_line, &thread[line].line)) != 0)
        {
            perror("");
            exit(1);
        }
    }
    for (line = 0; line < NTHREADS; line++)
    {
        if ((pthread_join(thread[line].tid, NULL)) != 0)
        {
            perror("");
            exit(1);
        }
    }
    for (line = 0; line < NTHREADS; line++)
    {
        sem_destroy(&thread[line].mutex);
    }
    free(thread);
    reset_xterm_color(1);
    return 0;
}


