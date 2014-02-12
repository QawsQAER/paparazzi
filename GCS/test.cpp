/*
#include <stdlib.h>
#include <stdio.h>
#include <string>
//for O_RDWR, O_NOCITTY, O_SYNC those MACRO
#include <fcntl.h>
#include "Serial.h"
*/
#include "main.h"
#include "XBEE.h"
#include "pprz_msg.h"
int main(int argc, char ** argv)
{
	char portname[32];
	if(argc < 2)
	{
		printf("Warning: Using default port /dev/ttyUSB0\n");
		strcpy(portname,"/dev/ttyUSB0");
	}
	else
		strcpy(portname,argv[1]);
	 
	printf("name of device %s\n",portname);
	#if _DEBUG
		printf("!!!DEBUG MODE\n");
	#endif
	#if _DEBUG_XBEE_parse_XBEE_msg
		printf("!!!DEBUGING XBEE_parse_XBEE_msg()\n");
	#endif


	//Configure the port	
	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		fprintf(stderr, "%s\n", strerror(errno));
		return 0;
	}
	set_interface_attribs(fd,B57600,0);
	set_blocking(fd,0);
	//Configuration done

	//declaring XBEE module
	XBEE xbee_coor(fd,0x00000000,0x0000ffff,0);

	while(1)
	{
		//read every 5s
		sleep(5);
		xbee_coor.XBEE_read_into_recv_buff();
		//after read, data in recv_buff should be parse by XBEE::XBEE_parse_XBEE_msg()
		xbee_coor.XBEE_parse_XBEE_msg();

		//show the message
		//xbee_coor.XBEE_show_msg();
		
		while(!xbee_coor.msg.empty())
		{
			XBEE_msg * ptr = xbee_coor.msg.front();
			xbee_coor.msg.pop();
			pprz_msg data = ptr->get_pprz_msg();	
			data.show_hex();
			delete ptr;
		}
	}
	return 0;
}
