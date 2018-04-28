/*
	接口文件
*/
#include "log_interface.h"


//#define LOG_DEBUG 1


//日志共享内存全局变量
middle_log_context *gulMiddleWareLog = NULL;





/* 获取文件大小             参数1：文件名，参数2：文件大小       */
int common_get_file_size(char *fileName,int *fileSize)
{
	struct stat st1;

	if(NULL == fileName || NULL == fileSize)
		return MIDDLE_COMMON_FAILURE;

	if(stat(fileName, &st1) < 0)
	{
		printf("File %s not exist \n", fileName);
		return MIDDLE_COMMON_FAILURE;
	}

	*fileSize = st1.st_size;

	return MIDDLE_COMMON_SUCCESS;
}


/* 读取文件内容     	参数1：文件名，参数2：内容保存指针，参数3：文件大小 */
int common_read_file(char *fileName, char *fileContext, int fileSize)
{
	if(NULL == fileName || NULL == fileContext || fileSize < 0)
		return MIDDLE_COMMON_FAILURE;

	FILE *pFile = fopen(fileName, "r");
	if(NULL == pFile)
	{
		printf("Open file %s failed\n", fileName);
		return MIDDLE_COMMON_FAILURE;
	}

	fseek(pFile, 0, SEEK_SET);

	int readSize = fread(fileContext, sizeof(char), fileSize, pFile);
	if(readSize != fileSize)
	{
		printf("fread file failed, fileSize:%d, readSize:%d\n", fileSize, readSize);
		fclose(pFile);
		return MIDDLE_COMMON_FAILURE;
	}
	fclose(pFile);
	
	return MIDDLE_COMMON_SUCCESS;
}


// 截取字符串，只要最后一个/后边的文件名，去除前边的路径
const char *splitFileName(const char *pFileName)
{
	const char *filePath = strrchr(pFileName, '\0');

	if(filePath && *(filePath + 1) != '\0')	return filePath+1;

	return pFileName;
}

//获取当前进程的名字
int getProcessName(char *process, int len)
{
	char buf[256] = {0};
	char *str_start = NULL;

	//获取链接的内容，返回长度
	if(readlink("/proc/self/exe", buf, 256) <= 0)
	{
		printf("get process name readlink failed,errno:%d\n", errno);
		return MIDDLE_COMMON_FAILURE;
	}

	str_start = strrchr(buf, '/');
	if(NULL == str_start)
	{
		printf("get process name strrchr failed,errno:%d\n", errno);
		return MIDDLE_COMMON_FAILURE;
	}
	str_start++;

	strncpy(process, str_start, len);

	return MIDDLE_COMMON_SUCCESS;
}

//加载配置文件
void middle_load_cfg()
{
	_ASSERT(gulMiddleWareLog);

	cJSON *json;
	char *fileContext = NULL;
	int fileSize = 0;


	if(MIDDLE_COMMON_FAILURE == common_get_file_size(LOG_CONFIG_JSON_PATH, &fileSize))
		goto _DEFAULT_CONFIG;

	fileContext = (char *)malloc(fileSize);

	if(MIDDLE_COMMON_FAILURE == common_read_file(LOG_CONFIG_JSON_PATH, fileContext, fileSize))
		goto _DEFAULT_CONFIG;

	json = cJSON_Parse(fileContext);
	free(fileContext);
	
	if(NULL == json)
	{
		printf("cJson_Parse error.\n");
		goto _DEFAULT_CONFIG;
	}

	cJSON *maxCacheNum = cJSON_GetObjectItem(json, "maxCacheNum");
	if(NULL == maxCacheNum)
	{
		printf("Get cJSON_GetObjectItem:maxCacheNum failed\n");
		goto _DEFAULT_CONFIG;
	}
	gulMiddleWareLog->maxCacheNum = atoi(maxCacheNum->valuestring);

	return ;

_DEFAULT_CONFIG:
	gulMiddleWareLog->maxCacheNum = 2000;

	return ;
}

/* 日志相关初始化接口 */
int middle_ware_init_log(int enableLocalPrint)
{
	char *shmAddress = NULL;
	int shmId = 0;
	int length = 0;

	/* 初始化开辟共享内存及信号量 */
	if(NULL != gulMiddleWareLog)
	{
		return MIDDLE_COMMON_SUCCESS;
	}

	gulMiddleWareLog = (middle_log_context *)malloc(sizeof(gulMiddleWareLog));
	if(NULL == gulMiddleWareLog)
	{
		//syslog("log init malloc failed");
		printf("Middle ware init log malloc failed\n");
		return MIDDLE_COMMON_FAILURE;
	}

	//读取配置文件初始化
	middle_load_cfg();

	//初始化信号量，文件不存在创建，存在则打开  --> 有名信号量，可以用于进程间的同步
	gulMiddleWareLog->pSem = sem_open(LOG_SEM_NAME, O_EXCL);
	if(gulMiddleWareLog->pSem == NULL)
	{
		printf("Middle Init  sem failed, errno:%d \n", errno);
		goto	FAIL;
	}

	//判断文件是否存在
	if((shmId = shm_open(LOG_SHM_NAME, O_RDWR, 0644)) != -1)
	{
		//mmap映射内存
		//最大缓存数量的两倍
		length = (sizeof(log_string_st) )* (gulMiddleWareLog->maxCacheNum) * 2;
		shmAddress = (char *)mmap(0, length + sizeof(log_cfg_st), PROT_READ  |PROT_WRITE, MAP_SHARED, shmId, 0);
		close(shmId);
		
		if((char *)-1 == shmAddress)
		{
			printf("Middle ware init log shmFile failed, err:%d", errno);
			goto FAIL;
		}

		getProcessName(gulMiddleWareLog->log_filter.appName, LOG_APP_NAME_LEN);
		gulMiddleWareLog->log_filter.logLevel = LOG_INFO;
		gulMiddleWareLog->localPrint = enableLocalPrint;
		
		gulMiddleWareLog->pCfg = (log_cfg_st *)shmAddress;
		gulMiddleWareLog->pString = (log_string_st *)(shmAddress + sizeof(log_cfg_st));
		


		printf("Middle ware init log success\n");
		return MIDDLE_COMMON_SUCCESS;
	}

FAIL:
	printf("Middle ware init log failed\n");
	if(gulMiddleWareLog != NULL)
	{
		free(gulMiddleWareLog);
		gulMiddleWareLog = NULL;
	}
	
	return MIDDLE_COMMON_FAILURE;
}

//加上ms的时间
void middle_add_ms(struct timeval *time, int ms)
{
	time->tv_usec += 1000 * ms;
	if(time->tv_usec >= 1000000)
	{
		time->tv_usec %= 1000000;
		time->tv_sec += time->tv_usec / 1000000;
	}
}

//设置日志等级
void middle_set_log_level(int level)
{
	if(NULL != gulMiddleWareLog)
	{
		gulMiddleWareLog->log_filter.logLevel = level;
	}
}


//获取信号量
int middle_sem_timewait(int ms)
{
	struct timeval now;
	struct timespec end;
	int ret = 0;

	//第二个参数为时区
	gettimeofday(&now, NULL);
	middle_add_ms(&now, ms);

	end.tv_sec = now.tv_sec;    //秒
	end.tv_nsec = now.tv_usec * 1000;    //纳秒

	//EINTR错误需要特殊处理，当该调用被信号中断时会直接返回错误，错误码设置为EINTR，这种情况需要继续手动调用函数
	while((ret = sem_timedwait(gulMiddleWareLog->pSem, &end) == -1)  && errno == EINTR)
		continue;

	if(-1 == ret)
	{
		if(ETIMEDOUT == errno)
		{
			int semNum = 0;
			if(0 == sem_getvalue(gulMiddleWareLog->pSem, &semNum))
			{
				if(0 == semNum)
				{
					sem_post(gulMiddleWareLog->pSem);
					sem_wait(gulMiddleWareLog->pSem);
					return MIDDLE_COMMON_SUCCESS;
				}
				else
				{
					printf("sem value :%d\n", semNum);
					goto RE_CREAT;
				}
			}
			else
			{
				printf("sem_getvalue failed\n");
				goto RE_CREAT;
			}
		}
		
		RE_CREAT:
		//清除之前的文件，重新创建，但有一个问题，在有进程没有关闭文件的情况下，sem_unlink不会成功，感觉不太对
		printf("sem error, need to recreat sem\n");

		sem_close(gulMiddleWareLog->pSem);
		sem_unlink(LOG_SEM_NAME);

		gulMiddleWareLog->pSem = sem_open(LOG_SEM_NAME, O_CREAT, 0644, 1);
		if(gulMiddleWareLog->pSem == NULL)
		{
			printf("sem_open failed, errno:%d \n", errno);

			//这样直接返回是不是不合理，后边如果继续使用空指针会出错。直接退出是不是比较好
			return MIDDLE_COMMON_FAILURE;
			//exit(0);
		}
		
		sem_wait(gulMiddleWareLog->pSem);
		
	}

	return MIDDLE_COMMON_SUCCESS;
}


/* 写日志接口 */
void middle_ware_log(int level, const char* pFileName, const char* pFunName, int fileLine,  const char *format, ...)
{

#ifdef LOG_DEBUG
	va_list argList;
	char pString[255] = {0};

	// 将可变字符格式format格式化为字符串数组
	va_start(argList, format);
	vsnprintf(pString, 255, format, argList);
	va_end(argList);

	printf("fileName:%s funName:%s line:%d      %s\n", splitFileName(pFileName), pFunName, fileLine, pString);
#endif

	va_list argList1;
	char pString1[255] = {0};
	struct timeval tv;
	gettimeofday(&tv,NULL);
	

	// 将可变字符格式format格式化为字符串数组
	va_start(argList1, format);
	vsnprintf(pString1, 255, format, argList1);
	va_end(argList1);

	unsigned int ulWritePos = 0;
	unsigned int ulSerNum = 0;
	log_string_st *pLogString = NULL;

	middle_sem_timewait(1000);

	//获取当前写的位置和实时流水号，如果写的速度太快则向前覆盖
	ulWritePos = gulMiddleWareLog->pCfg->writePos % (gulMiddleWareLog->maxCacheNum * 2);
	ulSerNum = gulMiddleWareLog->pCfg->serNum;

	//writePos一直这么加下去没问题吗？？？
	gulMiddleWareLog->pCfg->writePos++;
	gulMiddleWareLog->pCfg->serNum++;

	sem_post(gulMiddleWareLog->pSem);

	pLogString = gulMiddleWareLog->pString + ulWritePos;

	strncpy(pLogString->appName, gulMiddleWareLog->log_filter.appName, LOG_APP_NAME_LEN);
	strncpy(pLogString->fileName, splitFileName(pFileName), LOG_FILE_NAME_LEN);
	strncpy(pLogString->funName, pFunName, strlen(pFunName));

	pLogString->moudleId = 0;
	pLogString->logLevel = level;
	pLogString->fileLine = fileLine;
	pLogString->timeMsec = tv.tv_usec;
	pLogString->timeSec = tv.tv_sec;

	strncpy(pLogString->string, pString1, strlen(pString1));

	
	//printf(" --- --- %s\n", pString1);

	return ;
}

