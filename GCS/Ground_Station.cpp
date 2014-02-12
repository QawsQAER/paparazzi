#include "Ground_Station.h"
/*
Ground_Station::Ground_Station()
{
}
*/
Ground_Station::Ground_Station(char *port_name)
{
	this->Swarm_state = new Swarm();
	int fd = open(port_name, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		printf("Error: Cannot open the port\n");
		fprintf(stderr,"%s\n",strerror(errno));
		exit(0);
		//the program should stop immediately
	}
	else
	{
		set_interface_attribs(fd,B57600,0);
		set_blocking(fd,0);
		this->Com = new XBEE(fd,0x00000000,0x0000ffff,0);
	}
	this->GCS_state = 0;	
}

Ground_Station::~Ground_Station()
{
	delete this->Swarm_state;
	delete this->Com;
}

void Ground_Station::init_nav_quadcopters(uint8_t &AC_ID)
{
	//set home point here
	this->Send_Msg_Block(AC_ID, BLOCK_ID_HOME_POINT);
}

void Ground_Station::Send_Msg_Block(uint8_t &AC_ID, uint8_t BLOCK_ID)
{	
	pprz_msg data;
	data.pprz_set_block(AC_ID,BLOCK_ID);
	data.show_hex();
	XBEE_msg msg;
	Swarm *temp = this->Swarm_state;
	msg.set_tran_packet(temp->get_address_HI(AC_ID),temp->get_address_LO(AC_ID),0xFF,0xFE,data.pprz_get_data_ptr(),data.pprz_get_length());
	msg.show_hex();
	this->Com->XBEE_send_msg(msg);	
}


























