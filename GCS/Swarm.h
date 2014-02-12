#ifndef _SWARM_H
#define _SWARM_H

#include "main.h"
#include "address.h"
#include "geo.h"


#define QUAD_INIT 0
#define QUAD_RECV 1
#define QUAD_EXEC 2

#define QUAD_NB 1

enum QuadState {IDLE, INIT, READY, EXEC, WAIT};
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

