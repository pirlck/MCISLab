/*************************************************************************
    > File Name: create_file.c
    > Author: cl-k
    > Mail: kuangcailiang07@gmail.com
    > Created Time: 2016年01月19日 星期二 9时32分21秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "file_ctl.h"

//create hole file
int create_file(char *filename, int size)
{
	int fd = open(filename, O_RDWR | O_CREAT);
	fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	int pos = lseek(fd, size-1, SEEK_SET);
	write(fd, "", 1);
	close(fd);
	return 0;
}

int main()
{
	int i, fd, size=1024*1024*512;

	create_file("123a", size);
	if((fd = open("123a", O_RDWR)) == -1 )
	{
		printf("open erro\n");
		exit(-1);
	}
	struct stat filestat;
	fstat(fd ,&filestat);
	printf("file size = %ld\n",filestat.st_size);

	return 0;
}


