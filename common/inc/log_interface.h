
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include "cJSON.h"

#include <readline/readline.h>
#include <readline/history.h>

#include <fcntl.h>
#include <sys/file.h>
#include <semaphore.h>
#include <sys/time.h>



// json文件路径
#define LOG_CONFIG_JSON_PATH "../pub/cfg/log_config.json"


#define LOG_FILE_NAME_LEN 32
#define LOG_FUNC_NAME_LEN 64
#define LOG_APP_NAME_LEN 16
#define LOG_STRING_LEN 256


#define MIDDLE_COMMON_SUCCESS 0
#define MIDDLE_COMMON_FAILURE 1



#define _ASSERT(condition)	do\
{ \
	if(condition)	{} 	\
	else 	\
	{	\
		printf("Assert failed,file:%s func:%s line:%d", __FILE__,  __func__, (int)__LINE__);    \
		abort();    \
	}	\
}while(0)



#define Middle_ware_log(ulLevel,format,...) do\
{\
	middle_ware_log(ulLevel, __FILE__, __func__, __LINE__, \
		(const char *)format, ##__VA_ARGS__); \
}while(0)



// 注意文件名要以'/'开头，文件放在/dev/shm目录下
#define LOG_SEM_NAME "/log_sem"
#define LOG_SHM_NAME "/log_shm"






typedef struct
{
	char appName[LOG_APP_NAME_LEN];   //进程名
	char logLevel;		//日志等级
	char used;	//是否被是用了，0未使用，1使用
}log_filter_st;



/* 共享内存控制结构体 */
typedef struct 
{
	char version[32];
	unsigned int writePos;		// 业务进程使用，当前写的位置
	unsigned int readPos;		// 代理进程使用，FLASH中实时读的位置
	unsigned int flashWritePos;   // 代理进程使用，FLASH中实时写的位置
	int flushFlag;  	// 强制刷新标志
	unsigned int serNum;			//日志流水号，业务进程使用
	log_filter_st log_filter[10];  //都会使用，日志等级
}log_cfg_st;


typedef enum log_type
{
	LOG_NONE = 0,
	LOG_ERROR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_MAX,
}log_type_en;

/* 日志内容结构体 */
typedef struct
{
	char appName[LOG_APP_NAME_LEN];         //进程名
	int moduleId;     //模块ID
	int logLevel;		//日志等级
	int timeSec;
	int timeMsec;
	char fileName[LOG_FILE_NAME_LEN];
	char funName[LOG_FUNC_NAME_LEN];
	int fileLine;
	int serNum;			//流水号
	char string[LOG_STRING_LEN];	//实际内容
}log_string_st;


/* 日志模块全局数据结构 */
typedef struct 
{
	int maxCacheNum;	//最大缓存数量
	int localPrint;	//是否本地打印
	log_filter_st log_filter;
	
	sem_t *pSem;
	log_cfg_st *pCfg;	//共享内存控制块
	log_string_st *pString;	//日志数据块

}middle_log_context;




extern middle_log_context *gulMiddleWareLog;





extern void middle_ware_log(int ulLevel, const char* pFileName, const char* pFunName, int ulLine,  const char *format, ...);

extern const char *splitFileName(const char *pFileName);

extern int common_read_file(char *fileName, char *fileContext, int fileSize);

extern int common_get_file_size(char *fileName,int *fileSize);

extern void common_json_init();

extern int middle_ware_init_log();






