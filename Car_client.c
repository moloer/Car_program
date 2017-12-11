/*
ARM开发板Linux系统客户端代码
*/
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, const char *argv[])
{
	if(argc < 3)
	{
		printf("请附上ip和端口,空格隔开\n");
		exit(-1);
	}

	/*1、创建套接字*/
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}	  

	/*2、连接服务器*/
	struct sockaddr_in sockin;
	bzero(&sockin, sizeof(sockin));
	sockin.sin_family = AF_INET;
	sockin.sin_port = htons(atoi(argv[2]));//端口
	inet_pton(AF_INET, argv[1], &sockin.sin_addr);
	if(connect(fd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
	{
		perror("connect");
		exit(EXIT_FAILURE);
	}

	/*3、打开驱动文件*/
	int fd_drivers = open("/dev/mydevice", O_RDWR);
	if(fd_drivers < 0)
	{
		perror("open");
		exit(-1);
	}

	/*读写操作*/
	while(1)
	{	
		char ch = 0;        
		read(fd, &ch, 1);//从服务端读取一个字符的内容到fd里
		if(ch == ' ')    //当读到'空格'结束循环
		{
			break;
		}
		write(fd_drivers, &ch, 1);//把这个内容写到驱动文件中
		printf("\n");
	}
	close(fd);	
	close(fd_drivers);
}
