#include <stdio.h>
#include <stdlib.h>

#include "log_interface.h"

int main()
{
	middle_ware_init_log(1);
	int loop = 0;


	Middle_ware_log(1,"This is the first one");
	Middle_ware_log(1,"This is the first one");
	Middle_ware_log(1,"This is the first one");

		Middle_ware_log(1,"This is the first one");
	Middle_ware_log(1,"This is the first one");
	Middle_ware_log(1,"This is the first one");

		Middle_ware_log(1,"This is the first one");
	Middle_ware_log(1,"This is the first one");
	Middle_ware_log(1,"This is the first one");


	return 0;
}

