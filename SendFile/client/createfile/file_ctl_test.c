//
// by cl-k
// a simple program to create hole file
//
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "file_ctl.h"


int main()
{
	int i, fd, size=1024*1024*512;

	create_file("hole_file", size);
	if((fd = open("hole_file", O_RDWR)) == -1 )
	{
		printf("open erro\n");
		exit(-1);
	}
	struct stat filestat;
	fstat(fd ,&filestat);
	printf("file size = %ld\n",filestat.st_size);

	return 0;
}



