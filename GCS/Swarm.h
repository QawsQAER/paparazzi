#ifndef _SWARM_H
#define _SWARM_H

#include "main.h"
#include "address.h"
#include "geo.h"


#define QUAD_INIT 0
#define QUAD_RECV 1
#define QUAD_EXEC 2

#define QUAD_NB 2

enum QuadState {SWARM_INIT,\
		SWARM_NEGOTIATE_REF,\
		SWARM_WAIT_CMD,\
		SWARM_WAIT_CMD_START_ENGINE,\
		SWARM_WAIT_CMD_TAKEOFF,\
		SWARM_SEND_ACK,\
		SWARM_WAIT_EXEC_ACK,\
		SWARM_EXEC_CMD,\
		SWARM_REPORT_STATE,\
		SWARM_KILLED};
class Swarm
{
	private:
		//the state machine of quadcopters
		QuadState state[QUAD_NB + 1];
		//the XBEE address of quadcopters
		uint32_t XBEE_ADDR_HI[QUAD_NB + 1];
		uint32_t XBEE_ADDR_LO[QUAD_NB + 1];
		//the ECEF coordinate system position
		struct EcefCoor_i pos[QUAD_NB + 1];
		//the accruacy of positioning
		int32_t pacc[QUAD_NB + 1];
		struct EcefCoor_i pos_mean_error;
	public:
		Swarm();
		~Swarm();
		uint32_t get_address_HI(uint8_t &AC_ID);
		uint32_t get_address_LO(uint8_t &AC_ID);
		QuadState get_state(uint8_t &AC_ID);
		void set_quad_state(uint8_t &AC_ID, QuadState s);
		void set_quad_ecef(uint8_t &AC_ID,struct EcefCoor_i &pos);
		void set_quad_pacc(uint8_t &AC_ID,int32_t paac);
		
		void set_pos_mean_error();
		bool all_in_state(QuadState s);
};
#endif

