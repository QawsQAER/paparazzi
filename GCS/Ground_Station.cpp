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
			//if recevived any message not quad_swarm_ack, quad_swarm_msg or DL_VALUE
			//set the sender quadcopter telemetry mode to no message
			uint8_t msg_id = data.pprz_get_msg_id();
			if(msg_id != RECV_MSG_ID_quad_swarm_ack && msg_id != RECV_MSG_ID_DL_VALUE && msg_id != 0 && msg_id != RECV_MSG_ID_quad_swarm_report)
			{
				//data.show_hex();
				printf("MSG_ID is %d\n",data.pprz_get_msg_id());
				printf("setting telemetry mode of quad %d\n",sender_id);
				pprz_msg dl_setting;
				uint8_t index = DL_SETTING_TELEMETRY;
				float value = 1;
				dl_setting.pprz_set_DL_SETTING(sender_id,index,value);
				XBEE_msg set_dl;
				uint32_t addr_hi = this->Swarm_state->get_address_HI(sender_id);
				uint32_t addr_lo = this->Swarm_state->get_address_LO(sender_id);
				uint8_t net_addr_hi = XBEE_NET_ADDR_HI;
				uint8_t net_addr_lo = XBEE_NET_ADDR_LO;
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
					if(int(value) == 1)
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
						//data.show_hex();
						printf("quad%d: not yet in AP_MODE_NAV\n",quad_swarm_id);
					}
					else if(quad_swarm_ack == 1)
					{printf("quad %d: not yet in geo_init block\n",quad_swarm_id);}
					else if(quad_swarm_ack == 2)
					{
						this->Swarm_state->set_quad_state(quad_swarm_id,SWARM_NEGOTIATE_REF);
						printf("quad %d: ready\n",quad_swarm_id);
					}
				}
				break;
				case(RECV_MSG_ID_quad_swarm_report):
				{
					struct quad_swarm_report report;
					data.pprz_get_quad_swarm_report(report);
					GCS_SHOW_REPORT(report);	
					this->update_on_quad_swarm_report(report);
					this->Swarm_state->set_quad_state(report.ac_id,report.state);
					if(report.state == SWARM_KILLED)
					{
						printf("init quad %d ?[y/n] \n",report.ac_id);
						char cmd[16];
						scanf("%s",cmd);
						if(strcmp(cmd,"y") == 0)
						{
							uint8_t ack = 0xfe;
							this->send_ack(report.ac_id,ack);
						}
					}
				}
				break;
				default:
				break;
			}
		delete ptr;
		}
		//Check State of Swarm
		all_init = this->Swarm_state->all_in_state(SWARM_NEGOTIATE_REF);
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
				GCS_SHOW_REPORT(report);
				if(report.pacc/100 > 20)
				continue;
				else
				{
					if(report.state == SWARM_NEGOTIATE_REF)
					{
						//the quadcopter should proceed to SWARM_WAIT_CMD
						uint8_t ack = 2;
						this->update_on_quad_swarm_report(report);
						this->send_ack(report.ac_id,ack);
					}else if(report.state == SWARM_WAIT_CMD)
					{
						this->update_on_quad_swarm_report(report);
						this->Swarm_state->set_quad_state(report.ac_id,report.state);
					}
				}
			}
		}
		//message handling done
		
		//if all quadcopter are in SWARM_WAIT_CMD state
		if(this->Swarm_state->all_in_state(SWARM_WAIT_CMD))
		{
			printf("All quads are waiting for command to start engine\n");
			printf("start engine?[y/n]\n");
			char cmd[16];
			scanf("%s",cmd);
			if(strcmp(cmd,"y") == 0)
			{
				//send navigation change block message to all quadcopters
				this->nav_start_engine();
				//stop the while(1) loop.
				break;
			}else
			printf("Your command is %s\n",cmd);
		}
	}
	
	//find the positioning info with the smallest error
	uint8_t count_ac = 0;
	uint8_t min = 1;
	for(count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		if(this->Swarm_state->get_pacc(min) > this->Swarm_state->get_pacc(count_ac))
			min = count_ac;
	}
	//set the Ref as the one with smallest error
	struct EcefCoor_i min_pos = this->Swarm_state->get_quad_coor(min);
	ltp_def_from_ecef_i(&(this->ref),&min_pos);
	printf("the ref ecef is %d %d %d \n",ref.ecef.x,ref.ecef.y,ref.ecef.z);
	this->update_ned_coor_by_ecef_coor();
	
	//wait for all quadcopters to be in SWARM_WAIT_CMD_START_ENGINE state
	wait_all_quads(SWARM_WAIT_CMD_START_ENGINE);
	char cmd[16];
	memset(cmd,0,16);
	while(strcmp(cmd,"y") == 0)
	{
		printf("now all quads have started engine, takeoff? [y/n] :");
		scanf("%s",cmd);
	}

	//send navigation change block message to all quadcopters
	this->nav_takeoff();
	printf("taking off\n");
	//wait for all quadcopters to be in SWARM_WAIT_CMD_TAKEOFF state 
	wait_all_quads(SWARM_WAIT_CMD_TAKEOFF);
	printf("all quadcopters has taken off\n");
	//then return of this function
	return ;
}

void Ground_Station::calculating_target()
{
	//should be calculating intermediate targets here
	struct NedCoor_i ned_tar[QUAD_NB + 1];
	//the 1st quadcopter should proceed to north with 0.5 meters
	ned_tar[1].x = 100 + POS_FLOAT_OF_BFP(this->ned_pos[1].x) * 100;//add 50cm to x (North)
	ned_tar[1].y = POS_FLOAT_OF_BFP(this->ned_pos[1].y) * 100;//keep the original y
	ned_tar[1].z = POS_FLOAT_OF_BFP(this->ned_pos[1].z) * 100;//keep the original z
	//the 2nd quadcopter should be 3 meters south to the 1st quad	
	#if QUAD_NB >= 2
	ned_tar[2].x = ned_tar[1].x - 300;
	ned_tar[2].y = ned_tar[1].y;
	ned_tar[2].z = ned_tar[1].z;
	#endif
	for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		ecef_of_ned_point_i(&(this->target[count_ac]),&(this->ref),&ned_tar[count_ac]);
	}
	return ;
}

void Ground_Station::sending_target()
{
	//should be sending intermediate targets here
	for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		this->send_target(1,&target[1]);
		printf("quad %d target sent\n",count_ac);
	}
	return ;	
}

void Ground_Station::wait_cmd_ack()
{
	this->wait_all_quads(SWARM_SEND_ACK);
	printf("all quads have received their targets\n");
	//should be waiting for ack for command from quads
	printf("all quads are waiting for cmd ack for exec\n");
	return ;
}

void Ground_Station::send_exec_cmd_ack()
{
	printf("send_exec_cmd_ack\n");
	wait_all_quads(SWARM_EXEC_CMD);
	return ;
}

void Ground_Station::wait_report()
{
	printf("waiting report\n");
	wait_all_quads(SWARM_REPORT_STATE);
	return ;
}
void Ground_Station::wait_all_quads(uint8_t s)
{
	while(!this->Swarm_state->all_in_state(s))
	{
		if(s == SWARM_SEND_ACK)
		{
			if(this->Swarm_state->all_in_state(SWARM_WAIT_EXEC_ACK))
			{break;}
		}
		if(s == SWARM_EXEC_CMD)
		{
			if(this->Swarm_state->all_in_state(SWARM_REPORT_STATE))
			break;
		}
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
			if(msg_id == 159)
			{
				//if received navigation info, just used for debugging
				printf("-----------------\n");
				data.show_hex();
				data.pprz_read_byte();
				data.pprz_read_byte();
				uint16_t block_time = data.pprz_read_2bytes();
				uint16_t stage_time = data.pprz_read_2bytes();
				uint8_t cur_block = data.pprz_read_byte();
				uint8_t cur_stage = data.pprz_read_byte();
				printf("block time %d stage time %d cur_block %d cur_stage %d\n",block_time,stage_time,cur_block,cur_stage);
				printf("------------------\n");	
			}
			else if(msg_id == RECV_MSG_ID_quad_swarm_report)
			{
				struct quad_swarm_report report;
				data.pprz_get_quad_swarm_report(report);
				this->update_on_quad_swarm_report(report);
				this->Swarm_state->set_quad_state(report.ac_id,report.state);
				this->Swarm_state->set_quad_pacc(report.ac_id,report.pacc);
				this->update_ned_coor_by_ecef_coor(report.ac_id);				
				struct NedCoor_i pos = ned_pos[report.ac_id];
				
				printf("quad %d in state %d\n",report.ac_id,report.state);
				printf("quad %d ned.x %f ned.y %f ned.z %f\n",report.ac_id,POS_FLOAT_OF_BFP(pos.x),POS_FLOAT_OF_BFP(pos.y),POS_FLOAT_OF_BFP(pos.z));
			
				#if QUAD_NB >= 2
				double dis_x = POS_FLOAT_OF_BFP(ned_pos[1].x) - POS_FLOAT_OF_BFP(ned_pos[2].x);
				double dis_y = POS_FLOAT_OF_BFP(ned_pos[1].y) - POS_FLOAT_OF_BFP(ned_pos[2].y);
				double dis_z = POS_FLOAT_OF_BFP(ned_pos[1].z) - POS_FLOAT_OF_BFP(ned_pos[2].z);
				double total = (dis_x) * (dis_x) + (dis_y) * (dis_y);
				uint8_t ac1 = 1;
				uint8_t ac2 = 2;
				printf("distance x %f y %f z %f total %f\nquad 1 pacc  %d quad 2 pacc  %d\n\n",\
						dis_x,dis_y,dis_z,sqrt(total),\
						this->Swarm_state->get_pacc(ac1),\
						this->Swarm_state->get_pacc(ac2));
				#endif
			}
		}
		switch(s)
		{
			case(SWARM_KILLED):
			{
				//all quadcopters should be killed
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
				//all quadcopters should be in state SWARM_WAIT_CMD_START_ENGINE 3, from SWARM_WAIT_CMD 2
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(this->Swarm_state->get_state(count) != SWARM_WAIT_CMD_START_ENGINE)
					{
						printf("trying to start engine quad %d\n",count);
						this->nav_start_engine(count);
					}
				}
			}
			break;
			case(SWARM_WAIT_CMD_TAKEOFF):
			{
				//all quadcopters should be in state SWARM_WAIT_CMD_TAKEOFF 4, from SWARM_WAIT_START_ENGINE 3
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
				//all quadcopters should be in state SWARM_SEND_ACK 5, from SWARM_WAIT_TAKEOFF 4
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(this->Swarm_state->get_state(count) != SWARM_SEND_ACK && this->Swarm_state->get_state(count) != SWARM_WAIT_EXEC_ACK)
					{
						//TODO 
						//send the target of this quadcopter
						this->send_target(count,&target[count]);
						printf("resending target to quad %d\n",count);
					}
				}
			}
			break;
			case(SWARM_EXEC_CMD):
			{
				//all quadcopters should be executing the command sent by GCS
				for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
				{
					uint8_t quad_state = this->Swarm_state->get_state(count_ac);
					if(quad_state != SWARM_EXEC_CMD && quad_state != SWARM_REPORT_STATE)
					{
						uint8_t ack_for_exec_cmd = 3;
						this->send_ack(count_ac,ack_for_exec_cmd);
						printf("sending exec_ack to quad %d\n",count_ac);
					}
				}
			}
			break;
			case(SWARM_REPORT_STATE):
			{
				//you can do nothing if the quad has not reach the target destination
			}
			break;
			default:
			{
			}
			break;
		}
	}
}

void Ground_Station::send_target(uint8_t AC_ID, struct EcefCoor_i * tar)
{
	struct quad_swarm_msg content;
	content.ac_id = AC_ID;
	content.dummy = 0;
	content.x = tar->x;
	content.y = tar->y;
	content.z = tar->z; 
	pprz_msg frame;
	frame.pprz_set_msg(AC_ID,content);
	XBEE_msg msg;
	Swarm *temp = this->Swarm_state;
	msg.set_tran_packet(temp->get_address_HI(AC_ID),temp->get_address_LO(AC_ID),0xFF,0xFE,frame.pprz_get_data_ptr(),frame.pprz_get_length());
	this->Com->XBEE_send_msg(msg);
	printf("sending target to quad %d\n",AC_ID);
}
void Ground_Station::Send_Msg_Block(uint8_t &AC_ID, uint8_t &BLOCK_ID)
{	
	pprz_msg data;
	data.pprz_set_block(AC_ID,BLOCK_ID);
	XBEE_msg msg;
	Swarm *temp = this->Swarm_state;
	msg.set_tran_packet(temp->get_address_HI(AC_ID),temp->get_address_LO(AC_ID),0xFF,0xFE,data.pprz_get_data_ptr(),data.pprz_get_length());
	//msg.show_hex();
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

//TODO implement land here functionality
void Ground_Station::land_here()
{
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

void Ground_Station::update_ned_coor_by_ecef_coor()
{
	uint8_t count_ac = 1;
	for(count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		struct EcefCoor_i pos = this->Swarm_state->get_quad_coor(count_ac);
		ned_of_ecef_pos_i(&ned_pos[count_ac],&ref,&(pos));
	}
}

void Ground_Station::update_ned_coor_by_ecef_coor(uint8_t AC_ID)
{
	struct EcefCoor_i pos = this->Swarm_state->get_quad_coor(AC_ID);
	ned_of_ecef_pos_i(&ned_pos[AC_ID],&ref,&(pos));
}










