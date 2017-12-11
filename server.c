/*
服务器程序(采用TCP协议)
*/
#include <unistd.h>
#include <sys/socket.h>  //套接字相关
#include <arpa/inet.h> //因特网地址结构体 struct sockaddr_in定义
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(void)
{
	/*1、创建套接字*/
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}	

    /*2、将套接字和地址进行绑定*/
	/*2-1 先准备地址结构体*/
	struct sockaddr_in sockin;
	bzero(&sockin, sizeof(sockin));
	sockin.sin_family = AF_INET;//地址族
	sockin.sin_port = htons(2020);//本机监听端口
	sockin.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY的值0

	/*2-2 绑定 */
	if(bind(listen_fd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/*3、开放套接字,开始监听*/
	if(listen(listen_fd, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/*4、从队列中取一个连接, 进行通信*/
	struct sockaddr_in clientaddr;
	bzero(&clientaddr, sizeof(clientaddr));
	size_t len = sizeof(clientaddr);//长度必须要有初始值
	
	int fd_car = accept(listen_fd, (struct sockaddr *)&clientaddr, &len);	//fd_car为小车客户端
	printf("小车客户端已连接\n");

	if(fd_car < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	//第一个必须由小车客户端连接进入
	while(1)//给PC端可连接多个
	{
		int fd_pc = accept(listen_fd, (struct sockaddr *)&clientaddr, &len);	//fd_pc为PC客户端
		printf("PC客户端已连接\n");

		if(fd_pc < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		while(1)//读写操作
		{
			char ch = 0;
			read(fd_pc, &ch, 1);//读取PC端发送过来的内容
			if(ch == ' ')
			{
				break;
			}   
			write(fd_car, &ch, 1);//将PC端的内容发给小车客户端
		}
		close(fd_pc);//PC客户端端退出连接
	}
	close(fd_car);//小车客户端
	close(listen_fd);
}
