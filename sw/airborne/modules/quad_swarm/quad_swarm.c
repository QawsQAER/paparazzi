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

uint8_t self_state;
void quad_swarm_init( void ) 
{
	self_state = SWARM_INIT;
	memset(&target,0,sizeof(struct EcefCoor_i));	
}
void quad_swarm_periodic( void )
{
	switch(self_state)
	{
		case(SWARM_INIT)
		{
			//wait for GCS to start the module
			//if received quad_swarm_msg with (0,0,0)
			//proceed to the next state
			break;
		}
		case(SWARM_NEOGOTIATE_REF)
		{
			//module started
			//sending its gps position to the GCS
			//wait for GCS to ack this quad that GCS 
			//has choose a gps position as the reference
			break;
		}
		case(SWARM_WAIT_CMD)
		{
			//wait for command
			//if receive quad_swarm_msg 
			//record the target position
			//and proceed to next state
			break;
		}
		case(SWARM_SEND_ACK)
		{
			//wait for ack
			//if receive ack
			//proceed to next state
			break;
		}
		case(SWARM_EXEC_CMD)
		{
			//change the target position of the navigation system
			//check the the execution status
			//if reach the target
			//proceed to the next state
			break;
		}
		case(SWARM_REPORT_STATE)
		{
			//report that this quad has finished the task
			//wait for ack
			//if ack received
			//go back to SWARM_WAIT_CMD
			break;
		}
	}	
}
void quad_swarm_event( void ) {}
void quad_swarm_datalink( void )
{
	//handle the incoming message
	//find out whether it's the ack or real target command
	//and alter the state accordingly
}


