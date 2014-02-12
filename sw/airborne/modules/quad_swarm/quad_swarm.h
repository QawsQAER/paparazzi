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

#define SWARM_INIT 0
#define SWARM_NEGOTIATE_REF 1
#define SWARM_WAIT_CMD 2
#define SWARM_SEND_ACK 3
#define SWARM_WAIT_ACK 4
#define SWARM_EXEC_CMD 5
#define SWARM_REPORT_STATE 6

struct EcefCoor_i target;
extern uint8_t self_state;
extern void quad_swarm_init( void );
extern void quad_swarm_periodic( void );
extern void quad_swarm_event( void );
extern void quad_swarm_datalink( void );

#endif

