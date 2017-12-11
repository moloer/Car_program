#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h> //copy_to_user, copy_from_user
#include <linux/io.h> //ioremap,writeb,readb等
MODULE_LICENSE("GPL");
dev_t devno;
struct class *cls;
//将引脚地址置空
volatile unsigned int *GPH2CON=NULL;
volatile unsigned int *GPH2DAT=NULL;
volatile unsigned int *GPH3CON=NULL;
volatile unsigned int *GPH3DAT=NULL;

//准备一个open函数,用来服务open请求
int myopen(struct inode *ino,struct file *file)
{
    printk(KERN_CRIT"fake file was open!\n");
    GPH2CON=ioremap(0xE0200C40,4);//GPH2CON地址重映射
    GPH2DAT=ioremap(0xE0200C44,4);
    GPH3CON=ioremap(0xE0200C60,4);//GPH3CON地址重映射
    GPH3DAT=ioremap(0xE0200C64,4);
    writel(0x1111 ,GPH2CON);//GPH2CON第1、2、3、4号引脚设置成输出功能
    writel(0x0011 ,GPH3CON);//GPH3CON第1、2号引脚设置成输入功能，第3、4号引脚设置成输出功能
    return 0;
}
// close函数
int myclose(struct inode *ino, struct file *file)
{
    printk(KERN_CRIT"fake file was close!\n");
    iounmap(GPH2CON);//取消映射
    iounmap(GPH2DAT);
    iounmap(GPH3CON);
    iounmap(GPH3DAT);
    return 0;
}
//read函数,从内核空间将数据拷贝到用户空间
ssize_t myread(struct file *file,char __user *buffer, size_t len, loff_t *off)
{
    int num=2017;
    ssize_t re=copy_to_user(buffer,&num,sizeof num);
    return re;
}
//write函数,从用户空间将数据拷贝到内核空间
ssize_t mywrite(struct file *file,const char __user *buffer,size_t len,loff_t *off)
{
    unsigned char ch;
    ssize_t re=copy_from_user(&ch,buffer,len);
    //前进
    if(ch == 'w')
    {
    writel(0x0,GPH2DAT);
    //红外避障模块暂且不加入
    //遇障停止
    // if((GPH3DAT & (0x1 << 2)) != 0x1 << 2)
	// {
    //     writel(0x0,GPH2DAT);
    // }
    writel(0x5,GPH2DAT);
    }
    //后退
    if(ch == 's')
    {
    writel(0x0,GPH2DAT);
	writel(0xA,GPH2DAT);
    }
    //前左拐弯
    if(ch == 'q')
    {
    writel(0x0,GPH2DAT);
    writel(0x1,GPH2DAT);
    }
    //前右拐弯
    if(ch == 'e')
    {
    writel(0x0,GPH2DAT);
    writel(0x4,GPH2DAT);
    }
    //后左拐弯
    if(ch == 'a')
    {
    writel(0x0,GPH2DAT);
	writel(0x2,GPH2DAT);
    }
    //后右拐弯
    if(ch == 'd')
    {
    writel(0x0,GPH2DAT);
    writel(0x8,GPH2DAT);
    }
	//停止
    if(ch == 'f')
    {
    writel(0x0,GPH2DAT);
    }
    return re;
}

//准备好文件操作结构体并初始化
struct file_operations fops={
    .owner=THIS_MODULE,
    .open=myopen,//将自定义的open服务函数交给它
    .release=myclose,
    .read=myread,
    .write=mywrite
};


int __init mydriverinit(void)
{
    alloc_chrdev_region(&devno,10,1,"mydriver");
    /* 注册驱动 */
    register_chrdev(MAJOR(devno),"mydriver",&fops);

    cls=class_create(THIS_MODULE,"myclass");
    device_create(cls,NULL,devno,NULL,"mydevice");
    return 0;
}
module_init(mydriverinit);
void __exit mydriverexit(void)
{
    device_destroy(cls,devno);
    class_destroy(cls);
    unregister_chrdev(MAJOR(devno),"mydriver");
    unregister_chrdev_region(devno,1);
}
module_exit(mydriverexit);
