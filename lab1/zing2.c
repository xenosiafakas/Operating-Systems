#include <stdio.h>
#include <unistd.h>
void zing(char *name)
{
	name=getlogin();
	printf ("zing2 here:  %s\n", name);
};
