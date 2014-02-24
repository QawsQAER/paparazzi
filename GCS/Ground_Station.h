/*

        This source code is written by Edward HUANG, and Tony Yi
        For the development of their Final Year Project
        Department of Computer Science and Engineering
        The Chinese University of HONG KONG

*/

#ifndef _GROUND_STATION_H
#define _GROUND_STATION_H
#include "main.h"
#include "XBEE.h"
#include "pprz_msg.h"
#include "Swarm.h"
#include "Serial.h"
#include "geo.h"

enum GroundControlStation_state {GCS_INIT,\
		GCS_NEGOTIATE_REF,\
		GCS_WAIT_TO_START_ENGINE,\
		GCS_START_ENGINE,\
		GCS_WAIT_TO_TAKEOFF,\
		GCS_TAKEOFF,\
		GCS_WAIT_TO_SEND_CMD,\
		GCS_SEND_CMD,\
		GCS_WAIT_ACK,\
		GCS_SEND_EXEC_ACK,\
		GCS_WAIT_REPORT};

class Ground_Station
{
	private:
		Swarm *Swarm_state;
		XBEE *Com;
		GroundControlStation_state GCS_state;
		EcefCoor_i ref_ecef;
		NedCoor_i ref_ned;
		void Send_Msg_Block(uint8_t &AC_ID, uint8_t &BLOCK_ID);
	public:
		//Ground_Station();
		//This function will initilize the serial port for communication and the xbee module etc.
		Ground_Station(char * port_name);
		~Ground_Station();
		
		
		//after this function is called, 
		//all quadcopters should be in SWARM_NEGOTIATE_REF
		//the navigation subsystem of quadcopter would be in block 2
		void init_quadcopters();
	
		void negotiate_ref();		
		//This function will ask quadcopters to takeoff
		void nav_start_engine(uint8_t AC_ID);
		void nav_start_engine();
		
		void nav_takeoff(uint8_t AC_ID);
		void nav_takeoff();
		
		void takeoff_quadcopters();
		//This function will ask the quadcopter to takeoff
		void takeoff_quadcopter(uint8_t AC_ID);
		
		void send_ack(uint8_t AC_ID, uint8_t ack);	
		void ap_kill_quadcopter(uint8_t AC_ID);
		void ap_nav_quadcopter(uint8_t AC_ID);
};

#endif
