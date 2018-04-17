
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>

#include <fcntl.h>
#include <errno.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <time.h>
#include <dirent.h>


#include "cJSON.h"

#include <stdarg.h>


#include <readline/readline.h>
#include <readline/history.h>



// json文件路径
#define LOG_CONFIG_JSON_PATH "../cfg/log_config.json"


#define  COMMON_SUCCESS       	0
#define  COMMON_FAILURE 		1


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


extern ST_LOG_CONFIG_    gstLogConfig;


extern int common_read_file(char *fileName, char *fileContext, int fileSize);

extern int common_get_file_size(char *fileName,int *fileSize);

extern void common_json_init();


