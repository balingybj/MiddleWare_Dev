
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <sys/klog.h>


#include "log_interface.h"


#define LOG_TRUE 1
#define LOG_FALSE 0


#define LOG_SUCCESS 0
#define LOG_FAILURE 1



#define LOG_ASSERT(condition)	do\
{ \
	if(condition)	{} 	\
	else 	\
	{	\
		printf("Assert failed,file:%s func:%s line:%d", __FILE__,  __func__, (int)__LINE__);    \
		abort();    \
	}	\
}while(0)


#define LOG_PRINT(format, ...) do \
{\
	printf("%s %d : "format"\n", __func__, __LINE__, ##__VA_ARGS__);\
}while(0)


#define LOG_VERSION "v2018.04"


typedef struct _LOG_CONFIG_
{
	int enablePrint;
	int logOutFormat;
	int maxSaveZipCounts;
	int maxCacheNum;
	int autoRefreshTime;
	int maxFileSize;
	char logName[20];
	char LogPath[40];
	int strValidLen;
	int maxSupportApp;
} ST_LOG_CONFIG_;


/* 日志模块全局数据结构 */
typedef struct 
{
	int maxCacheNum;	//最大缓存数量
	int localPrint;	//是否本地打印
	int maxSaveZipCounts;	//压缩文件最大个数
	int autoRefreshTime;   	//日志刷新时间
	int maxFileSize;	//文件到达压缩的大小
	int timeElapse;	//时间间隔
	
	sem_t *pSem;
	log_cfg_st *pCfg;	//共享内存控制块
	log_string_st *pString;	//日志数据块

}log_gb_context_;



void log_init_load_cfg();
int log_init(char *version);
void log_init_load_json_cfg();

int check_timer_flush();
void log_flush();

void *log_main_loop(void *data);

void* log_tarSys_loop(void *data);

void log_update();

void write_log(int writePos);










