#include <Serval_Lwm2m.h>
#include "AppLwm2mObjects.h"

#include <stdio.h>
#include <string.h>

#include "SiloIO.h"


static retcode_t get_state(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	char status[12]; //change to something better

	switch (silo_state) {
	case 0:
		strcpy(status, "Empty");
		break;
	case 1:
		strcpy(status, "Filling");
		break;
	case 2:
		strcpy(status, "Full");
		break;
	case 3:
		strcpy(status, "Emptying");
		break;
	case 4:
		strcpy(status, "Stopped");
		break;
	case 5:
		strcpy(status, "Initializing");
		break;
	}

	//char* status_name[] = {"Empty", "Filling", "Full", "Emptying"};
	//char status[12] = status_name[silo_state];

	StringDescr_T strDescr_tz;
	StringDescr_set(&strDescr_tz, (char*) &status, strlen(status));

//	Lwm2m_URI_Path_T currentTimeUriPath = {OBJECT_INDEX_VALUE,OBJECT_INSTANCE_INDEX_VALUE,OBJECT_RESOURCE_NUMBER};
//	Lwm2mReporting_resourceChanged(&currentTimeUriPath);

	return (Lwm2mSerializer_serializeString(serializer_ptr, &strDescr_tz));
}


static retcode_t get_filling_completed(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	return (Lwm2mSerializer_serializeBool(serializer_ptr, filling_completed));
}

static retcode_t get_emptying_completed(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	return (Lwm2mSerializer_serializeBool(serializer_ptr, emptying_completed));
}

#if SILO_MODE ==4

static retcode_t get_heating_completed(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	return (Lwm2mSerializer_serializeBool(serializer_ptr, heating_completed));
}

static retcode_t get_mixing_completed(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	return (Lwm2mSerializer_serializeBool(serializer_ptr, mixing_completed));
}

#endif

struct Silo silo1 = {
		{ 0, LWM2M_DYNAMIC( get_state ) },
		{ 1, LWM2M_FUNCTION( exec_fill ) },
		{ 2, LWM2M_FUNCTION( exec_empty ) },
		{ 3, LWM2M_FUNCTION( exec_stop ) },
		{ 4, LWM2M_FUNCTION( exec_initialize ) },
		{ 5, LWM2M_FUNCTION( exec_heat ) },
		{ 6, LWM2M_FUNCTION( exec_mix ) },
		{ 7, LWM2M_DYNAMIC( get_filling_completed ) },
		{ 8, LWM2M_DYNAMIC( get_emptying_completed ) },
		{ 9, LWM2M_DYNAMIC( get_heating_completed ) },
		{ 10, LWM2M_DYNAMIC( get_mixing_completed ) },
		{ 11, LWM2M_FLOAT( 32 ) | LWM2M_WRITE_ALLOWED },
		};
