#include "geo.h"
#include "GCS_math.h"

//This function on the paparazzi broad is called by
//ins_update_gps(),
void ned_of_ecef_point_i(struct NedCoor_i *ned, struct LtpDef_i *def, struct EcefCoor_i *ecef)
{
	struct EnuCoor_i enu;
	enu_of_ecef_point_i(&enu, def, ecef);
	ENU_OF_TO_NED(*ned, enu);
}


/** Convert a ECEF position to local NED.
 * @param[out] ned  NED position in meter << #INT32_POS_FRAC
 * @param[in]  def  local coordinate system definition
 * @param[in]  ecef ECEF position in cm
 */
void ned_of_ecef_pos_i(struct NedCoor_i* ned, struct LtpDef_i* def, struct EcefCoor_i* ecef) {
  struct EnuCoor_i enu;
  enu_of_ecef_pos_i(&enu, def, ecef);
  ENU_OF_TO_NED(*ned, enu);
}

/** Convert a ECEF position to local ENU.
 * @param[out] enu  ENU position in meter << #INT32_POS_FRAC
 * @param[in]  def  local coordinate system definition
 * @param[in]  ecef ECEF position in cm
 */
void enu_of_ecef_pos_i(struct EnuCoor_i* enu, struct LtpDef_i* def, struct EcefCoor_i* ecef) {
  struct EnuCoor_i enu_cm;
  enu_of_ecef_point_i(&enu_cm, def, ecef);

  /* enu = (enu_cm / 100) << INT32_POS_FRAC
   * to loose less range:
   * enu_cm = enu << (INT32_POS_FRAC-2) / 25
   * which puts max enu output Q23.8 range to 8388km / 25 = 335km
   */
  INT32_VECT3_LSHIFT(*enu, enu_cm, INT32_POS_FRAC-2);
  VECT3_SDIV(*enu, *enu, 25);
}

/*
//TODO define INT32_VECT3_LSHIFT and VEC3_SDIV in GCS_math.h
//TODO define INT32_POS_FRAC
void enu_of_ecef_pos_i(struct EnuCoor_i * enu, struct LtpDef_i * def, struct EcefCoor_i *ecef)
{
	struct EnuCoor_i enu_cm;
	enu_of_ecef_point_i(&enu_cm,def,ecef);	
	INT32_VECT3_LSHIFT(*enu, enu_cm, INT32_POS_FRAC - 2);
	VEC3_SDIV(*enu, *enu, 25);
}
*/

//TODO define HIGH_RES_TRIG_FRAC in GCS_math.h
//TODO understand the algorithm for converting
//ECEF TO ENU
void enu_of_ecef_point_i(struct EnuCoor_i *enu, struct LtpDef_i *def, struct EcefCoor_i *ecef)
{
  struct EcefCoor_i delta;
  VECT3_DIFF(delta, *ecef, def->ecef);
  const int64_t tmpx = (int64_t)def->ltp_of_ecef.m[0]*delta.x +
                       (int64_t)def->ltp_of_ecef.m[1]*delta.y +
                       0; /* this element is always zero http://en.wikipedia.org/wiki/Geodetic_system#From_ECEF_to_ENU */
  enu->x = (int32_t)(tmpx>>HIGH_RES_TRIG_FRAC);
  const int64_t tmpy = (int64_t)def->ltp_of_ecef.m[3]*delta.x +
                       (int64_t)def->ltp_of_ecef.m[4]*delta.y +
                       (int64_t)def->ltp_of_ecef.m[5]*delta.z;
  enu->y = (int32_t)(tmpy>>HIGH_RES_TRIG_FRAC);
  const int64_t tmpz = (int64_t)def->ltp_of_ecef.m[6]*delta.x +
                       (int64_t)def->ltp_of_ecef.m[7]*delta.y +
                       (int64_t)def->ltp_of_ecef.m[8]*delta.z;
  enu->z = (int32_t)(tmpz>>HIGH_RES_TRIG_FRAC);
}


