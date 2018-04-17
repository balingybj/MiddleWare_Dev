
#include "log.h"
#include "log_interface.h"

static int gtLockFileFd = 0;

// 日志配置相关参数
ST_LOG_CONFIG_      gstLogConfig;



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
		printf("Open file(%s) failed. Errors:%s\n", pName, strerror(errno));
		goto _EXIT_PROCESS;
	}

	// 尝试对文件加锁         LOCK_EX 为互斥锁   ，LOCK_NB为非阻塞，flock方式
	if(0 == flock(gtLockFileFd, LOCK_EX | LOCK_NB))
	{
		atexit(close_lock_file);
		return ;
	}
	//if(0 == fcntl_lock_file())
	
	printf("The process %s has run\n", pName);

_EXIT_PROCESS:
	close(gtLockFileFd);
	gtLockFileFd = -1;
	exit(0);
}


/* 获取文件大小             参数1：文件名，参数2：文件大小       */
int common_get_file_size(char *fileName,int *fileSize)
{
	struct stat st1;

	if(NULL == fileName || NULL == fileSize)
		return COMMON_FAILURE;

	if(stat(fileName, &st1) < 0)
	{
		printf("File %s not exist \n", fileName);
		return COMMON_FAILURE;
	}

	*fileSize = st1.st_size;

	return COMMON_SUCCESS;
}


/* 读取文件内容     	参数1：文件名，参数2：内容保存指针，参数3：文件大小 */
int common_read_file(char *fileName, char *fileContext, int fileSize)
{
	if(NULL == fileName || NULL == fileContext || fileSize < 0)
		return COMMON_FAILURE;

	FILE *pFile = fopen(fileName, "r");
	if(NULL == pFile)
	{
		printf("Open file %s failed\n", fileName);
		return COMMON_FAILURE;
	}

	fseek(pFile, 0, SEEK_SET);

	int readSize = fread(fileContext, sizeof(char), fileSize, pFile);
	if(readSize != fileSize)
	{
		printf("fread file failed, fileSize:%d, readSize:%d\n", fileSize, readSize);
		fclose(pFile);
		return COMMON_FAILURE;
	}
	fclose(pFile);
	
	return COMMON_SUCCESS;
}


/* 读取json文件内容保存到全局变量中 */
void common_json_init()
{
	cJSON *json;
	char *fileContext = NULL;
	int fileSize = 0;

	memset(&gstLogConfig, 0, sizeof(gstLogConfig));

	if(COMMON_FAILURE == common_get_file_size(LOG_CONFIG_JSON_PATH, &fileSize))
		goto USE_DEFAULT_CONFIG;

	fileContext = (char *)malloc(fileSize);

	if(COMMON_FAILURE == common_read_file(LOG_CONFIG_JSON_PATH, fileContext, fileSize))
		goto USE_DEFAULT_CONFIG;

	json = cJSON_Parse(fileContext);
	free(fileContext);

	if(NULL == json)
	{
		printf("cJson_Parse error.\n");
		goto USE_DEFAULT_CONFIG;
	}

	char *sPrint = cJSON_Print(json);
	printf("Json file:%s\n", sPrint);

	cJSON *enablePrint = cJSON_GetObjectItem(json, "enablePrint");
	if(NULL == enablePrint)
	{
		printf("Get cJSON_GetObjectItem:enablePrint failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.enablePrint = atoi(enablePrint->valuestring);

	cJSON *logOutFormat = cJSON_GetObjectItem(json, "logOutFormat");
	if(NULL == logOutFormat)
	{
		printf("Get cJSON_GetObjectItem:logOutFormat failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.logOutFormat = atoi(logOutFormat->valuestring);
	
	cJSON *maxSaveZipCounts = cJSON_GetObjectItem(json, "maxSaveZipCounts");
	if(NULL == maxSaveZipCounts)
	{
		printf("Get cJSON_GetObjectItem:maxSaveZipCounts failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxSaveZipCounts = atoi(maxSaveZipCounts->valuestring);
	
	cJSON *maxCacheNum = cJSON_GetObjectItem(json, "maxCacheNum");
	if(NULL == maxCacheNum)
	{
		printf("Get cJSON_GetObjectItem:maxCacheNum failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxCacheNum = atoi(maxCacheNum->valuestring);
	
	cJSON *autoRefreshTime = cJSON_GetObjectItem(json, "autoRefreshTime");
	if(NULL == autoRefreshTime)
	{
		printf("Get cJSON_GetObjectItem:autoRefreshTime failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.autoRefreshTime = atoi(autoRefreshTime->valuestring);
	
	cJSON *maxFileSize = cJSON_GetObjectItem(json, "maxFileSize");
	if(NULL == maxFileSize)
	{
		printf("Get cJSON_GetObjectItem:enablePrint failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxFileSize = atoi(maxFileSize->valuestring);
	
	cJSON *logName = cJSON_GetObjectItem(json, "logName");
	if(NULL == logName)
	{
		printf("Get cJSON_GetObjectItem:logName failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	memcpy(gstLogConfig.logName, logName->valuestring, strlen(logName->valuestring));
	
	cJSON *LogPath = cJSON_GetObjectItem(json, "LogPath");
	if(NULL == LogPath)
	{
		printf("Get cJSON_GetObjectItem:LogPath failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	memcpy(gstLogConfig.LogPath, LogPath->valuestring, strlen(LogPath->valuestring));
	
	cJSON *strValidLen = cJSON_GetObjectItem(json, "strValidLen");
	if(NULL == strValidLen)
	{
		printf("Get cJSON_GetObjectItem:strValidLen failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.strValidLen = atoi(strValidLen->valuestring);
	
	cJSON *maxSupportApp = cJSON_GetObjectItem(json, "maxSupportApp");
	if(NULL == maxSupportApp)
	{
		printf("Get cJSON_GetObjectItem:maxSupportApp failed\n");
		goto USE_DEFAULT_CONFIG;
	}
	gstLogConfig.maxSupportApp = atoi(maxSupportApp->valuestring);

	return ;
	
	
USE_DEFAULT_CONFIG:
	memset(&gstLogConfig, 0, sizeof(gstLogConfig));
	printf("use defalut json config:\n");
	gstLogConfig.autoRefreshTime = 60;  //60s
	gstLogConfig.enablePrint = 0;  // noPrint

	memcpy(gstLogConfig.logName, "test.dat", strlen("test.dat"));
	gstLogConfig.logOutFormat = 2;

	memcpy(gstLogConfig.LogPath, "/rru/log", strlen("/rru/log"));
	gstLogConfig.maxCacheNum = 1000;
	gstLogConfig.maxFileSize = 1024*1024;
	gstLogConfig.maxSaveZipCounts = 2;
	gstLogConfig.maxSupportApp = 4;
	gstLogConfig.strValidLen = 128;

	return ;
}

// 截取字符串，只要最后一个/后边的文件名，去除前边的路径
const char *splitFileName(const char *pFileName)
{
	const char *filePath = strrchr(pFileName, '\0');

	if(filePath && *(filePath + 1) != '\0')	return filePath+1;

	return pFileName;
}


void middle_ware_log(int ulLevel, const char* pFileName, const char* pFunName, int ulLine,  const char *format, ...)
{
	va_list argList;
	char pString[255] = {0};

	// 将可变字符格式format格式化为字符串数组
	va_start(argList, format);
	vsnprintf(pString, 255, format, argList);
	va_end(argList);

	printf("fileName:%s funName:%s line:%d      %s\n", splitFileName(pFileName), pFunName, ulLine, pString);
	
	return ;
}


int main (int argc, char **argv)
{
	test_process_running("log_test");

	common_json_init();

	Middle_ware_log(1, "I am a teacher,age %d,name %s ", 12, "Y");

	while(1)
	{
		sleep(1000);
		
	}

    exit(0);
}

