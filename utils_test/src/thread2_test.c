#include <stdio.h>
#include <stdlib.h>

#include "log_interface.h"

int main()
{
	middle_ware_init_log(1);

	Middle_ware_log(1,"This is the second one");
	Middle_ware_log(1,"This is the second one");
	Middle_ware_log(1,"This is the secondone");
		Middle_ware_log(1,"This is the second one");
	Middle_ware_log(1,"This is the second one");
	Middle_ware_log(1,"This is the second one");
		Middle_ware_log(1,"This is the second one");
	Middle_ware_log(1,"This is the second one");
	Middle_ware_log(1,"This is the second one");


	return 0;
}

