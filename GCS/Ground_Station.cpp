#include "Ground_Station.h"

char state_string[12][32];
Swarm* Ground_Station::Swarm_state = NULL;
XBEE* Ground_Station::Com = NULL;
GroundControlStation_state Ground_Station::GCS_state;
struct LtpDef_i Ground_Station::ref;
struct EcefCoor_i Ground_Station::target[QUAD_NB + 1];
struct NedCoor_i Ground_Station::ned_pos[QUAD_NB + 1];
GUI* Ground_Station::GCS_GUI = NULL;

Ground_Station::Ground_Station(char *port_name, int argc, char **argv)
{
	strcpy(state_string[0],"SWARM_INIT");
	strcpy(state_string[1],"SWARM_NEGOTIATE_REF");
	strcpy(state_string[2],"SWARM_WAIT_CMD");
	strcpy(state_string[3],"SWARM_WAIT_CMD_START_ENGINE");
	strcpy(state_string[4],"SWARM_WAIT_CMD_TAKEOFF");
	strcpy(state_string[5],"SWARM_SEND_ACK");
	strcpy(state_string[6],"SWARM_WAIT_EXEC_ACK");
	strcpy(state_string[7],"SWARM_EXEC_CMD");
	strcpy(state_string[8],"SWARM_REPORT_STATE");
	strcpy(state_string[9],"SWARM_LANDHERE");
	strcpy(state_string[10],"SWARM_LANDED");
	strcpy(state_string[11],"SWARM_KILLED");

	printf("Creating Ground Control Station\n");
	Swarm_state = new Swarm();
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
		Com = new XBEE(fd,0x00000000,0x0000ffff,0);
		printf("setting done\n");
	}
	GCS_state = GCS_INIT;
	printf("Ground Control Station Created\n\n");
	GCS_GUI = new GUI(argc,argv);

	//Add event listener for the buttons.
	GCS_GUI->button_add_event_listener(GCS_GUI->quad_control_panel.button_init,init_quadcopters,(void *) &GCS_busy);
	GCS_GUI->button_add_event_listener(GCS_GUI->quad_control_panel.button_start_engine,start_engine,(void *) &GCS_busy);
	GCS_GUI->button_add_event_listener(GCS_GUI->quad_control_panel.button_takeoff,takeoff,(void *) &GCS_busy);
	GCS_GUI->button_add_event_listener(GCS_GUI->quad_control_panel.button_landhere,nav_land_here,(void *)&GCS_busy);

}

Ground_Station::~Ground_Station()
{
  delete Swarm_state;
  delete Com;
}

//------------------------------------------------------------------//
//------------------------------------------------------------------//
//static void *init_quadcopters(void * arg);
//this function will try to initilize all the quadcopters to be in
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void *Ground_Station::init_quadcopters(void * arg)
{
	//invoke a thread
	pthread_t tid = 0;
	pthread_attr_t thread_attr;
	printf("creating thread for init\n");
	pthread_attr_init(&thread_attr);
	pthread_create(&tid,&thread_attr,init_quadcopters_thread,NULL);
	return NULL;
}

void *Ground_Station::init_quadcopters_thread( void * arg)
{
	int rev = 0;
	if((rev = pthread_mutex_trylock(&GCS_busy)) == 0)
	{
		//Critical Section, sending command, waitting to next stage.
		printf("init_quadcopters_thread: GCS_busy locking\n");
		wait_all_quads(SWARM_INIT);
		printf("init_quadcopters_thread: All quadcopters in SWARM_INIT\n");

		//waiting for all quadcopter to enter SWARM_WAIT_CMD stage
		wait_all_quads(SWARM_NEGOTIATE_REF);

		wait_all_quads(SWARM_WAIT_CMD);
		printf("init_quadcopters_thread: All quadcopters in SWARM_WAIT_CMD\n");

		printf("init_quadcopters_thread: GCS_busy unlocking\n");
		
		pthread_mutex_unlock(&GCS_busy);
	}
	else if (rev == EBUSY)
	{
		printf("init_quadcopters_thread ERROR: GCS_busy is locked\n");
	}
	else if(rev == EINVAL)
	{
		printf("init_quadcopters_thread ERROR: GCS_busy is not initilized\n");
	}
	else if(rev == EFAULT)
	{
		printf("init_quadcopters_thread ERROR: arg is not a valid pointer\n");
	}
	return NULL;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void *Ground_Station::start_engine(void * arg)
{
	pthread_t tid = 0;
	pthread_attr_t thread_attr;
	printf("creating thread for start_engine\n");
	pthread_attr_init(&thread_attr);
	pthread_create(&tid,&thread_attr,start_engine_thread,NULL);
	return NULL;
}

void *Ground_Station::start_engine_thread(void *arg)
{
	int rev = 0;
	if((rev = pthread_mutex_trylock(&GCS_busy)) == 0)
	{
		//Critical Section, sending command, waitting to next stage.
		printf("start_engine_thread: GCS_busy locking\n");
		wait_all_quads(SWARM_WAIT_CMD_START_ENGINE);

		printf("start_engine_thread: All quadcopters in SWARM_WAIT_START_ENGINE\n");

		printf("start_engine_thread: GCS_busy unlocking\n");
		
		pthread_mutex_unlock(&GCS_busy);
	}
	else if (rev == EBUSY)
	{
		printf("start_engine_thread ERROR: GCS_busy is locked\n");
	}
	else if(rev == EINVAL)
	{
		printf("start_engine_thread ERROR: GCS_busy is not initilized\n");
	}
	else if(rev == EFAULT)
	{
		printf("start_engine_thread ERROR: arg is not a valid pointer\n");
	}
	return NULL;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void *Ground_Station::takeoff(void *arg)
{
	pthread_t tid = 0;
	pthread_attr_t thread_attr;
	printf("creating thread for takeoff\n");
	pthread_attr_init(&thread_attr);
	pthread_create(&tid,&thread_attr,takeoff_thread,NULL);
	return NULL;
}

void *Ground_Station::takeoff_thread(void *arg)
{
	int rev = 0;
	if((rev = pthread_mutex_trylock(&GCS_busy)) == 0)
	{
		//Critical Section, sending command, waitting to next stage.
		printf("start_engine_thread: GCS_busy locking\n");
		wait_all_quads(SWARM_WAIT_CMD_TAKEOFF);

		printf("start_engine_thread: All quadcopters in SWARM_WAIT_CMD_TAKEOFF\n");

		printf("start_engine_thread: GCS_busy unlocking\n");
		
		pthread_mutex_unlock(&GCS_busy);


		//Set the origin of the coordination system.
	}
	else if (rev == EBUSY)
	{
		printf("start_engine_thread ERROR: GCS_busy is locked\n");
	}
	else if(rev == EINVAL)
	{
		printf("start_engine_thread ERROR: GCS_busy is not initilized\n");
	}
	else if(rev == EFAULT)
	{
		printf("start_engine_thread ERROR: arg is not a valid pointer\n");
	}
	return NULL;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::negotiate_ref()
{
	printf("\n\n\n************************\n***********************\n");
	printf("\nnegotiating reference point\n");

	while(1)
	{
		if(Swarm_state->all_in_state(SWARM_WAIT_CMD))
		{
			printf("All quads are waiting for command to start engine\n");
			printf("start engine?[y/n]\n");
			char cmd[16];
			scanf("%s",cmd);
			if(strcmp(cmd,"y") == 0)
			{
				//send navigation change block message to all quadcopters
				nav_start_engine();
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
		if(Swarm_state->get_pacc(min) > Swarm_state->get_pacc(count_ac))
			min = count_ac;
	}
	//set the Ref as the one with smallest error
	struct EcefCoor_i min_pos = Swarm_state->get_quad_coor(min);
	ltp_def_from_ecef_i(&(ref),&min_pos);
	printf("the ref ecef is %d %d %d \n",ref.ecef.x,ref.ecef.y,ref.ecef.z);
	update_ned_coor_by_ecef_coor();
	
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
	nav_takeoff();
	printf("taking off\n");
	//wait for all quadcopters to be in SWARM_WAIT_CMD_TAKEOFF state 
	wait_all_quads(SWARM_WAIT_CMD_TAKEOFF);
	printf("all quadcopters has taken off\n");
	//then return of this function
	return ;
}

//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::calculating_target()
{
	//should be calculating intermediate targets here
	struct NedCoor_i ned_tar[QUAD_NB + 1];
	//the 1st quadcopter should proceed to north with 0.5 meters
	ned_tar[1].x = 100 + POS_FLOAT_OF_BFP(ned_pos[1].x) * 100;//add 50cm to x (North)
	ned_tar[1].y = POS_FLOAT_OF_BFP(ned_pos[1].y) * 100;//keep the original y
	ned_tar[1].z = POS_FLOAT_OF_BFP(ned_pos[1].z) * 100;//keep the original z
	//the 2nd quadcopter should be 3 meters south to the 1st quad	
	#if QUAD_NB >= 2
	ned_tar[2].x = ned_tar[1].x - 300;
	ned_tar[2].y = ned_tar[1].y;
	ned_tar[2].z = ned_tar[1].z;
	#endif
	for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		ecef_of_ned_point_i(&(target[count_ac]),&(ref),&ned_tar[count_ac]);
	}
	return ;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::sending_target()
{
	//should be sending intermediate targets here
	for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		send_target(1,&target[1]);
		printf("quad %d target sent\n",count_ac);
	}
	return ;	
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::wait_cmd_ack()
{
	wait_all_quads(SWARM_SEND_ACK);
	printf("all quads have received their targets\n");
	//should be waiting for ack for command from quads
	printf("all quads are waiting for cmd ack for exec\n");
	return ;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::send_exec_cmd_ack()
{
	printf("send_exec_cmd_ack\n");
	wait_all_quads(SWARM_EXEC_CMD);
	return ;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::wait_report()
{
	printf("waiting report\n");
	wait_all_quads(SWARM_REPORT_STATE);
	return ;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::wait_all_quads(uint8_t s)
{
	struct timeval current_time;
	while(!Swarm_state->all_in_state(s))
	{
		if(s == SWARM_SEND_ACK)
		{
			if(Swarm_state->all_in_state(SWARM_WAIT_EXEC_ACK))
			{break;}
		}
		if(s == SWARM_EXEC_CMD)
		{
			if(Swarm_state->all_in_state(SWARM_REPORT_STATE))
			break;
		}

		switch(s)
		{
			case(SWARM_KILLED):
			{
				//all quadcopters should be killed
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					gettimeofday(&current_time,NULL); 
					if(Swarm_state->get_state(count) != SWARM_KILLED && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000))
					{
						ap_kill_quadcopter(count);
					}
				}
			}
			break;
			case(SWARM_INIT):
			{
				//printf("checking\n");
				//waiting for all quadcopter to be in SWARM_INIT state
				
				for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
				{
					gettimeofday(&current_time,NULL); 
					if(Swarm_state->get_state(count_ac) != SWARM_INIT && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000) )
					{
						//printf("sending 0xfe to quad %d\n",count_ac);
						send_ack(count_ac,0xfe);
					}
				}
			}
			break;
			case(SWARM_WAIT_CMD):
			{
				for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
				{
					gettimeofday(&current_time,NULL); 
					if(Swarm_state->get_state(count_ac) != SWARM_WAIT_CMD && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000))
					{
						send_ack(count_ac,2);
					}
				}
			}
			break;
			case(SWARM_WAIT_CMD_START_ENGINE):
			{
				//all quadcopters should be in state SWARM_WAIT_CMD_START_ENGINE 3, from SWARM_WAIT_CMD 2
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					gettimeofday(&current_time,NULL); 
					if(Swarm_state->get_state(count) != SWARM_WAIT_CMD_START_ENGINE && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000))
					{
						printf("trying to start engine quad %d\n",count);
						nav_start_engine(count);
					}
				}
			}
			break;
			case(SWARM_WAIT_CMD_TAKEOFF):
			{
				//all quadcopters should be in state SWARM_WAIT_CMD_TAKEOFF 4, from SWARM_WAIT_START_ENGINE 3
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					gettimeofday(&current_time,NULL); 
					if(Swarm_state->get_state(count) != SWARM_WAIT_CMD_TAKEOF && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000))
					{
						nav_takeoff(count);
					}
				}
			}
			break;
			case(SWARM_SEND_ACK):
			{
				//all quadcopters should be in state SWARM_SEND_ACK 5, from SWARM_WAIT_TAKEOFF 4
				for(uint8_t count = 1;count < QUAD_NB + 1;count++)
				{
					if(Swarm_state->get_state(count) != SWARM_SEND_ACK && Swarm_state->get_state(count) != SWARM_WAIT_EXEC_ACK && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000))
					{
						//TODO 
						//send the target of this quadcopter
						send_target(count,&target[count]);
						printf("resending target to quad %d\n",count);
					}
				}
			}
			break;
			
			case(SWARM_EXEC_CMD):
			{
				uint8_t quad_state = 0;
				//all quadcopters should be executing the command sent by GCS
				for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
				{
					quad_state = Swarm_state->get_state(count_ac);
					if(quad_state != SWARM_EXEC_CMD && quad_state != SWARM_REPORT_STATE && (current_time.tv_usec - Swarm_state->get_last_modified(count_ac)->tv_usec > 1000))					{
					{
						uint8_t ack_for_exec_cmd = 3;
						send_ack(count_ac,ack_for_exec_cmd);
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

			case(SWARM_LANDED):
			{
				uint8_t quad_state = 0;
				uint8_t block_id_landhere = BLOCK_ID_LAND_HERE;
				for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
				{
					quad_state = Swarm_state->get_state(count_ac);
					if(quad_state != SWARM_LANDHERE && quad_state != SWARM_LANDED)
					{
						Send_Msg_Block(count_ac,block_id_landhere);
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
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
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
	Swarm *temp = Swarm_state;
	msg.set_tran_packet(temp->get_address_HI(AC_ID),temp->get_address_LO(AC_ID),0xFF,0xFE,frame.pprz_get_data_ptr(),frame.pprz_get_length());
	Com->XBEE_send_msg(msg);
	printf("sending target to quad %d\n",AC_ID);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::Send_Msg_Block(uint8_t &AC_ID, uint8_t &BLOCK_ID)
{	
	pprz_msg data;
	data.pprz_set_block(AC_ID,BLOCK_ID);
	XBEE_msg msg;
	Swarm *temp = Swarm_state;
	msg.set_tran_packet(temp->get_address_HI(AC_ID),temp->get_address_LO(AC_ID),0xFF,0xFE,data.pprz_get_data_ptr(),data.pprz_get_length());
	//msg.show_hex();
	Com->XBEE_send_msg(msg);	
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::send_ack(uint8_t AC_ID, uint8_t ack)
{
	pprz_msg pprz_ack;
	pprz_ack.pprz_set_ack(AC_ID,ack);
	XBEE_msg msg_ack;
    uint32_t addr_hi = Swarm_state->get_address_HI(AC_ID);
    uint32_t addr_lo = Swarm_state->get_address_LO(AC_ID);
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
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::nav_start_engine()
{
	uint8_t count = 1;
	for(count = 1;count < QUAD_NB + 1;count++)
		nav_start_engine(count);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::nav_start_engine(uint8_t AC_ID)
{
	uint8_t block_id = BLOCK_ID_START_ENGINE;
	Send_Msg_Block(AC_ID,block_id);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::nav_takeoff()
{
	uint8_t count = 1;
	for(count = 1;count < QUAD_NB + 1;count++)
		nav_takeoff(count);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::nav_takeoff(uint8_t AC_ID)
{
	uint8_t block_id = BLOCK_ID_TAKE_OFF;
	Send_Msg_Block(AC_ID,block_id);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//

//TODO implement land here functionality
void* Ground_Station::nav_land_here(void * arg)
{
	pthread_t tid = 0;
	pthread_attr_t thread_attr;
	printf("creating thread for landhere\n");
	pthread_attr_init(&thread_attr);
	pthread_create(&tid,&thread_attr,nav_land_here_thread,NULL);
	return NULL;
}

void* Ground_Station::nav_land_here_thread(void * arg)
{
	int rev = 0;
	if((rev = pthread_mutex_trylock(&GCS_busy)) == 0)
	{
		//Critical Section, sending command, waitting to next stage.
		printf("nav_land_here_thread: GCS_busy locking\n");
		wait_all_quads(SWARM_LANDED);

		printf("nav_land_here_thread: All quadcopters in SWARM_WAIT_CMD_TAKEOFF\n");

		printf("nav_land_here_thread: GCS_busy unlocking\n");
		
		pthread_mutex_unlock(&GCS_busy);


		//Set the origin of the coordination system.
	}
	else if (rev == EBUSY)
	{
		printf("nav_land_here_thread ERROR: GCS_busy is locked\n");
	}
	else if(rev == EINVAL)
	{
		printf("nav_land_here_thread ERROR: GCS_busy is not initilized\n");
	}
	else if(rev == EFAULT)
	{
		printf("nav_land_here_thread ERROR: arg is not a valid pointer\n");
	}
	return NULL;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::takeoff_quadcopters()
{
	
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::takeoff_quadcopter(uint8_t AC_ID)
{

}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::ap_kill_quadcopter(uint8_t AC_ID)
{
	send_ack(AC_ID,(uint8_t) 255);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::ap_kill_quadcopter()
{
	uint8_t count = 1;
	for(count = 1;count < QUAD_NB;count++)
	{
		ap_kill_quadcopter(count);
	}
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::ap_nav_quadcopter(uint8_t AC_ID)
{
	send_ack(AC_ID,(uint8_t) 254);	
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::update_on_quad_swarm_report(quad_swarm_report report)
{
	struct EcefCoor_i pos;
	pos.x = report.x;
	pos.y = report.y;
	pos.z = report.z;
	Swarm_state->set_quad_ecef(report.ac_id,pos);
	Swarm_state->set_quad_pacc(report.ac_id,report.pacc);
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::update_ned_coor_by_ecef_coor()
{
	uint8_t count_ac = 1;
	for(count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		struct EcefCoor_i pos = Swarm_state->get_quad_coor(count_ac);
		ned_of_ecef_pos_i(&ned_pos[count_ac],&ref,&(pos));
	}
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void Ground_Station::update_ned_coor_by_ecef_coor(uint8_t AC_ID)
{
	struct EcefCoor_i pos = Swarm_state->get_quad_coor(AC_ID);
	ned_of_ecef_pos_i(&ned_pos[AC_ID],&ref,&(pos));
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
void * Ground_Station::periodic_data_handle(void * arg)
{
	while(1)
	{
		Com->XBEE_read_into_recv_buff();
		Com->XBEE_parse_XBEE_msg();
		//then update state
		while(!Com->msg.empty())
		{
			XBEE_msg *ptr = Com->msg.front();
			Com->msg.pop();
			pprz_msg data = ptr->get_pprz_msg();
			uint8_t msg_id = data.pprz_get_msg_id();
			if(msg_id == 155)
			{
				printf("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				data.show_hex();
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
			}
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
				pthread_mutex_lock(&quad_status_readable);
				printf("Data_handler: Locked for updating data\n");
				update_on_quad_swarm_report(report);
				Swarm_state->set_quad_state(report.ac_id,report.state);
				Swarm_state->set_quad_pacc(report.ac_id,report.pacc);
				update_ned_coor_by_ecef_coor(report.ac_id);
				pthread_mutex_unlock(&quad_status_readable);
				//this is the unproper method
				//update_GUI_quad_status(report);

				//this is what should be used if want to change the content from another thread

				g_main_context_invoke(NULL,update_GUI_quad_status_pthread,(gpointer) &report);
				//gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_state),"haha");
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
						Swarm_state->get_pacc(ac1),\
						Swarm_state->get_pacc(ac2));
				#endif
			}
		}
	}
	return NULL;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
gboolean Ground_Station::update_GUI_quad_status_pthread(gpointer userdata)
{
	struct quad_swarm_report report = *(struct quad_swarm_report *)userdata;
	
	char *buffer = (char *) malloc(sizeof(char) * 64);
	memset(buffer,0,sizeof(char)*64);

	sprintf(buffer,"ned x: %f",POS_FLOAT_OF_BFP(ned_pos[report.ac_id].x));
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_ned_x),buffer);
	sprintf(buffer,"ned y: %f",POS_FLOAT_OF_BFP(ned_pos[report.ac_id].y));
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_ned_y),buffer);
	sprintf(buffer,"ned z: %f",POS_FLOAT_OF_BFP(ned_pos[report.ac_id].z));
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_ned_z),buffer);

	sprintf(buffer,"ecef x: %d",report.x);
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_ecef_x),buffer);
	sprintf(buffer,"ecef y: %d",report.y);
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_ecef_y),buffer);
	sprintf(buffer,"ecef z: %d",report.z);
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_ecef_z),buffer);
	
	sprintf(buffer,"pacc : %d",report.pacc);
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_pacc),buffer);
	
	sprintf(buffer,"state: %s",state_string[report.state]);
	gtk_label_set_text(GTK_LABEL(GCS_GUI->quad_status_frame[report.ac_id].label_state),buffer);

	free(buffer);
	return G_SOURCE_REMOVE;
}
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//

