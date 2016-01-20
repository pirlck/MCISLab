//
// by cl-k
// a simple program to create hole file
//

#ifndef __FILE_CTL__
#define __FILE_CTL__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


//
// brief     创建指定大小的文件
// para      filename	文件名；size 文件大小，单位字节
// return    0: 成功 其他: 失败  
//
int create_file(char *filename, int size)
{
	int fd = open(filename, O_RDWR | O_CREAT);
	
	fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	int pos = lseek(fd, size-1, SEEK_SET);
	
	write(fd, "", 1);
	close(fd);
	return 0;
}

#endif

