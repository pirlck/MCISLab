//
// file about send file data to server
//
//
#ifndef __SEND_DATA_WORK__
#define __SEND_DATA_WORK__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <time.h>


#define SERVER_IP   "127.0.0.1"     //server IP
#define PORT 10000                  //Server Port
#define THREAD_NUM  4               //size of pool
#define FILENAME_MAXLEN   30        //the max lenth of file
#define INT_SIZE    4               //

//#define SEND_SIZE    32768       	//32K
#define SEND_SIZE    65536       	//64K
//#define SEND_SIZE	131072			//128K
//#define SEND_SIZE	262144			//256K

//#define BLOCKSIZE   134217728		//128M
//#define BLOCKSIZE   268435456		//256M
#define BLOCKSIZE   536870912		//512M
//#define BLOCKSIZE	1073741824		//1G

//file info
struct fileinfo{
    char filename[FILENAME_MAXLEN];     //file name
    int filesize;                       //size of file
    int count;                          //num of block
    int bs;                             //real size of block
};

//head info of block
struct head{
    char filename[FILENAME_MAXLEN];     //file name
    int id;                             //the file id of file block
    int offset;                         //the offset of the file block
    int bs;                             //the size of file block
};

//create hole file for test
int create_file(char *filename, int size);

//set fd non block
void set_fd_noblock(int fd);

//create sock fd for client
int client_init(char *ip);

//send the file info
/*last_bs==0:所有分块都是标准分块；flag>0:最后一个分块不是标准分块，last_bs即为最后一块大小*/
void send_file_info(int sock_fd                  //sock fd
                   , char *fname                //filename
                   , struct stat* p_fstat       //info of file struct
                   , struct fileinfo *p_finfo   //return file info 
                   , int *flag);                 //if the last block exactly the normal

//send file data block
void * send_file_data(void *args);

//generate file head 
struct head * new_fb_head(char *filename, int freeid, int *offset);

#endif
