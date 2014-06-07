#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
struct time_lrc/* 存放歌曲行开始播放的时间和歌词的位置*/
{
	int t;
	char *lrc;
}music[200];
void sig_int(int sig){
	printf("\033[0m");
	exit(1) ;
}
int main(int argc ,char *argv[])
{
	if(argc!=2)
	{
		printf("Error: Usage ./%s <The *.mp3 you want to play>",argv[0]);
		printf("wrong in number of parameter \nthere one parameter need\n");
		exit(1);
	}
	signal(SIGINT,sig_int);
	char music_you_like[100];
	strcpy(music_you_like,argv[1]);
	music_you_like[99]=0;
	int time_front;
	printf("请输入您想要的歌词提前时间（ms）不想提前直接输入0即可: ");
	scanf("%d",&time_front);
	pid_t child_num;
	child_num=fork();
	if(child_num==0)
	{
		char cmd[100]={0};
		char *cmd1="mpg321 ";
		char *cmd2=" 2>&1 /dev/null";
		strcat(cmd,cmd1);
		strcat(cmd,"\"");
		strcat(cmd,music_you_like);
		strcat(cmd,"\"");
		strcat(cmd,cmd2);
		FILE *fp;
		if(fp=popen(cmd,"r") == NULL)
		{
			printf("error in play %s\n ",cmd);
			exit(1);
		}
		pclose(fp);
		exit(0);
	}
	else
	{
		//printf("               为了保证同步，没让用户输入 \n");
		printf("\033[31m");
		printf("      *     * ***** *      **** ****** *    * *****\n");
		printf("      *  *  * *     *     *     *    * **  ** *    \n");
		printf("      * * * * ***** *     *     *    * * ** * *****\n");
		printf("      **   ** *     *     *     *    * *    * *    \n");
		printf("      *     * ***** *****  **** ****** *    * *****\n");
		printf("\033[0m");
		int fd;
		int num,i,j,k;
		int all_time[100];
		int time_long,len_lrc;
		char var[15];
		int min,sec,macro;
		char name[50]={0};//="music.lrc";
		k=strlen(music_you_like);
		music_you_like[k-4]=0;
		strcat(music_you_like,".lrc");
		strcpy(name,music_you_like);
		char buffer[10000];
		fd=open(name,O_RDONLY,10175);
		if(fd==-1)
		{
			printf("%s文件打开错误 ，请检查该文件是否存在",music_you_like);
			exit(1);
		}
		num=read(fd,buffer,5000);
		buffer[num]=0;
		i=0;
		j=0;
		while(buffer[i])
		{
			if(buffer[i]=='\n')
				buffer[i]=0;
			if(buffer[i]=='[')
			{
				strncpy(var,buffer+i,10);
				var[10]=0;
				sscanf(var,"[%d:%d.%d]",&min,&sec,&macro);
				//printf("%d %d %d\n",min,sec,macro);
				music[j].t=min*1000*60+sec*1000+macro*10;
				music[j].lrc=buffer+i+10;
				j++;
			}
			i++;
		}
		for(i=0; ;i++)
			if(buffer[i]=='[')
			{ printf("     版本 ：[%s     ",buffer+i+5);break;}
		i++;
		for(i; ;i++)
			if(buffer[i]=='[')
			{ printf("     作者 ：[%s     ",buffer+i+4);break;}
		i++;
		for(i; ;i++)
			if(buffer[i]=='[')
			{ printf("     曲名 ：[%s     ",buffer+i+4);break;}
		printf("\n");
		for(i=0;i<j;i++)
			if(music[i].t)
				break;
		if(time_front < 20000)//不能超时间太长了20秒上限制
			music[i].t-=time_front;//////////////////////////////////////////////让字幕稍稍提前一点
		else
			printf("抱歉，您的歌词不能超前显示这么长的时间\n");
		if(music[i].t<0)//暂停到开始有字幕的时候
			music[i].t=0;
		usleep(music[i].t*1000);
		/*for( ;i<j;i++)
		  {
		  if(music[i].t)
		  printf("time is %d   lrc is %s\n",music[i].t,music[i].lrc);
		  }*/
		for(i=0;i<j;i++)
			if(music[i].t)
				break;
		//printf("%s",music[10].lrc);
		for(i;i<j-1;i++)
		{
			time_long=music[i+1].t-music[i].t;
			len_lrc=strlen(music[i].lrc);
			if(len_lrc==0)//防止长度为0如果长度是0，那么就将其变成1 这样就OK 了
				len_lrc=1;
			time_long/=len_lrc;
			printf("\033[32m");
			printf("             ");
			printf("%s",music[i].lrc);
			putchar(13);
			printf("             ");

			for(k=0;k<len_lrc;k++)
			{
				usleep(time_long*700);//本来应该是*1000的，由于歌曲尾部叫长先延迟700
				/*
				 * 下面的注释如果去掉那么就能随机彩色显示歌词
				 */
				/*switch(rand()%6) 
				  {
				  case 0:printf("\033[31m"); break;
				  case 1:printf("\033[32m"); break;
				  case 2:printf("\033[33m"); break;
				  case 3:printf("\033[34m"); break;
				  case 4:printf("\033[35m"); break;
				  case 5:printf("\033[36m"); break;
				  }*/
				printf("\033[33m");
				putchar(' ');
				putchar('\b');
				printf("%c",music[i].lrc[k]);
				fflush(stdout);
			}
			usleep(time_long*len_lrc*300);//补上开始的一千
			printf("\n");
		}
		printf("             ");
		len_lrc=strlen(music[j-1].lrc);
		for(k=0;k<len_lrc;k++)
		{
			usleep(time_long*700);//到这里反正快结束了，后面的就不管了！
			printf("%c",music[i].lrc[k]);
			fflush(stdout);
		}
		printf("\033[0m");
		close(fd);
		return 0;
	}
}

