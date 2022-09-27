/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

/*TODO header file for m(un)map*/

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

int n;
sem_t *nsem;

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
         * x and y traverse the complex plane.
         */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x += xstep, n++)
	{

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point = '@';
	char newline = '\n';

	for (i = 0; i < x_chars; i++)
	{
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1)
		{
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1)
	{
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}
}

void compute_and_output_mandel_line(int fd, int line, int i)
{
	/*
         * A temporary array, used to hold color values for the line being drawn
         */
	int color_val[x_chars];

	compute_mandel_line(line, color_val);

	sem_wait(&nsem[i]);

	output_mandel_line(fd, color_val);
	
	if (i + 1 < n)
	{
		sem_post(&nsem[i + 1]);
	}
	else
	{
		sem_post(&nsem[0]);
	}
}

/*
 * Create a shared memory area, usable by all descendants of the calling
 * process.
 */
void *create_shared_memory_area(unsigned int numbytes)
{
	int pages;
	void *addr;

	// Our memory buffer will be readable and writable:
	int protection = PROT_READ | PROT_WRITE;

	// The buffer will be shared (meaning other processes can access it), but
	// anonymous (meaning third-party processes cannot obtain an address for it),
	// so only this process and its children will be able to use it:
	int visibility = MAP_SHARED | MAP_ANONYMOUS;

	if (numbytes == 0)
	{
		fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
		exit(1);
	}

	/*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
	pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

	/* Create a shared, anonymous mapping for this number of pages */

	addr = mmap(NULL, pages * sysconf(_SC_PAGE_SIZE), protection, visibility, -1, 0);

	return addr;
}

void destroy_shared_memory_area(void *addr, unsigned int numbytes)
{
	int pages;

	if (numbytes == 0)
	{
		fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
		exit(1);
	}

	/*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
	pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

	if (munmap(addr, pages * sysconf(_SC_PAGE_SIZE)) == -1)
	{
		perror("destroy_shared_memory_area: munmap failed");
		exit(1);
	}
}

void make_mandel(int i)
{
	xstep = (xmax - xmin) / x_chars;
	ystep = (ymax - ymin) / y_chars;
	/*
         * draw the Mandelbrot Set, one line at a time.
         * Output is sent to file descriptor '1', i.e., standard output.
         */
	int line;
	for (line = i; line < y_chars; line += n)
	{
		compute_and_output_mandel_line(1, line, i);
	}

	reset_xterm_color(1);
}

void fork_procs()
{
	pid_t pid[n];
	int status[n];
	int i;

	for (i = 0; i < n; i++)
	{
		pid[i] = fork();
		if (pid[i] < 0)
		{
			perror("main: fork");
			exit(1);
		}
		else if (pid[i] == 0)
		{
			make_mandel(i);
			exit(1);
		}
	}

	for (i = 0; i < n; i++)
	{
		pid[i] = wait(&status[i]);
	}
	exit(0);
}

void sig_handler(int signum)
{
	printf("\nCaught SIGINT\n");
	set_xterm_color(1, 7);
	exit(1);
}

int main(int argc, char *argv[])
{
	/* HANDLE SIGNAL */
	signal(SIGINT, sig_handler);

	/* MAKE n = NPROCS */
	n = 1;
	if (argc < 2)
	{
		printf("Usage: ./mandel [NPROCS]\nSwitching to default. Using 1 process.\n");
	}
	else
	{
		n = atoi(argv[1]);
	}

	/* CREATE & INITIALIZE SEMAPHORES */
	int j;
	nsem = create_shared_memory_area(sizeof(sem_t) * n);
	sem_init(&nsem[0], 1, 1);
	for (j = 1; j < n; j++)
		sem_init(&nsem[j], 1, 0);

	/* FORK FOR [NPROCS] */
	pid_t pid[n];
	int status[n];
	int i;

	for (i = 0; i < n; i++)
	{
		pid[i] = fork();
		if (pid[i] < 0)
		{
			perror("main: fork");
			exit(1);
		}
		else if (pid[i] == 0)
		{
			make_mandel(i);
			exit(1);
		}
	}

	/* WAIT FOR PROCESSES TO EXIT */
	for (i = 0; i < n; i++)
	{
		pid[i] = wait(&status[i]);
	}


	/* DESTROY SHARED MEMORY AREA */
	destroy_shared_memory_area(&nsem[0], sizeof(sem_t) * n);

	return 0;
}

