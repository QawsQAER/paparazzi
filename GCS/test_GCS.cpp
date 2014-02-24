#include "main.h"
#include "Ground_Station.h"

int main(int argc, char **argv)
{
	Ground_Station * GCS;
	char default_portname[32] = "/dev/ttyUSB0"; 
	if(argc == 2)
		GCS = new Ground_Station(argv[1]);	
	else	
		GCS = new Ground_Station(default_portname);
//	uint8_t ac_id = 162;
	GCS->init_quadcopters();
	return 0;
}
