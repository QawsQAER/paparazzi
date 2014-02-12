/*

        This source code is written by Edward HUANG, and Tony Yi
        For the development of their Final Year Project
        Department of Computer Science and Engineering
        The Chinese University of HONG KONG

*/

/* 
        This source code referenced to the implemenetation of some geo functions
        in the paparazzi open source project
        http:\\paparazzi.enac.fr
*/

#ifndef _GEO_H
#define _GEO_H
#include "main.h"
#include "GCS_math.h"

//-----------------------------------------------//
//-Struct definition for geographical function---//
//-----------------------------------------------//
struct EcefCoor_i
{
	int32_t x;
	int32_t y;
	int32_t z;
};
////////////////////
struct LlaCoor_i
{
	//in radians *1e7
	int32_t lon;
	int32_t lat;
	int32_t alt;
};
///////////////////
struct NedCoor_i
{
	//north
	int32_t x;
	//east
	int32_t y;
	//down
	int32_t z;
};
//////////////////
struct EnuCoor_i
{
	int32_t x;
	int32_t y;
	int32_t z;
};
//////////////////
struct LtpDef_i
{
	//reference point in ecef coordinate system
	struct EcefCoor_i ecef;
	//reference point in lla coordinate system
	struct LlaCoor_i lla;
	//Rotational matrix of ecef w.r.t local tangent plane
	struct Int32Mat33 ltp_of_ecef;
	//height above mean sea level
	int32_t hmsl;
};

//-----------------------------------------------------//
//-Function prototypes for geographical functionality--//
//-----------------------------------------------------//


//*****************************************************//
//*This function takes 'ecef' as the input, and compute//
//*the 'ned' as the output, w.r.t 'def', the output ned//
//*is in unit centimeter                              *//
//*****************************************************//

/* In ins_int.c it is implemented as 
void ins_update_gps(void) {
#if USE_GPS
	if (gps.fix == GPS_FIX_3D)
	{
		if (!ins_ltp_initialised)
		{	//initialised the INS local tangent plane coordinate system	
			ltp_def_from_ecef_i(&ins_ltp_def, &gps.ecef_pos);
			ins_ltp_def.lla.alt = gps.lla_pos.alt;
			ins_ltp_def.hmsl = gps.hmsl;
			ins_ltp_initialised = TRUE;
			stateSetLocalOrigin_i(&ins_ltp_def);
		}
		//the output of ned_of_ecef_point_i is in NedCoor_i, and it's in unit of centimeter
		ned_of_ecef_point_i(&ins_gps_pos_cm_ned, &ins_ltp_def, &gps.ecef_pos);
		ned_of_ecef_vect_i(&ins_gps_speed_cm_s_ned, &ins_ltp_def, &gps.ecef_vel);
		#if USE_HFF // if horizontal filter is used 
		VECT2_ASSIGN(ins_gps_pos_m_ned, ins_gps_pos_cm_ned.x, ins_gps_pos_cm_ned.y);
		VECT2_SDIV(ins_gps_pos_m_ned, ins_gps_pos_m_ned, 100.);
		VECT2_ASSIGN(ins_gps_speed_m_s_ned, ins_gps_speed_cm_s_ned.x, ins_gps_speed_cm_s_ned.y);
		VECT2_SDIV(ins_gps_speed_m_s_ned, ins_gps_speed_m_s_ned, 100.);
		if(ins.hf_realign)
		{
			ins.hf_realign = FALSE;
			#ifdef SITL
			struct FloatVect2 true_pos, true_speed;
			VECT2_COPY(true_pos, fdm.ltpprz_pos);
			VECT2_COPY(true_speed, fdm.ltpprz_ecef_vel);
			b2_hff_realign(true_pos, true_speed);
			#else
			const struct FloatVect2 zero = {0.0, 0.0};
			b2_hff_realign(ins_gps_pos_m_ned, zero);
			#endif
		}
		b2_hff_update_gps();

		#if !USE_VFF // vff not used 
		ins_ltp_pos.z = (ins_gps_pos_cm_ned.z * INT32_POS_OF_CM_NUM) / INT32_POS_OF_CM_DEN;
		ins_ltp_speed.z = (ins_gps_speed_cm_s_ned.z * INT32_SPEED_OF_CM_S_NUM) INT32_SPEED_OF_CM_S_DEN;
		#endif // vff not used 
		#endif // hff used  
		#if !USE_HFF // hff not used //
		#if !USE_VFF // neither hf nor vf used 
		INT32_VECT3_SCALE_3(ins_ltp_pos, ins_gps_pos_cm_ned, INT32_POS_OF_CM_NUM, INT32_POS_OF_CM_DEN);
		INT32_VECT3_SCALE_3(ins_ltp_speed, ins_gps_speed_cm_s_ned, INT32_SPEED_OF_CM_S_NUM, INT32_SPEED_OF_CM_S_DEN);
		#else // only vff used 
		INT32_VECT2_SCALE_2(ins_ltp_pos, ins_gps_pos_cm_ned, INT32_POS_OF_CM_NUM, INT32_POS_OF_CM_DEN);
		INT32_VECT2_SCALE_2(ins_ltp_speed, ins_gps_speed_cm_s_ned, INT32_SPEED_OF_CM_S_NUM, INT32_SPEED_OF_CM_S_DEN);
		#endif
		#if USE_GPS_LAG_HACK
		VECT2_COPY(d_pos, ins_ltp_speed);
		INT32_VECT2_RSHIFT(d_pos, d_pos, 11);
		VECT2_ADD(ins_ltp_pos, d_pos);
		#endif
		#endif // hff not used 
		INS_NED_TO_STATE();
	}
#endif // USE_GPS 
}

*/
extern void ned_of_ecef_point_i(struct NedCoor_i * ned, struct LtpDef_i *def, struct EcefCoor_i * ecef);


//*****************************************************//
//*This function is called by the above function      *//
//*'ned_of_ecef_point_i'                              *//
//*****************************************************//
extern void enu_of_ecef_point_i(struct EnuCoor_i *enu, struct LtpDef_i *def, struct EcefCoor_i *ecef);


//-----------------------------------------------------//
//--MACRO for implementation of geopraphical functions-//
//-----------------------------------------------------//

#define ENU_OF_TO_NED(_po, _pi)\
{\
	(_po).x = (_pi).y;\
	(_po).y = (_pi).x;\
	(_po).z = -(_pi).z;\
}

#define HIGH_RES_TRIG_FRAC 20




#endif
