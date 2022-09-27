/*
 * mmap.c
 *
 * Examining the virtual memory of processes.
 *
 * Operating Systems course, CSLab, ECE, NTUA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>

#include "help.h"

#define RED     "\033[31m"
#define RESET   "\033[0m"


char *heap_private_buf;
char *heap_shared_buf;

char *file_shared_buf;

uint64_t buffer_size;


/*
 * Child process' entry point.
 */
void child(void)
{
        int i; 
	uint64_t pa, va;  // pa: physical address, va: virtual address

	/*  ***************** Step 7 - Child ******************  */
	if (0 != raise(SIGSTOP)) die("raise(SIGSTOP)");

	/* TODO  */
	show_maps();
	/* TODO  */

	/*  ***************** Step 8 - Child ******************  */
	if (0 != raise(SIGSTOP)) die("raise(SIGSTOP)");

	/* TODO  */
	va = (uint64_t) heap_private_buf;
	printf("virtual address (on child proc) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address (on child proc) is 0x%lx\n", pa);
	/* TODO  */

	/*  ***************** Step 9 - Child ******************  */
	if (0 != raise(SIGSTOP)) die("raise(SIGSTOP)");

	/* TODO  */
	for (i = 0; i < (int) buffer_size; i++) 
             heap_private_buf[i] = 1 ;
	va = (uint64_t) heap_private_buf;
	printf("virtual address (on child proc) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address (on child proc) is 0x%lx\n", pa);
	/* TODO  */

	/*  ***************** Step 10 - Child ******************  */
	if (0 != raise(SIGSTOP)) die("raise(SIGSTOP)");

	/* TODO  */
	for (i = 0; i < (int) buffer_size; i++) 
            heap_shared_buf[i] = 1 ;
	va = (uint64_t) heap_shared_buf;
	printf("virtual address (on child proc) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address (on child proc) is 0x%lx\n", pa);
	/* TODO  */

	/*  ***************** Step 11 - Child ******************  */
	if (0 != raise(SIGSTOP)) die("raise(SIGSTOP)");

	/* TODO  */
        mprotect(heap_shared_buf, buffer_size, PROT_READ);
	show_maps();
	printf(RED "\nDifference between parent and child: parent can read and write, child only reads:" RESET);
	printf(RED "\nparent: 7f0c2f499000-7f0c2f49a000 rw-s 00000000 00:04 2059970                    /dev/zero (deleted)\n" RESET);
	printf(RED "child:  7f0c2f499000-7f0c2f49a000 r--s 00000000 00:04 2059970                    /dev/zero (deleted)\n" RESET);
	/* TODO  */

	/*  ***************** Step 12 - Child ******************  */

	/* TODO  */
	/* TODO  */

}

/*
 * Parent process' entry point.
 */
void parent(pid_t child_pid)
{
	uint64_t pa, va;
	int status;

	/* Wait for the child to raise its first SIGSTOP. */
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");

	/*  ***************** Step 7 - Parent ******************  */
	/* Step 7: Print parent's and child's maps. What do you see? */

	printf(RED "\nStep 7: Print parent's and child's map.\n" RESET);
	press_enter();

	/* TODO  */
	show_maps();
	/* TODO  */

	if (-1 == kill(child_pid, SIGCONT)) die("kill"); if (-1 == waitpid(child_pid, &status, WUNTRACED)) die("waitpid");


	/*  ***************** Step 8 - Parent ******************  */
	/*  Step 8: Get the physical memory address for heap_private_buf.  */

	printf(RED "\nStep 8: Find the physical address of the private heap buffer (main) for both the parent and the child.\n" RESET);
	press_enter();

	/* TODO  */
	va = (uint64_t) heap_private_buf;
	printf("virtual address (on parent proc) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address (on parent proc) is 0x%lx\n", pa);
	/* TODO  */

	if (-1 == kill(child_pid, SIGCONT)) die("kill"); if (-1 == waitpid(child_pid, &status, WUNTRACED)) die("waitpid");


	/*  ***************** Step 9 - Parent ******************  */
	/*  Step 9: Write to heap_private_buf. What happened?  */

	printf(RED "\nStep 9: Write to the private buffer from the child and repeat step 8. What happened?\n" RESET);
	press_enter();

	/* TODO  */
	va = (uint64_t) heap_private_buf;
	printf("virtual address (on parent proc) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address (on parent proc) is 0x%lx\n", pa);
	/* TODO  */

	if (-1 == kill(child_pid, SIGCONT)) die("kill"); if (-1 == waitpid(child_pid, &status, WUNTRACED)) die("waitpid");


	/*  ***************** Step 10 - Parent ******************  */
	/*  Step 10: Get the physical memory address for heap_shared_buf.  */

	printf(RED "\nStep 10: Write to the shared heap buffer (main) from child\n" RESET);
        printf(RED "and get the physical address for both the parent and the child. What happened?\n" RESET);
	press_enter();

	/* TODO  */
	va = (uint64_t) heap_shared_buf;
	printf("virtual address (on parent proc) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address (on parent proc) is 0x%lx\n", pa);
	/* TODO  */

	if (-1 == kill(child_pid, SIGCONT)) die("kill"); if (-1 == waitpid(child_pid, &status, WUNTRACED)) die("waitpid");


	/*  ***************** Step 11 - Parent ******************  */
	/*  Step 11: Disable writing on the shared buffer for the child * (hint: mprotect(2)).  */

	printf(RED "\nStep 11: Disable writing on the shared buffer for the child. Verify through the maps for the parent and the child.\n" RESET);
	press_enter();

	/* TODO  */
	show_maps();
	/* TODO  */

	if (-1 == kill(child_pid, SIGCONT)) die("kill"); if (-1 == waitpid(child_pid, &status, 0)) die("waitpid");


	/*  ***************** Step 12 - Parent ******************  */
	/*  Step 12: Free all buffers for parent and child.  */

	/* TODO  */
        munmap(heap_private_buf, buffer_size);
        munmap(file_shared_buf, buffer_size);
        munmap(heap_shared_buf, buffer_size);
	printf(RED "\nStep 12: All buffers were freed. Program terminates\n" RESET);
	/* TODO  */

}

int main(void)
{
	int i;
	pid_t mypid, p;
	int fd = -1, newfd = -1;
	uint64_t pa, va;

	mypid = getpid();
	buffer_size = 1 * get_page_size();



	/*  ***************** Step 1 - main() ******************  */
	/*  Step 1: Print the virtual address space layout of this process.  */

	printf(RED "\nStep 1: Print the virtual address space map of this process [%d].\n" RESET, mypid);
	press_enter();

	/* TODO  */
	show_maps();   
	/* TODO  */


	
	/*  ***************** Step 2 - main() ******************  */
	/*  Step 2: Use mmap to allocate a buffer of 1 page and print the map * again. Store buffer in heap_private_buf.  */

	printf(RED "\nStep 2: Use mmap(2) to allocate a private buffer of size equal to 1 page and print the VM map again.\n" RESET);
	press_enter();

	/* TODO  */
	heap_private_buf = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, fd, 0);
	if (heap_private_buf == MAP_FAILED) printf("MMAP FAILED\n");
	show_maps();
	va = (uint64_t) heap_private_buf;
        printf("private buffer info: \n");
	show_va_info(va);
	/* TODO  */



	/*  ***************** Step 3 - main() ******************  */
	/*  Step 3: Find the physical address of the first page of your buffer in main memory. What do you see?  */

	printf(RED "\nStep 3: Find and print the physical address of the buffer in main memory. What do you see?\n" RESET);
	press_enter();

	/* TODO  */
	printf("virtual address (on main()) is 0x%lx\n", va);
	pa = get_physical_address(va);    /* get_physical_address() prints a message: VA[0x7f48fcdd5000] is not mapped */
	/* TODO  */


	
	/*  ***************** Step 4 - main() ******************  */
	/*  Step 4: Write zeros to the buffer and repeat Step 3.  */

	printf(RED "\nStep 4: Initialize your buffer with zeros and repeat Step 3. What happened?\n" RESET);
	press_enter();

	/* TODO  */
	for (i = 0; i < (int) buffer_size; ++i) heap_private_buf[i]=0;
	printf("virtual address (on main()) is 0x%lx\n", va);
	pa = get_physical_address(va);
	printf("Physical address for VA 0x%lx is 0x%lx\n", va, pa);   /* now VA[0x7f48fcdd5000] is in memory ! */
	/* TODO  */



	/*  ***************** Step 5 - main() ******************  */
	/*  Step 5: Use mmap(2) to map file.txt (memory-mapped files) and print its content. Use file_shared_buf.  */

	printf(RED "\nStep 5: Use mmap(2) to read and print file.txt. Print the new mapping information that has been created.\n" RESET);
	press_enter();

	/* TODO  */
	fd = open("file.txt", O_RDONLY);
	if(fd < 0) perror("Cannot open file.txt\n");
	file_shared_buf = mmap(NULL, buffer_size, PROT_READ, MAP_SHARED, fd, 0);
	if (file_shared_buf == MAP_FAILED) printf("MMAP FAILED\n");
	fprintf(stdout, file_shared_buf);
	va = (uint64_t) file_shared_buf;
        printf("private buffer for file info: \n");
	show_va_info(va);
	pa = get_physical_address(va);
	printf("Physical address is 0x%lx\n", pa);
	/* TODO  */



	/*  ***************** Step 6 - main() ******************  */
	/*  Step 6: Use mmap(2) to allocate a shared buffer of 1 page. Use heap_shared_buf.  */

	printf(RED "\nStep 6: Use mmap(2) to allocate a shared buffer of size equal to 1 page." RESET);
        printf(RED "\nInitialize the buffer and print the new mapping information that has been created.\n" RESET);
	press_enter();

	/* TODO  */
	heap_shared_buf = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, newfd, 0);
	if (heap_shared_buf == MAP_FAILED) printf("MMAP FAILED\n");
	for (i = 0; i < (int) buffer_size; ++i) heap_shared_buf[i]=0;
	va = (uint64_t) heap_shared_buf;
        printf("shared buffer info: \n");
	show_va_info(va);
	pa = get_physical_address(va);
	printf("Physical address is 0x%lx\n", pa);
	/* TODO  */




	
	/*Fork is called*/
	p = fork();
	if (p < 0)  die("fork");
	if (p == 0)  /* i am the child */
        {
		child();
		return 0;
	}

	parent(p);   /* parent knows p, child's PID */

	/* close(fd); */
	if (-1 == close(fd))
		perror("close");
	return 0;
}

