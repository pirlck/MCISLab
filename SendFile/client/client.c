//
//
//  client: test send big file to the server 
//
#include "tpool.h"
#include "work.h"

int port=PORT;	//default PORT
char *mbegin;	//map addr

int main(int argc, char **argv)
{
	//initial the client
	if (argc>1)
    port = atoi(argv[1]);
    int info_fd = client_init(SERVER_IP);

    char filename[FILENAME_MAXLEN] = {0};
    printf("BLOCKSIZE=  %d\n",BLOCKSIZE);
    printf("Input filename : ");
    scanf("%s",filename);
    int fd=0;
    if((fd = open(filename, O_RDWR)) == -1 )
	{
		printf("open erro ！\n");
		exit(-1);
	}	
	
	/*Timer*/
    printf("Timer start!\n");
	time_t t_start, t_end;
	t_start=time(NULL);

	//send file info
    struct stat filestat;
	fstat(fd ,&filestat);
	int last_bs=0;
    struct fileinfo finfo;
	send_fileinfo(info_fd, filename, &filestat, &finfo, &last_bs);

	//
    char id_buf[INT_SIZE] = {0};
    int n=0;
    for(n=0; n<INT_SIZE; n++){
        read(info_fd, &id_buf[n], 1);
    }
    int freeid = *((int *)id_buf);
    printf("freeid = %d\n", freeid);

	//map .
    mbegin = (char *)mmap(NULL, filestat.st_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd , 0);
    close(fd);

    //add task to the list 
    int j=0, num=finfo.count, offset=0;
	pthread_t pid[num];
	memset(pid, 0, num*sizeof(pthread_t));
	int head_len = sizeof(struct head);
	// file can be slipt into blocks
    if(last_bs == 0){
        for(j=0; j<num; j++){
            struct head * p_fhead = new_fb_head(filename, freeid, &offset);
			if (pthread_create(&pid[j], NULL, send_filedata, (void *)p_fhead) != 0){
            printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
            exit(-1);
        	}
        }
    }
	//file cannot slipt into blocks
    else{
        for(j=0; j<num-1; j++){
            struct head * p_fhead = new_fb_head(filename, freeid, &offset);
			if (pthread_create(&pid[j], NULL, send_filedata, (void *)p_fhead) != 0){
            	printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
            	exit(-1);
        	}
        }
        //the last block
        struct head * p_fhead = (struct head *)malloc(head_len);
		bzero(p_fhead, head_len);
        strcpy(p_fhead->filename, filename);
        p_fhead->id = freeid;
        p_fhead->offset = offset;
        p_fhead->bs = last_bs;

		if (pthread_create(&pid[j], NULL, send_filedata, (void *)p_fhead) != 0){
            printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
            exit(-1);
        }
    }

	//join all the pthread
    for (j = 0; j < num; ++j) {
        pthread_join(pid[j], NULL);
    }

	//calculate the time for send file 
	t_end=time(NULL);
	printf("Master prosess exit!\n");
	printf("TIME:%.0fs\n",difftime(t_end,t_start));

    return 0;
}

