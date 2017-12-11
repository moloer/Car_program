//PC端
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <termios.h>
#include <term.h>
#include <curses.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

static struct termios initial_settings, new_settings;
static int peek_character = -1;
void init_keyboard();
void close_keyboard();
int kbhit();
int readch();

void init_keyboard()
{
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}
void close_keyboard()
{
    tcsetattr(0, TCSANOW, &initial_settings);
}
//主函数以外的函数均是无阻塞输入功能函数
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
	sockin.sin_port = htons(atoi(argv[2]));//2020端口
	inet_pton(AF_INET, argv[1], &sockin.sin_addr);
	if(connect(fd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
	{
		perror("connect");
		exit(EXIT_FAILURE);
	}
	
	/*读写操作*/
	int ch = 0;
    init_keyboard();//初始化无阻塞键盘功能
    while(ch != ' ')
	{
        usleep(10000);  //缓冲一波
        if(kbhit())
		{
        	ch = readch();
        	printf("you hit %c\n",ch);     //输出键盘输入的值
        	write(fd,&ch,1);    //将数据写入文件
        }
    }
    close_keyboard();//关闭无阻塞键盘功能
    close(fd);
    exit(0);	
}

int kbhit()
{
    char ch;
    int nread;
    if(peek_character != -1)
        return 1;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &new_settings);
    if(nread == 1) {
      peek_character = ch;
      return 1;
    }
return 0;
}

int readch()
{
    char ch;
    if(peek_character != -1) {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    read(0,&ch,1);
    return ch;
}
