#ifndef LEG_H_
#define LEG_H_

typedef struct
{
	/*Leg offset fom center*/
	uint16_t ox, oy, oz;
	/*Current leg coordinates*/
	float x, y, z;
	/*Current leg angles*/
	float s1, s2, s3;
	/*Leg status bits*/
	uint8_t status;
} leg_t;

#define LEG_ON_GROUND (1 << 0)
#define LEG_ENABLED (1 << 1)
#define LEG_MIRROR (1 << 2)


void leg_calc_ik(leg_t* l);

void leg_update(leg_t* l);

#endif






