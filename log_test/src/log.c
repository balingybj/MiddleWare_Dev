

#include "log.h"
#include <pthread.h>


static int gtLockFileFd = 0;


log_gb_context_  *gstLogGbContext = NULL;

// 日志进程相关参数
ST_LOG_CONFIG_   	gstLogConfig;



static void close_lock_file()
{
	if(gtLockFileFd != -1)
		close(gtLockFileFd);

	gtLockFileFd = -1;
}

/*
static int fcntl_lock_file()
{
	struct flock stFlock;

	// 写锁
	stFlock.l_type = F_WRLCK;

	// 设置为全文加锁（整个文件加锁）
	stFlock.l_start = 0;
	stFlock.l_whence = SEEK_SET;
	stFlock.l_len = 0;

	return (fcntl(gtLockFileFd, F_SETLK, &stFlock));
}
*/

// 检验文件是否已经运行
static void test_process_running(char *pName)
{
	if(NULL == pName)
		return ;

	char lock_file[64];

	snprintf(lock_file, sizeof(lock_file), "/tmp/%s.lock", pName);

	if((gtLockFileFd = open(lock_file, O_CREAT | O_RDWR, 0644)) < 0)
	{
		LOG_PRINT("Open file(%s) failed. Errors:%s\n", pName, strerror(errno));
		goto _EXIT_PROCESS;
	}

	// 尝试对文件加锁         LOCK_EX 为互斥锁   ，LOCK_NB为非阻塞，flock方式
	if(0 == flock(gtLockFileFd, LOCK_EX | LOCK_NB))
	{
		atexit(close_lock_file);
		return ;
	}
	//if(0 == fcntl_lock_file())
	
	LOG_PRINT("The process %s has run\n", pName);

_EXIT_PROCESS:
	close(gtLockFileFd);
	gtLockFileFd = -1;
	exit(0);
}

void log_init_load_json_cfg()
{
	cJSON *json;
	char *fileContext = NULL;
	int fileSize = 0;

	memset(&gstLogConfig, 0, sizeof(gstLogConfig));

	if(MIDDLE_COMMON_FAILURE == common_get_file_size(LOG_CONFIG_JSON_PATH, &fileSize))
		goto USE_DEFAULT_CONFIG;

	fileContext = (char *)malloc(fileSize);

	if(MIDDLE_COMMON_FAILURE == common_read_file(LOG_CONFIG_JSON_PATH, fileContext, fileSize))
		goto USE_DEFAULT_CONFIG;

	json = cJSON_Parse(fileContext);
	free(fileContext);
	
	if(NULL == json)
	{
		LOG_PRINT("cJson_Parse error.\n");
		goto USE_DEFAULT_CONFIG;
	}

	char *sPrint = cJSON_Print(json);
	printf("Json file:%s\n", sPrint);

	cJSON *enablePrint = cJSON_GetObjectItem(json, "enablePrint");
	if(NULL == enablePrint)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:enablePrint failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.enablePrint = atoi(enablePrint->valuestring);

	cJSON *logOutFormat = cJSON_GetObjectItem(json, "logOutFormat");
	if(NULL == logOutFormat)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:logOutFormat failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.logOutFormat = atoi(logOutFormat->valuestring);
	
	cJSON *maxSaveZipCounts = cJSON_GetObjectItem(json, "maxSaveZipCounts");
	if(NULL == maxSaveZipCounts)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:maxSaveZipCounts failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxSaveZipCounts = atoi(maxSaveZipCounts->valuestring);
	
	cJSON *maxCacheNum = cJSON_GetObjectItem(json, "maxCacheNum");
	if(NULL == maxCacheNum)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:maxCacheNum failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxCacheNum = atoi(maxCacheNum->valuestring);
	
	cJSON *autoRefreshTime = cJSON_GetObjectItem(json, "autoRefreshTime");
	if(NULL == autoRefreshTime)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:autoRefreshTime failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.autoRefreshTime = atoi(autoRefreshTime->valuestring);
	
	cJSON *maxFileSize = cJSON_GetObjectItem(json, "maxFileSize");
	if(NULL == maxFileSize)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:enablePrint failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxFileSize = atoi(maxFileSize->valuestring);
	
	cJSON *logName = cJSON_GetObjectItem(json, "logName");
	if(NULL == logName)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:logName failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	memcpy(gstLogConfig.logName, logName->valuestring, strlen(logName->valuestring));
	
	cJSON *LogPath = cJSON_GetObjectItem(json, "LogPath");
	if(NULL == LogPath)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:LogPath failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	memcpy(gstLogConfig.LogPath, LogPath->valuestring, strlen(LogPath->valuestring));
	
	cJSON *strValidLen = cJSON_GetObjectItem(json, "strValidLen");
	if(NULL == strValidLen)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:strValidLen failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.strValidLen = atoi(strValidLen->valuestring);
	
	cJSON *maxSupportApp = cJSON_GetObjectItem(json, "maxSupportApp");
	if(NULL == maxSupportApp)
	{
		LOG_PRINT("Get cJSON_GetObjectItem:maxSupportApp failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxSupportApp = atoi(maxSupportApp->valuestring);

	return ;
	
	
USE_DEFAULT_CONFIG:
	memset(&gstLogConfig, 0, sizeof(gstLogConfig));
	LOG_PRINT("use defalut json config:\n");
	gstLogConfig.autoRefreshTime = 60;	//60s
	gstLogConfig.enablePrint = 0;  // noPrint

	memcpy(gstLogConfig.logName, "test.dat", strlen("test.dat"));
	gstLogConfig.logOutFormat = 2;

	memcpy(gstLogConfig.LogPath, "./", strlen("./"));
	gstLogConfig.maxCacheNum = 2000;
	gstLogConfig.maxFileSize = 2*1024*1024;
	gstLogConfig.maxSaveZipCounts = 10;
	gstLogConfig.maxSupportApp = 4;
	gstLogConfig.strValidLen = 128;

}

//加载配置文件进行初始化相关参数
void log_init_load_cfg()
{
	LOG_ASSERT(gstLogGbContext != NULL);

	log_init_load_json_cfg();

	gstLogGbContext->autoRefreshTime = gstLogConfig.autoRefreshTime;
	gstLogGbContext->localPrint = gstLogConfig.enablePrint;
	gstLogGbContext->maxCacheNum = gstLogConfig.maxCacheNum;
	gstLogGbContext->maxFileSize = gstLogConfig.maxFileSize;
	gstLogGbContext->maxSaveZipCounts = gstLogConfig.maxSaveZipCounts;

	LOG_PRINT("localPrint:%d, autoRefreshTime:%d, maxCacheNum:%d, maxFileSize:%d, maxSaveZipCounts:%d\n",\
		gstLogGbContext->localPrint, gstLogGbContext->autoRefreshTime, gstLogGbContext->maxCacheNum, gstLogGbContext->maxFileSize,\
			gstLogGbContext->maxSaveZipCounts);

	return ;
}

//清除之前初始化的变量
static void log_uninit()
{
	if(NULL == gstLogGbContext)
	{
		return ;
	}

	if(gstLogGbContext->pSem != NULL)
	{
		sem_unlink(LOG_SEM_NAME);
	}

	if(gstLogGbContext->pCfg != (log_cfg_st *)-1)
	{
		shm_unlink(LOG_SHM_NAME);
	}

	free(gstLogGbContext);
	gstLogGbContext = NULL;
	return ;
}


//日志代理进程初始化操作
int log_init(char *version)
{
	char *shmAddress = NULL;
	int fd = 0;
	int length = 0;
	int shmId = 0;

	//初始化分配内存空间
	if(NULL == gstLogGbContext)
	{
		gstLogGbContext = (log_gb_context_ *)malloc(sizeof(log_gb_context_));
		if(NULL == gstLogGbContext)
		{
			//syslog("log init malloc failed");
			LOG_PRINT("Log init malloc failed\n");
			return LOG_FAILURE;
		}
		memset(gstLogGbContext, 0, sizeof(log_gb_context_));
	}
	else
	{
		return LOG_SUCCESS;
	}

	//读取配置文件初始化
	log_init_load_cfg();

	//输出重定向
	if(!gstLogGbContext->localPrint)
	{
		fd = open("/dev/null", O_RDWR);
		dup2(fd, 1);
		close(fd);
	}

	//最大缓存数量的两倍
	length = (sizeof(log_string_st)) *( gstLogGbContext->maxCacheNum) * 2;


CREAT:
	//初始化信号量，文件不存在创建，存在则打开  --> 有名信号量，可以用于进程间的同步
	gstLogGbContext->pSem = sem_open(LOG_SEM_NAME, O_CREAT, 0644, 1);
	if(gstLogGbContext->pSem == NULL)
	{
		LOG_PRINT("Init  sem failed, errno:%d \n", errno);
		return LOG_FAILURE;
	}
	
#if 0

注意点：这里是文件存在则直接打开了，如果之前有线程申请了资源，在未释放之前被杀掉了（异常终止），则信号量的值变为0了，
		内核也不会因为该线程的异常而重置信号量，这种情况下会导致永远无法得到资源了。

可用解决办法：
	业务进程使用时用sem_timewait之类的函数，超时未获取到后，通过sem_getvalue函数获取当前信号量的值，如果大于0，不可能出现阻塞的情况；
	如果小于0，其绝对值是当前被阻塞住线程（申请资源的个数）的个数，这种情况下则直接删除这个文件，然后重新创建。
	如果等于0，可以按照小于0时的处理策略，CSPL中是直接添加一个资源，然后自己重新去获取（会不会出问题？）

或者直接这里，也就是创建的地方，打开文件后读取一下信号量的值，如果小于等于0，则重新创建，或者将值调整为1。

	int num = 0;
	sem_getvalue(gstLogGbContext->pSem, &num);
	printf("sem count is %d\n", num);
#endif

	//创建或打开一段共享内存，文件已经存在则直接返回失败（O_CREAT和O_EXCL组合使用），O_RDWR读写方式打开
	shmId = shm_open(LOG_SHM_NAME, O_RDWR | O_CREAT  | O_EXCL , 0644);
	if(shmId == -1)
	{
		//判断文件是否存在
		if((shmId = shm_open(LOG_SHM_NAME, O_RDWR, 0644)) != -1)
		{
			//mmap映射内存
			shmAddress = (char *)mmap(0, length + sizeof(log_cfg_st), PROT_READ  |PROT_WRITE, MAP_SHARED, shmId, 0);
			close(shmId);
			
			if((char *)-1 == shmAddress)
			{
				log_uninit();
				LOG_PRINT("Init mmap shmFile failed, err:%d", errno);
				return LOG_FAILURE;
			}
			
			gstLogGbContext->pCfg = (log_cfg_st *)shmAddress;
			if(strncmp(gstLogGbContext->pCfg->version, version, strlen(version)))
			{
				//版本不一致则删除共享内存，重新创建
				LOG_PRINT("Log version has changed,need to again");
				munmap(shmAddress, length + sizeof(log_cfg_st));
				shm_unlink(LOG_SHM_NAME);
				sem_close(gstLogGbContext->pSem);
				sem_unlink(LOG_SEM_NAME);
				goto CREAT;
			}
			else
			{
				LOG_PRINT("Log version is the same");
			}
			
			gstLogGbContext->pString = (log_string_st *)(shmAddress + sizeof(log_cfg_st));
			//清除之前的数据，是否有必要？
			memset(gstLogGbContext->pString, 0, sizeof(length));

			return LOG_SUCCESS;
		}
		else
		{
			log_uninit();
			LOG_PRINT("Init create shm failed, err:%d", errno);
			return LOG_FAILURE;
		}
	}

	//调节文件（共享内存）的大小，查看文件大小会变为调节后的大小
	if(ftruncate(shmId, length + sizeof(log_cfg_st)) == -1)
	{
		log_uninit();
		LOG_PRINT("Init ftruncate shmFile failed, err:%d", errno);
		return LOG_FAILURE;
	}

	//mmap映射内存
	shmAddress = (char *)mmap(0, length + sizeof(log_cfg_st), PROT_READ  |PROT_WRITE, MAP_SHARED, shmId, 0);
	close(shmId);
	if((char *)-1 == shmAddress)
	{
		log_uninit();
		LOG_PRINT("Init mmap shmFile failed, err:%d", errno);
		return LOG_FAILURE;
	}
	memset(shmAddress, 0, sizeof(log_cfg_st) + length);

	gstLogGbContext->pCfg = (log_cfg_st *)shmAddress;
	strncpy(gstLogGbContext->pCfg->version, version, 32);
	
	gstLogGbContext->pString = (log_string_st *)(shmAddress + sizeof(log_cfg_st));

	LOG_PRINT("Log init success");
	
	return LOG_SUCCESS;
}


static FILE* file_open(const char *fileName)
{
	if(NULL == fileName)
		return NULL;

	FILE *fp = NULL;

	fp = fopen(fileName, "a+");
	if(NULL == fp)
		printf("Open file %s failed\n", fileName);

	return fp;
}

static void file_write(FILE *fp, log_string_st *pLog)
{
	log_string_st *log = pLog;
	if(NULL == fp || NULL == log)
		return ;

	int module = log->moduleId;
	int logLevel = log->logLevel;
	int timeSec = log->timeSec;
	int timeMsec = log->timeMsec;
	int fileLine = log->fileLine;
	int serNum = log->serNum;

	char funcName[LOG_FUNC_NAME_LEN] = {0};
	strncpy(funcName, log->funName, strlen(log->funName));

	char fileName[LOG_FILE_NAME_LEN] = {0};
	strncpy(fileName, log->fileName, strlen(log->fileName));

	char appName[LOG_APP_NAME_LEN] = {0};
	strncpy(appName, log->appName, strlen(log->appName));

	char logStr[LOG_STRING_LEN] = {0};
	strncpy(logStr, log->string, strlen(log->string));
	
	
	char str[2*LOG_STRING_LEN] = {0};

	// 时间这块还需要转换才行
	sprintf(str, "%s %d %d:%d %s %s %d %d %d %s\n", appName, serNum, timeSec, \
		timeMsec, fileName, funcName, fileLine, module, logLevel, logStr);

	fwrite(str, strlen(str), 1, fp);

	return ;
}

static void file_close(FILE* fp)
{
	if(NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}
}


//写FLASH
void write_log(int writePos)
{
	char tempFile[LOG_FILE_NAME_LEN] = {0};
	FILE *fp = NULL;

	strncpy(tempFile, gstLogConfig.logName, strlen(gstLogConfig.logName));
	
	fp = file_open(tempFile);

	if(NULL == fp)
		return ;

	while(writePos > gstLogGbContext->pCfg->flashWritePos)
	{
		log_string_st *log = gstLogGbContext->pString + gstLogGbContext->pCfg->flashWritePos % (2 * gstLogGbContext->maxCacheNum);

		file_write(fp, log);
		
		gstLogGbContext->pCfg->flashWritePos++;
	}

	file_close(fp);
	
}

// 实时更新日志
void log_update()
{
	int currentWritePos = gstLogGbContext->pCfg->writePos;
	int max_cache = gstLogGbContext->maxCacheNum > 2000 ? 2000 : gstLogGbContext->maxCacheNum;

	if(currentWritePos > gstLogGbContext->pCfg->readPos)
	{
		while(currentWritePos > gstLogGbContext->pCfg->readPos)
		{
			gstLogGbContext->pCfg->readPos++;
		}
	}

	// 是否需要写FLASH
	if(currentWritePos - gstLogGbContext->pCfg->flashWritePos >= max_cache)
	{
		write_log(currentWritePos);
	}

	return ;
}

// 将共享内存中的日志搬到FLASH中
void log_flush()
{
	int currentWritePos = gstLogGbContext->pCfg->writePos;

	if(currentWritePos > gstLogGbContext->pCfg->flashWritePos)
	{
		write_log(currentWritePos);
	}
}

// 检测定时刷新时间是否到达
int check_timer_flush()
{
	static time_t begin = 0;
	time_t end = 0;

	if(0 == begin)
	{
		begin = time(NULL);
		gstLogGbContext->timeElapse = 0;
		return LOG_FALSE;
	}
	end = time(NULL);

	gstLogGbContext->timeElapse += (end - begin);
	begin = end;
	
	if(gstLogGbContext->timeElapse >= gstLogGbContext->autoRefreshTime)
	{
		gstLogGbContext->timeElapse = 0;
		return LOG_TRUE;
	}

	return LOG_FALSE;
}

//管理共享内存
void* log_main_loop(void *data)
{
	while(1)
	{
		// 判读是否有日志刷新
		if(gstLogGbContext->pCfg->readPos != gstLogGbContext->pCfg->writePos)
		{
			log_update();
		}

		// 定时刷新周期到达
		if(LOG_TRUE == check_timer_flush())
		{
			printf("time to flush.\n");
			log_flush();
		}

		// 强制刷新标志
		if(gstLogGbContext->pCfg->flushFlag)
		{
			printf("force to flush.\n");
			log_flush();
		}

		usleep(3000);
	}
	
}

//管理压缩文件
void* log_tarSys_loop(void *data)
{
	while(1)
	{
		
	}
}

int main (int argc, char **argv)
{
	pthread_t threadId;
	pthread_attr_t attr;

	test_process_running("log_test");
	log_init(LOG_VERSION);

	pthread_attr_init(&attr);
	if(0 != pthread_create(&threadId, &attr, log_main_loop, NULL))
	{
		printf("pthread_create failed\n");
		return 0;
	}

	if(0 != pthread_create(&threadId, &attr, log_tarSys_loop, NULL))
	{
		printf("pthread_create failed\n");
		return 0;
	}

    while(1)
	{
		sleep(100);
	}

	return 0;
}

