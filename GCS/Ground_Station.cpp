#include "Ground_Station.h"


Ground_Station::Ground_Station(char *port_name)
{
	printf("Creating Ground Control Station\n");
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
		printf("setting baud rate attribute\n");
		set_interface_attribs(fd,B57600,0);
		set_blocking(fd,0);
		this->Com = new XBEE(fd,0x00000000,0x0000ffff,0);
		printf("setting done\n");
	}
	this->GCS_state = GCS_INIT;	
	printf("Ground Control Station Created\n\n");
}

Ground_Station::~Ground_Station()
{
	delete this->Swarm_state;
	delete this->Com;
}

void Ground_Station::init_nav_quadcopters(uint8_t &AC_ID)
{
	//set home point here
	//this->Send_Msg_Block(AC_ID, BLOCK_ID_HOME_POINT);
	//wait for quad to report their state (pos)
	printf("Initiliziing quadcopter %d\n",AC_ID);
	bool telemetry_set = 0;
	bool module_set = 1;
	printf("quadcopter %d: setting telemetry mode && module\n",AC_ID);
	while((telemetry_set && module_set) == 0) 
	{
		Com->XBEE_read_into_recv_buff();
		Com->XBEE_parse_XBEE_msg();
		//while there're multiple XBEE message
		while(!Com->msg.empty())
		{
			XBEE_msg *ptr = Com->msg.front();
			Com->msg.pop();
			pprz_msg data = ptr->get_pprz_msg();
			//if recevived any message not quad_swarm_ack, quad_swarm_msg or DL_VALUE 
			if(data.pprz_get_msg_id() != RECV_MSG_ID_quad_swarm_ack && data.pprz_get_msg_id() != RECV_MSG_ID_DL_VALUE)
			{
					
					pprz_msg dl_setting;
					uint8_t ac_id = data.pprz_read_byte();
					uint8_t index = 1;
					float value = 14;
					dl_setting.pprz_set_DL_SETTING(ac_id,index,value);
					XBEE_msg set_dl;
					//hard coded address for easy debugging
					uint32_t addr_hi = 0x0013a200;
					uint32_t addr_lo = 0x409c278a;
					uint16_t net_addr_hi = 0xff;
					uint16_t net_addr_lo = 0xfe;
					set_dl.set_tran_packet(addr_hi,\
											addr_lo,\
											net_addr_hi,\
											net_addr_lo,\
											dl_setting.pprz_get_data_ptr(),\
											dl_setting.pprz_get_length());
					Com->XBEE_send_msg(set_dl);
			}
			else
			{
					switch(data.pprz_get_msg_id())
					{
						case(RECV_MSG_ID_DL_VALUE):
							{
								uint8_t cur_ac_id = 0,index = 0;
								float value = 0;
								data.pprz_get_DL_VALUE(cur_ac_id,index,value);
								printf("MSG: DL_VALUE\n");
								printf("ac_id %d, index %d, value %f\n",cur_ac_id,index,value);
								if(int(value) == 14)
								{
									printf("quad %d has received the setting command\n",AC_ID);
									telemetry_set = 1;
								}
							}
							break;
						case(RECV_MSG_ID_quad_swarm_ack):
							{
								//printf("MSG: quad_swarm_ack\n");
								uint8_t ac_id, quad_swarm_id, quad_swarm_ack;
								data.pprz_get_quad_swarm_ack(ac_id,quad_swarm_id,quad_swarm_ack);	
								//printf("ac_id %d,\nquad_swarm_id %d,\nquad_swarm_ack %d\n\n",ac_id,quad_swarm_id,quad_swarm_ack);
							}
							break;
						default:
							{
								printf("MSG_ID %d does not match any\n\n",data.pprz_get_msg_id());
							}
							break;
				}
			}
			delete ptr;
		}
	}	
	printf("quadcopter %d initlization done\n",AC_ID);
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


























