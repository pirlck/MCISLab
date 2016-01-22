//
// 
// send data struct
//
#ifndef __WORK__H__
#define __WROK__H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/epoll.h>
#include <sys/mman.h>

#define PORT 10000                  //PORT
#define LISTEN_QUEUE_LEN 100        //listen size lens
#define THREAD_NUM  8               //pool size 
#define CONN_MAX  10                //max num connections
#define EPOLL_SIZE  50              //epoll-nums fd
#define FILENAME_MAXLEN   30        //max length filename
#define INT_SIZE    4               //int(TYPE) size 

// size of recv data
//#define RECVBUF_SIZE    4096        //4K
//#define RECVBUF_SIZE    32768       //32K
//#define RECVBUF_SIZE    131072      //128K
//#define RECVBUF_SIZE    262144      //256K
#define RECVBUF_SIZE    65536       //64K

//file info 
struct fileinfo{
    char filename[FILENAME_MAXLEN];     //file name
    int filesize;                       //file size
    int count;                          //size of block
    int bs;                             //size of block
};

//head info of the block
struct head{
    char filename[FILENAME_MAXLEN];     //file name
    int id;                             //file block id in array
    int offset;                         //offset of the block
    int bs;                             //size of the file block
};

//connection with client,share with all threads
struct conn{
    int info_fd;                      //send info socket
    char filename[FILENAME_MAXLEN];   //fielname 
    int filesize;                     //size file
    int bs;                           //size of block
    int count;                        //nums block
    int recvcount;                    //recv nums block，recv_count == count:recv over
    char *mbegin;                     //mmap begin addr
    int used;                         //conn flag,1:active
};

//thread paras
struct args{
    int fd;
    void (*recv_finfo)(int fd);
    void (*recv_fdata)(int fd);
};

//create test helo file
int create_file(char *filename, int size);

//create server sock fd
int server_init(int port);

//set file info non block
void set_fd_noblock(int fd);

//recv client file info
void recv_fileinfo(int sockfd);

//
void recv_filedata(int sockfd);

//work func
void * worker(void *argc);

#endif
