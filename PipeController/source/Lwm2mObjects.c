/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */
#include "AppLwm2mObjects.h"
#include "AppLwm2mInterface.h"

#include <Serval_Clock.h>
#include <Serval_Exceptions.h>

/* global variables ********************************************************* */

extern Lwm2mDevice_T deviceResourceInfo;

/* local variables ********************************************************* */
static int32_t offset = UINT32_C(0);
static char tz[20] = "Europe/Berlin";

/* local functions ********************************************************** */

/* @brief
 * This function is used to update the deviceReboot Resource value into the LWM2M Server*
 */
static retcode_t deviceRebootFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("deviceReboot\r\n");
    return (RC_OK);
}
/* @brief
 * This function is used to update the factoryReset Resource value into the LWM2M Server*
 */
static retcode_t factoryResetFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("factoryReset\r\n");
	return (RC_OK);
}
/* @brief
 * This function is used to update the batteryLevel Resource value into the LWM2M Server*
 */
static retcode_t batteryLevelFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	if ( parser_ptr != NULL ) {
		return (RC_LWM2M_METHOD_NOT_ALLOWED);
	}
	printf("batteryLevel\r\n");
	
	return (Lwm2mSerializer_serializeInt(serializer_ptr, 100));
}
/* @brief
 * This function is used to update the memoryFree Resource value into the LWM2M Server*
 */
static retcode_t memoryFreeFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	if ( parser_ptr != NULL ) {
		return (RC_LWM2M_METHOD_NOT_ALLOWED);
	}

	printf("memoryFree\r\n");
	
	return (Lwm2mSerializer_serializeInt(serializer_ptr, 50));
}
/* @brief
 * This function is used to update the errorCode Resource value into the LWM2M Server*
 */
static retcode_t errorCodeFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	if ( parser_ptr != NULL ) {
		return (RC_LWM2M_METHOD_NOT_ALLOWED);
	}

	printf("errorCode\r\n");
	
	return (Lwm2mSerializer_serializeInt(serializer_ptr, 0));
}
/* @brief
 * This function is used to update the resetErrorCode Resource value into the LWM2M Server*
 */
static retcode_t resetErrorCodeFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("resetErrorCode\r\n");
	
	return (RC_OK);
}
/* @brief
 * This function is used to update the currentTime Resource value into the LWM2M Server*
 */
static retcode_t getCurrentTime(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	retcode_t rc = RC_OK;

	uint32_t time;
	Clock_getTime(&time);

	printf("currentTime\r\n");

	if ( parser_ptr != NULL ) {
	    /* Update current time */
		int32_t newTime;
		char newTimeString[STRING_RESOURCES_MAX_LENGTH];

		StringDescr_T strDescr;
		StringDescr_set(&strDescr, newTimeString, STRING_RESOURCES_MAX_LENGTH);
		rc = Lwm2mParser_getString(parser_ptr, &strDescr);

		if ( rc != RC_OK ) {
			return (rc);
		}

		rc = Lwm2mParser_convertStringToInt(newTimeString, strlen(newTimeString), &newTime);
		if ( rc != RC_OK ) {
			return (rc);
		}

		offset = newTime - time;

		Lwm2m_URI_Path_T currentTimeUriPath = {OBJECT_INDEX_VALUE,OBJECT_INSTANCE_INDEX_VALUE,OBJECT_RESOURCE_NUMBER};
		Lwm2mReporting_resourceChanged(&currentTimeUriPath);

		return (RC_OK);
	}

	return (Lwm2mSerializer_serializeTime(serializer_ptr, time + offset));
}
/* @brief
 * This function is used to update the tzone Resource value into the LWM2M Server*
 */
static retcode_t getTimeZone(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	StringDescr_T strDescr_tz;
	StringDescr_set(&strDescr_tz, (char*) &tz, strlen(tz));

	printf("tzone\r\n");
	
	if (parser_ptr == NULL) {
		return (Lwm2mSerializer_serializeString(serializer_ptr, &strDescr_tz));
	} else {
		retcode_t rc;
		char newTz[STRING_RESOURCES_MAX_LENGTH];

		StringDescr_T strDescr;
		StringDescr_set(&strDescr, newTz, STRING_RESOURCES_MAX_LENGTH);
		rc = Lwm2mParser_getString(parser_ptr, &strDescr);
		if ( rc != RC_OK ) {
			return (rc);
		}
		if ( strlen(newTz) > 19 ) {
			return (RC_LWM2M_INTERNAL_ERROR);
		}
		strncpy(tz, newTz, strlen(newTz));
		tz[strlen(newTz)] = '\0';
		return (RC_OK);
	}
}
/* @brief
 * This function is used to update the getBinding Resource value into the LWM2M Server*
 */
static retcode_t getBindingFunc(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr)
{
	if ( parser_ptr != NULL ) {
		return (RC_LWM2M_METHOD_NOT_ALLOWED);
	}

	char* binding;
	uint8_t length;
	
	printf("getBinding\r\n");
	
	switch (deviceResourceInfo.binding)
	{
	case UDP:
		binding = "U";
		length = LWM2MOBJECTS_UDP_LENGTH;
		break;

	case UDP_QUEUED:
		binding = "UQ";
		length = LWM2MOBJECTS_UDP_QUEUED_LENGTH;
		break;

	case SMS:
		binding = "S";
		length = LWM2MOBJECTS_SMS_LENGTH;
		break;

	case SMS_QUEUED:
		binding = "SQ";
		length = LWM2MOBJECTS_SMS_QUEUED_LENGTH;
		break;

	case UDP_AND_SMS:
		binding = "US";
		length = LWM2MOBJECTS_UDP_AND_SMS_LENGTH;
		break;

	case UDP_QUEUED_AND_SMS:
		binding = "UQS";
		length = LWM2MOBJECTS_UDP_QUEUED_AND_SMS_LENGTH;
		break;
	}

	StringDescr_T strDescr;

	StringDescr_set(&strDescr, binding, length);

	return (Lwm2mSerializer_serializeString(serializer_ptr, &strDescr));
}

/* TODO need a way to define inactive resources. */
struct DeviceResource_S deviceResources =
{
        { 0,  LWM2M_STRING_RO( "Bosch Connected Devices and Solutions GmbH" ) },
        { 1,  LWM2M_STRING_RO( "XDK110" ) },
        { 2,  LWM2M_STRING_RO( "1" ) },
        { 3,  LWM2M_STRING_RO( "1" ) },
        { 4,  LWM2M_FUNCTION( deviceRebootFunc ) },
        { 5,  LWM2M_FUNCTION( factoryResetFunc ) },
        { 6,  LWM2M_INTEGER( 0 ) },
        { 7,  LWM2M_INTEGER( 0 ) },
        { 8,  LWM2M_INTEGER( 0 ) },
        { 9,  LWM2M_DYNAMIC( batteryLevelFunc ) },
        { 10, LWM2M_DYNAMIC( memoryFreeFunc ) },
        { 11, LWM2M_DYNAMIC( errorCodeFunc ) },
        { 12, LWM2M_FUNCTION( resetErrorCodeFunc ) },
        { 13, LWM2M_DYNAMIC( getCurrentTime ) },//LWM2M_TIME( -6000 ) },
        { 14, LWM2M_STRING_RO( "UTC+2" ) },
        { 15, LWM2M_DYNAMIC( getTimeZone ) },
        { 16, LWM2M_DYNAMIC( getBindingFunc ) },
};
