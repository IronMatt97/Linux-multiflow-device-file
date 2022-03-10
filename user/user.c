#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>

int i;
char buff[4096];

#define DATA "Init"
#define SIZE strlen(DATA)

void * the_thread(void* path)
{
	int fd;
	char* device = (char*)path;
	
	sleep(1);
	printf("opening device %s\n",device);
	fd = open(device,O_RDWR|O_APPEND);
	if(fd == -1)
	{
		printf("open error on device %s\n",device);
		return NULL;
	}
	printf("Init: device %s successfully opened\n",device);
	
	//Thread activity
	ioctl(fd,0);
	ioctl(fd,4);
	write(fd,DATA,SIZE);
	close(fd);
	fd = open(device,O_RDWR|O_APPEND);
	ioctl(fd,1);
	ioctl(fd,4);
	write(fd,DATA,SIZE);
	close(fd);
	printf("Init: device %s successfully closed\n",device);
	return NULL;
}

void * test_lo_thread_w(void* path)
{
	char* device = (char*)path;
	int fd = open(device,O_RDWR|O_APPEND);
	sleep(1);
	printf("LowPrioW: opening device %s\n",device);
	if(fd == -1)
	{
		printf("open error on device %s\n",device);
		return NULL;
	}
	printf("LowPrioW: device %s successfully opened\n",device);
	ioctl(fd,0);
	ioctl(fd,3);
	char *message = "LowPrio";
	write(fd,message,strlen(message));
	close(fd);
	printf("LowPrioW: device %s successfully closed\n",device);
	return NULL;
}
void * test_lo_thread_r(void* path)
{
	char* device = (char*)path;
	int fd = open(device,O_RDWR);
	sleep(1);
	printf("LowPrioR: opening device %s\n",device);
	if(fd == -1)
	{
		printf("open error on device %s\n",device);
		return NULL;
	}
	printf("LowPrioR: device %s successfully opened\n",device);
	ioctl(fd,0);
	ioctl(fd,3);
	char *message = malloc(2);
	read(fd,message,2);
	printf("LowPrioR: è stato letto %s\n",message);
	close(fd);
	printf("LowPrioR: device %s successfully closed\n",device);
	return NULL;
}

void * test_hi_thread_w(void* path)
{
	char* device = (char*)path;
	int fd = open(device,O_RDWR|O_APPEND);
	sleep(1);
	printf("HighPrioW: opening device %s\n",device);
	if(fd == -1)
	{
		printf("open error on device %s\n",device);
		return NULL;
	}
	printf("HighPrioW: device %s successfully opened\n",device);
	ioctl(fd,1);
	ioctl(fd,3);
	char *message = "HighPrio";
	write(fd,message,strlen(message));
	close(fd);
	printf("HighPrioW: device %s successfully closed\n",device);
	return NULL;
}

void * test_hi_thread_r(void* path)
{
	char* device = (char*)path;
	int fd = open(device,O_RDWR);
	sleep(1);
	printf("HighPrioR: opening device %s\n",device);
	if(fd == -1)
	{
		printf("open error on device %s\n",device);
		return NULL;
	}
	printf("HighPrioR: device %s successfully opened\n",device);
	ioctl(fd,1);
	ioctl(fd,3);
	char *message = malloc(2);
	read(fd,message,2);
	printf("HighPrioR: è stato letto %s\n",message);
	close(fd);
	printf("HighPrioR: device %s successfully closed\n",device);
	return NULL;
}

int main(int argc, char** argv)
{
	int ret;
	int major = strtol(argv[2],NULL,10);
    int minors = strtol(argv[3],NULL,10);
    char *path = argv[1];
    pthread_t tid;
	char **minors_list = malloc(minors*sizeof(char*));

    if(argc<4)
	{
		printf("useg: prog pathname major minors\n");
		return -1;
    }
     
	printf("creating %d minors for device %s with major %d\n",minors,path,major);
	for(i=0;i<minors;i++)
	{
		sprintf(buff,"mknod %s%d c %d %i\n",path,i,major,i);
		system(buff);
		sprintf(buff,"%s%d",path,i);
		minors_list[i] = malloc(32);
		strcpy(minors_list[i],buff);
		pthread_create(&tid,NULL,the_thread,strdup(buff));
    }

	printf("Minors list:\n");
	for(i=0;i<minors;i++)
	{
		printf("%s\n",minors_list[i]);
    }
	sleep(1);
	//Test implementation
	//Per ogni file spawno due thread ad alta priorità e due a bassa priorità, dove ogni volta uno scrive ed uno legge
	for(i=0;i<minors;i++)
	{
		//thread_create(&tid,NULL,test_hi_thread_w,strdup(minors_list[i]));
		//pthread_create(&tid,NULL,test_hi_thread_w,strdup(minors_list[i]));
		//pthread_create(&tid,NULL,test_lo_thread_w,strdup(minors_list[i]));
		//pthread_create(&tid,NULL,test_lo_thread_w,strdup(minors_list[i]));
		//sleep(1);
		//pthread_create(&tid,NULL,test_lo_thread_r,strdup(minors_list[i]));
		//pthread_create(&tid,NULL,test_lo_thread_r,strdup(minors_list[i]));
		//sleep(1);
		//pthread_create(&tid,NULL,test_hi_thread_r,strdup(minors_list[i]));
		//pthread_create(&tid,NULL,test_hi_thread_r,strdup(minors_list[i]));
		
		//pthread_create(&tid,NULL,test_hi_thread_r,strdup(minors_list[i]));
    }

    pause();
    return 0;
}