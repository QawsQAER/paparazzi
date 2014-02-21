/*
 * Copyright (C) Edward HUANG
 *
 * This file is part of paparazzi

 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "modules/quad_swarm/quad_swarm.h"

/**********************************************************************/
/*** for transmission                                              ****/
/***DefaultChannel and DefaultDevice are 'ifndef then define here' ****/
/*** message.h is included here                                    ****/
/**********************************************************************/
#include "subsystems/datalink/downlink.h"

/**********************************************************************/
/****     UART2CheckFreeSpace is defined here                      ****/
/**********************************************************************/
#include "mcu_periph/uart.h"

//for autopilot_mode
#include "autopilot.h"
//for flight plan, nav_block, nav_stage etc
#include "subsystems/navigation/common_flight_plan.h"
uint8_t quad_swarm_state;


//like the ac_id
uint8_t quad_swarm_id;
struct EcefCoor_i quad_swarm_target;

void quad_swarm_init( void ) 
{
	quad_swarm_state = SWARM_INIT;
	memset(&quad_swarm_target,0,sizeof(struct EcefCoor_i));	
	quad_swarm_id = AC_ID;
}

#define send_quad_swarm_report()  DOWNLINK_SEND_quad_swarm_report(\
        DefaultChannel,\
        DefaultDevice,\
        &quad_swarm_id,\
        &state.ecef_pos_i.x,\
        &state.ecef_pos_i.y,\
        &state.ecef_pos_i.z,\
        &quad_swarm_state)

void quad_swarm_periodic( void )
{
	//periodically send position info to the GCS
	
	//state.ecef_pos_i is in cm
	if(autopilot_mode != AP_MODE_NAV)
	{return ;}
	switch(quad_swarm_state)
	{
		case(SWARM_INIT):
		{
			//0
			//wait for AP_NAV  to start the module
			//if received quad_swarm_msg with (0,0,0)
			//proceed to the next state
			if(nav_block == 1)
			{
				quad_swarm_state = SWARM_NEGOTIATE_REF;
			}		
			break;
		}
		case(SWARM_NEGOTIATE_REF):
		{
			//1
			//module started
			//sending its gps position to the GCS
			//wait for GCS to ack this quad that GCS 
			//has choose a gps position as the reference
			send_quad_swarm_report();
			quad_swarm_state = SWARM_NEGOTIATE_REF;
			break;
		}
		case(SWARM_WAIT_CMD):
		{
			//2
			//wait for command
			//if receive quad_swarm_msg 
			//record the target position
			//and proceed to next state
			break;
		}
		case(SWARM_SEND_ACK):
		{
			//3
			//send ack to GCS 
			//to acknowledge the target position
			//proceed to next state
			break;
		}
		case(SWARM_WAIT_EXEC_ACK):
		{
			//4
			//wait for ack to carry out the previous cmd
			break;
		}
		case(SWARM_EXEC_CMD):
		{
			//5
                        //change the target position of the navigation system
                        //check the the execution status
                        //if reach the target
                        //proceed to the next state
			break;
		}
		case(SWARM_REPORT_STATE):
		{
			//6
			//report that this quad has finished the task
			//wait for ack
			//if ack received
			//go back to SWARM_WAIT_CMD
			break;
		}
		default:
		{
			//Encounter Error, please handle
			break;
		}
	}	
}

void quad_swarm_event( void ) {}

/*void quad_swarm_datalink( void )
{
	//handle the incoming message
	//find out whether it's the ack or real target command
	//and alter the state accordingly
}*/
void quad_swarm_start( void ){}
void quad_swarm_stop( void ){}
