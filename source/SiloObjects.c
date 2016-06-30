#include <Serval_Lwm2m.h>
#include "AppLwm2mObjects.h"

#include <stdio.h>
#include <string.h>

#include "SiloIO.h"


struct Silo {

	Lwm2mResource_T status;
	Lwm2mResource_T fillingcompleted;
	Lwm2mResource_T emptyingcompleted;
	Lwm2mResource_T mode;
	Lwm2mResource_T fill;
	Lwm2mResource_T empty;
	Lwm2mResource_T stopfilling;
	Lwm2mResource_T stopemptying;
	Lwm2mResource_T initialize;
#if SILO_MODE == 4
	Lwm2mResource_T heat;
	Lwm2mResource_T mix;
#endif
};

static retcode_t get_state(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	(void) parser_ptr;
	(void) serializer_ptr;


	char status[12]; //change to something better

	switch (silo_state){
		case 0: strcpy(status,"Empty");
			break;
		case 1: strcpy(status,"Filling");
			break;
		case 2: strcpy(status,"Full");
			break;
		case 3: strcpy(status,"Emptying");
			break;
		case 4: strcpy(status,"Stopped");
			break;
	}

	//char* status_name[] = {"Empty", "Filling", "Full", "Emptying"};
	//char status[12] = status_name[silo_state];

	StringDescr_T strDescr_tz;
		StringDescr_set(&strDescr_tz, (char*) &status, strlen(status));

	return (Lwm2mSerializer_serializeString(serializer_ptr, &strDescr_tz));
    return (RC_OK);
}

#if SILO_MODE ==4
static retcode_t get_mixing_status(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	(void) parser_ptr;
	(void) serializer_ptr;


	char status[12]; //change to something better

	switch (silo_state){
		case 0: strcpy(status,"Empty");
			break;
		case 1: strcpy(status,"Filling");
			break;
		case 2: strcpy(status,"Full");
			break;
		case 3: strcpy(status,"Emptying");
			break;
		case 4: strcpy(status,"Stopped");
			break;
	}

	//char* status_name[] = {"Empty", "Filling", "Full", "Emptying"};
	//char status[12] = status_name[silo_state];

	StringDescr_T strDescr_tz;
		StringDescr_set(&strDescr_tz, (char*) &status, strlen(status));

	return (Lwm2mSerializer_serializeString(serializer_ptr, &strDescr_tz));
    return (RC_OK);
}
#endif

#warning match ids
struct Silo silo1  =
{
	{ 0,  LWM2M_DYNAMIC( get_state ) },
	{ 1,  LWM2M_FUNCTION( exec_fill ) },
	{ 2,  LWM2M_FUNCTION( exec_empty ) },
	{ 3,  LWM2M_FUNCTION( exec_stop ) },
	{ 4,  LWM2M_FUNCTION( exec_initialize ) },

#if SILO_MODE == 4
	{ 5,  LWM2M_FUNCTION( exec_heat ) },
	{ 6,  LWM2M_FUNCTION( exec_mix ) },
	{ 10,  LWM2M_FUNCTION( get_mixing_status ) },
#endif

};
