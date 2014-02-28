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

#ifndef QUAD_SWARM_H
#define QUAD_SWARM_H
#include "state.h"

//for autopilot_set_mode
#include "autopilot.h"

//for flight plan, nav_block, nav_stage etc
#include "subsystems/navigation/common_flight_plan.h"

#define SWARM_INIT 0
#define SWARM_NEGOTIATE_REF 1
#define SWARM_WAIT_CMD 2
#define SWARM_WAIT_CMD_START_ENGINE 3
#define SWARM_WAIT_CMD_TAKEOFF 4
#define SWARM_SEND_ACK 5
#define SWARM_WAIT_EXEC_ACK 6
#define SWARM_EXEC_CMD 7
#define SWARM_REPORT_STATE 8
#define SWARM_KILLED 9

#define SWARM_REACH_CRITERION 9
extern struct EcefCoor_i quad_swarm_target;

extern void quad_swarm_init( void );
extern void quad_swarm_periodic( void );
extern void quad_swarm_event( void );
extern uint8_t quad_swarm_reach_tar( void );
extern void quad_swarm_target_to_waypoint( void );

extern uint8_t quad_swarm_state;
extern uint8_t quad_swarm_id;
extern uint8_t quad_swarm_recv_ack;

//called when DL_quad_swarm_msg is detected

#define quad_swarm_datalink(){\
	uint8_t ac_id = DL_quad_swarm_msg_ac_id(dl_buffer);\
	if(quad_swarm_id == ac_id && (quad_swarm_state == SWARM_WAIT_CMD_TAKEOFF || quad_swarm_state == SWARM_REPORT_STATE))\
	{\
		uint8_t dummy = DL_quad_swarm_msg_dummy(dl_buffer);\
		if(ac_id == AC_ID)\
		{\
		quad_swarm_target.x = DL_quad_swarm_msg_tar_ecef_pos_x(dl_buffer);\
		quad_swarm_target.y = DL_quad_swarm_msg_tar_ecef_pos_y(dl_buffer);\
		quad_swarm_target.z = DL_quad_swarm_msg_tar_ecef_pos_z(dl_buffer);\
		}\
		quad_swarm_state = SWARM_SEND_ACK;\
	}\
}

#define quad_swarm_ack_datalink(){\
	uint8_t ac_id = DL_quad_swarm_ack_forwarded_ac_id(dl_buffer);\
	uint8_t ack = DL_quad_swarm_ack_forwarded_ack(dl_buffer);\
	if(ack == 0xff)\
	{\
		autopilot_set_mode(AP_MODE_KILL);\
		quad_swarm_state = SWARM_KILLED;\
	}\
	else if(ack == 0xfe)\
	{\
		autopilot_set_mode(AP_MODE_NAV);\
		quad_swarm_state = SWARM_INIT;\
		nav_init();\
	}\
	else if(ack == 2 && quad_swarm_state == SWARM_NEGOTIATE_REF)\
	{\
		quad_swarm_state = SWARM_WAIT_CMD;\
	}\
	else if(ack == 3 && quad_swarm_state == SWARM_WAIT_EXEC_ACK)\
	{\
		quad_swarm_state = SWARM_EXEC_CMD;\
		quad_swarm_target_to_waypoint();\
	}\
}
extern void quad_swarm_start( void );
extern void quad_swarm_stop( void );
#endif

