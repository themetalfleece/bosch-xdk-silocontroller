#include <Serval_Lwm2m.h>
#include "AppLwm2mObjects.h"

#include <stdio.h>
#include <string.h>

#include "SiloIO.h"


static retcode_t get_owner(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	StringDescr_T strDescr_tz;
	StringDescr_set(&strDescr_tz, (char*) &owners[front], strlen(owners[front]));

	return (Lwm2mSerializer_serializeString(serializer_ptr, &strDescr_tz));
}


//static retcode_t get_filling_completed(Lwm2mSerializer_T *serializer_ptr,
//		Lwm2mParser_T *parser_ptr) {
//	(void) parser_ptr;
//	(void) serializer_ptr;
//
//	return (Lwm2mSerializer_serializeBool(serializer_ptr, filling_completed));
//}


struct Pipe pipe1 = {
		{ 0, LWM2M_DYNAMIC( get_owner ) },
		{ 1, LWM2M_FUNCTION( exec_acquire ) },
		{ 2, LWM2M_FUNCTION( exec_empty ) },
};
