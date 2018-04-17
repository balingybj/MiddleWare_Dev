/*
	接口文件
*/
#include "log_interface.h"

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

