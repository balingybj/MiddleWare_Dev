
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>




#define Middle_ware_log(ulLevel,format,...) do\
{\
	middle_ware_log(ulLevel, __FILE__, __func__, __LINE__, \
		(const char *)format, ##__VA_ARGS__); \
}while(0)


extern void middle_ware_log(int ulLevel, const char* pFileName, const char* pFunName, int ulLine,  const char *format, ...);

extern const char *splitFileName(const char *pFileName);





