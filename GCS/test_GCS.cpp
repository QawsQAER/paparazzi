#include "main.h"
#include "Ground_Station.h"
#include "signal.h"

Ground_Station * GCS;
void kill_all_quads(int32_t signum)
{
	printf("killing all quads\n");
	GCS->ap_kill_quadcopter();
	GCS->wait_all_quads(SWARM_KILLED);
	exit(1);
}

int main(int argc, char **argv)
{
	char default_portname[32] = "/dev/ttyUSB0"; 
	if(argc == 2)
		GCS = new Ground_Station(argv[1]);	
	else	
		GCS = new Ground_Station(default_portname);
//	uint8_t ac_id = 162;

	GCS->init_quadcopters();
	GCS->negotiate_ref();
	signal(SIGINT,kill_all_quads);
	char input[16];
	while(1)
	{
		GCS->calculating_target();
		printf("Send target ? [y] Land quadcopter ? [l]");
		scanf("%s",input);
		if(strcmp(input,"y") == 0)
		{
			GCS->sending_target();
			GCS->wait_cmd_ack();
			GCS->send_exec_cmd_ack();
			GCS->wait_report();
		}
		else if(strcmp(input,"l") == 0)
			GCS->land_here();
	}
	return 0;
}
