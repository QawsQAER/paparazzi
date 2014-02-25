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

void Ground_Station::init_quadcopters()
{
	printf("Initiliziing quadcopters\n");
	bool all_init = 0;
	QuadState s = SWARM_NEGOTIATE_REF;
	while(all_init == 0) 
	{
		Com->XBEE_read_into_recv_buff();
		Com->XBEE_parse_XBEE_msg();
		//while there're multiple XBEE message
		while(!Com->msg.empty())
		{
			XBEE_msg *ptr = Com->msg.front();
			Com->msg.pop();
			pprz_msg data = ptr->get_pprz_msg();
			uint8_t sender_id = *data.pprz_get_data_ptr();
			//if recevived any message not 	quad_swarm_ack, quad_swarm_msg or DL_VALUE
			//set the sender quadcopter telemetry mode to no message
			uint8_t msg_id = data.pprz_get_msg_id();
			if(msg_id != RECV_MSG_ID_quad_swarm_ack && msg_id != RECV_MSG_ID_DL_VALUE && msg_id != 0 && msg_id != RECV_MSG_ID_quad_swarm_report)
			{
					printf("MSG_ID is %d\n",data.pprz_get_msg_id());
					printf("setting telemetry mode of quad %d\n",sender_id);
					pprz_msg dl_setting;
					uint8_t index = DL_SETTING_TELEMETRY;
					float value = 1;
					dl_setting.pprz_set_DL_SETTING(sender_id,index,value);
					XBEE_msg set_dl;
					uint32_t addr_hi = this->Swarm_state->get_address_HI(sender_id);
					uint32_t addr_lo = this->Swarm_state->get_address_LO(sender_id);
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
								printf("sender_id %d, index %d, value %f\n",cur_ac_id,index,value);
								if(int(value) == 14)
								{
									printf("quad %d has received the setting command\n",cur_ac_id);
								}
							}
							break;
						case(RECV_MSG_ID_quad_swarm_ack):
							{
								uint8_t ac_id, quad_swarm_id, quad_swarm_ack;
								data.pprz_get_quad_swarm_ack(ac_id,quad_swarm_id,quad_swarm_ack);
								if(quad_swarm_ack == 0)
								{
									data.show_hex();
									printf("quad %d: not yet in AP_MODE_NAV\n",quad_swarm_id);
								}
								else if(quad_swarm_ack == 1)
							{
									printf("quad %d: not yet in geo_init block\n",quad_swarm_id);
								}else if(quad_swarm_ack == 2)
								{
									this->Swarm_state->set_quad_state(quad_swarm_id,s);
									printf("quad %d: ready\n",quad_swarm_id);
								}
							}
							break;
						case(RECV_MSG_ID_quad_swarm_report):	
							{
								struct quad_swarm_report report;
								data.pprz_get_quad_swarm_report(report);
								printf("MSG: quad_swarm_report\n");
								printf("quad %d: ap_mode %d state %d\n",report.ac_id,report.ap_mode,report.state);
								printf("quad %d: x %d y %d z%d\n\n",report.ac_id,report.x,report.y,report.z);
								this->update_on_quad_swarm_report(report);
								if(report.state == 2)
								{
									printf("setting state of %d\n",report.ac_id);
									this->Swarm_state->set_quad_state(report.ac_id,s);
								}
							}
							break;
						default:
							{
								//printf("MSG_ID %d does not match any\n\n",data.pprz_get_msg_id());
							}
							break;
				}
				delete ptr;
			}
			//Check State of Swarm
			all_init = this->Swarm_state->all_in_state(s);
		}
	}	
	printf("quadcopters initlization done\n");
}

void Ground_Station::negotiate_ref()
{
	printf("\n\n\n************************\n***********************\n");
	printf("\nnegotiating reference point\n");
	while(1)
	{
		Com->XBEE_read_into_recv_buff();
		Com->XBEE_parse_XBEE_msg();
		//while there're multiple XBEE message
		//handling with message
		while(!Com->msg.empty())
		{
			XBEE_msg *ptr = Com->msg.front();
			Com->msg.pop();
			pprz_msg data = ptr->get_pprz_msg();
			uint8_t msg_id = data.pprz_get_msg_id();
			if(msg_id == RECV_MSG_ID_quad_swarm_report)
			{
				struct quad_swarm_report report;
				data.pprz_get_quad_swarm_report(report);
				printf("MSG: quad_swarm_report\n");
				printf("quad %d: ap_mode %d state %d\n",report.ac_id,report.ap_mode,report.state);
				printf("quad %d: x %d y %d z %d\n\n",report.ac_id,report.x,report.y,report.z);
				printf("quad %d: pacc %d\n",report.ac_id,report.pacc);
				if(report.pacc/100 > 5)
				continue;
				else
				{
					this->update_on_quad_swarm_report(report);
				}
			}
			else if(msg_id == RECV_MSG_ID_quad_swarm_ack)
			{
				uint8_t cur_ac_id = 0;
				uint8_t quad_swarm_id = 0;
				uint8_t quad_swarm_ack = 0;
				data.pprz_get_quad_swarm_ack(cur_ac_id,quad_swarm_id,quad_swarm_ack);
				//if received ack telling GCS that the quad is in SWARM_WAIT_CMD
				//and nav_block = NAV_HOLDING_POINT
				if(quad_swarm_ack == 3)
				this->Swarm_state->set_quad_state(quad_swarm_id,SWARM_WAIT_CMD);
			}
		}
		//message handling done
		
		if(this->Swarm_state->all_in_state(SWARM_WAIT_CMD))
		{
			printf("All quads are waiting for command to start engine");
			printf("start engine?\n");
			char cmd[16];
			scanf("%s",cmd);
			if(strcmp(cmd,"y") == 0)
			{
				this->nav_start_engine();
				break;
			}else
			printf("Your command is %s\n",cmd);
		}
	}
	wait_all_quads(SWARM_WAIT_CMD_START_ENGINE);
	char cmd[16];
	memset(cmd,0,sizeof(char) * 16);
	while(strcmp(cmd,"y") != 0)
	{
		printf("now all quads have started engine, takeoff? [y/n] :");
		scanf("%s",cmd);
	}
	printf("taking off\n");
	this->nav_takeoff();
	printf("take off command sent\n");
	wait_all_quads(SWARM_WAIT_CMD_TAKEOFF);
}

void Ground_Station::calculating_target()
{
	//should be calculating intermediate targets here
	return ;
}

void Ground_Station::sending_target()
{
	//should be sending intermediate targets here
	this->wait_all_quads(SWARM_SEND_ACK);
	return ;	
}

void Ground_Station::wait_cmd_ack()
{
	//should be waiting for ack for command from quads
	return ;
}

void Ground_Station::wait_all_quads(QuadState s)
{
	while(!this->Swarm_state->all_in_state(s))
	{
		//read any buffer
		Com->XBEE_read_into_recv_buff();
		Com->XBEE_parse_XBEE_msg();
		//then update state
		while(!Com->msg.empty())
		{
			XBEE_msg *ptr = Com->msg.front();
			Com->msg.pop();
			pprz_msg data = ptr->get_pprz_msg();
			uint8_t msg_id = data.pprz_get_msg_id();
			if(msg_id == RECV_MSG_ID_quad_swarm_ack)
			{
				uint8_t cur_ac_id = 0;
				uint8_t quad_swarm_id = 0;
				uint8_t quad_swarm_ack = 0;
				data.pprz_get_quad_swarm_ack(cur_ac_id,quad_swarm_id,quad_swarm_ack);
				switch(quad_swarm_ack)
				{
					case(0):
					{this->Swarm_state->set_quad_state(quad_swarm_id, SWARM_KILLED);}
					break;
					
					case(3):
					{this->Swarm_state->set_quad_state(quad_swarm_id, SWARM_WAIT_CMD);}
					break;

					case(4):
					{this->Swarm_state->set_quad_state(quad_swarm_id, SWARM_WAIT_CMD_START_ENGINE);}
					break;
		
					case(5):
					{this->Swarm_state->set_quad_state(quad_swarm_id, SWARM_WAIT_CMD_TAKEOFF);}
					break;
					
					case(6):
					{this->Swarm_state->set_quad_state(quad_swarm_id,SWARM_SEND_ACK);}
					break;
					default:
					break;
				}			
			}
			else if(msg_id == RECV_MSG_ID_quad_swarm_report)
			{
				struct quad_swarm_report report;
				data.pprz_get_quad_swarm_report(report);
				this->update_on_quad_swarm_report(report);
			}
		}
		switch(s)
		{
			case(SWARM_KILLED):
			{
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(this->Swarm_state->get_state(count) != SWARM_KILLED)
					{
						this->ap_kill_quadcopter(count);
					}
				}
			}
			break;
			case(SWARM_WAIT_CMD_START_ENGINE):
			{
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(this->Swarm_state->get_state(count) != SWARM_WAIT_CMD_TAKEOFF)
					{
						this->nav_start_engine(count);
					}
				}
			}
			break;
			//waiting all quads to enter SWARM_WAIT_CMD_TAKEOFF state
			case(SWARM_WAIT_CMD_TAKEOFF):
			{
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(this->Swarm_state->get_state(count) != SWARM_WAIT_CMD_TAKEOFF)
					{
						this->nav_takeoff(count);
					}
				}
			}
			break;
			case(SWARM_SEND_ACK):
			{
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(this->Swarm_state->get_state(count) != SWARM_WAIT_CMD_TAKEOFF)
					{
						//TODO 
						//send the target of this quadcopter
					}
				}
			}
			break;
			default:
			{
			}
			break;
		}
	}
}

void Ground_Station::Send_Msg_Block(uint8_t &AC_ID, uint8_t &BLOCK_ID)
{	
	pprz_msg data;
	data.pprz_set_block(AC_ID,BLOCK_ID);
	XBEE_msg msg;
	Swarm *temp = this->Swarm_state;
	msg.set_tran_packet(temp->get_address_HI(AC_ID),temp->get_address_LO(AC_ID),0xFF,0xFE,data.pprz_get_data_ptr(),data.pprz_get_length());
	this->Com->XBEE_send_msg(msg);	
}
void Ground_Station::send_ack(uint8_t AC_ID, uint8_t ack)
{
	pprz_msg pprz_ack;
	pprz_ack.pprz_set_ack(AC_ID,ack);
	XBEE_msg msg_ack;
        uint32_t addr_hi = this->Swarm_state->get_address_HI(AC_ID);
        uint32_t addr_lo = this->Swarm_state->get_address_LO(AC_ID);
        uint16_t net_addr_hi = 0xff;
        uint16_t net_addr_lo = 0xfe;
        msg_ack.set_tran_packet(addr_hi,\
                              addr_lo,\
                              net_addr_hi,\
                              net_addr_lo,\
                              pprz_ack.pprz_get_data_ptr(),\
                              pprz_ack.pprz_get_length());
	Com->XBEE_send_msg(msg_ack);	
}

void Ground_Station::nav_start_engine()
{
	uint8_t count = 1;
	for(count = 1;count < QUAD_NB + 1;count++)
		this->nav_start_engine(count);
}

void Ground_Station::nav_start_engine(uint8_t AC_ID)
{
	uint8_t block_id = BLOCK_ID_START_ENGINE;
	this->Send_Msg_Block(AC_ID,block_id);
}

void Ground_Station::nav_takeoff()
{
	uint8_t count = 1;
	for(count = 1;count < QUAD_NB + 1;count++)
		this->nav_takeoff(count);
}
void Ground_Station::nav_takeoff(uint8_t AC_ID)
{
	uint8_t block_id = BLOCK_ID_TAKE_OFF;
	this->Send_Msg_Block(AC_ID,block_id);
}
void Ground_Station::takeoff_quadcopters()
{
	
}

void Ground_Station::takeoff_quadcopter(uint8_t AC_ID)
{

}
void Ground_Station::ap_kill_quadcopter(uint8_t AC_ID)
{
	this->send_ack(AC_ID,(uint8_t) 255);
}

void Ground_Station::ap_kill_quadcopter()
{
	uint8_t count = 1;
	for(count = 1;count < QUAD_NB;count++)
	{
		ap_kill_quadcopter(count);
	}
}
void Ground_Station::ap_nav_quadcopter(uint8_t AC_ID)
{
	this->send_ack(AC_ID,(uint8_t) 254);	
}

void Ground_Station::update_on_quad_swarm_report(quad_swarm_report report)
{
	struct EcefCoor_i pos;
	pos.x = report.x;
	pos.y = report.y;
	pos.z = report.z;
	this->Swarm_state->set_quad_ecef(report.ac_id,pos);
	this->Swarm_state->set_quad_pacc(report.ac_id,report.pacc);
}
















