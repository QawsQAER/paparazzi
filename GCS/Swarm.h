#ifndef _SWARM_H
#define _SWARM_H

#include "main.h"
#include "address.h"
#include "geo.h"


#define QUAD_INIT 0
#define QUAD_RECV 1
#define QUAD_EXEC 2

#define QUAD_NB 1

enum QuadState {SWARM_INIT, SWARM_NEGOTIATE_REF, SWARM_WAIT_CMD, \
		SWARM_SEND_ACK, SWARM_WAIT_EXEC_ACK, SWARM_EXEC_CMD,\
		SWARM_REPORT_STATE};
class Swarm
{
	private:
		QuadState state[QUAD_NB + 1];	
		uint32_t XBEE_ADDR_HI[QUAD_NB + 1];
		uint32_t XBEE_ADDR_LO[QUAD_NB + 1];	
		struct EcefCoor_i pos[QUAD_NB + 1];
	public:
		Swarm();
		~Swarm();
		uint32_t get_address_HI(uint8_t &AC_ID);
		uint32_t get_address_LO(uint8_t &AC_ID);
};
#endif

