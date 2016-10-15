/*************************************************************************
    > File Name: leds.c
    > Author: ZHAOCHAO
    > Mail: 479680168@qq.com 
    > Created Time: Wed 05 Oct 2016 07:17:24 AM PDT
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>



int main(int argc, char *argv[])
{
	int fd, led_index, led_status, ret;

	if ( argc != 3 ) {
		printf("usage: leds [led_index] [led_status; \n]");
		exit(1);
	} 

	led_index = atoi(argv[1]);
	led_status = atoi(argv[2]);

	if (led_index >= 2 ) {
		printf("error: led_index < 2 \n");
		exit(1);
	}

	if (led_status >= 2 ) {
		printf("error: led_stats 0 for off, 1 for on\n");
		exit(1);
	}

	if ((fd = open("/dev/leds_ctl_dev", O_RDWR|O_NOCTTY|O_NDELAY)) == -1 ) {
		printf("open /dev/leds error. \n");
		exit(1);
	}

	if ((ret = ioctl(fd, led_index, led_status)) == -1) {
		printf("ioctl /dev/leds error.\n");			
		exit(1);
	}
	
	printf("done.\n");


	exit(0);
}
