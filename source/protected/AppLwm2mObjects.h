/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */
#ifndef XDK110_LWM2MOBJECTS_H_
#define XDK110_LWM2MOBJECTS_H_

#include <Serval_Lwm2m.h>

/* public type and macro definitions */
#define LWM2MOBJECTS_UDP_LENGTH                 UINT32_C(1)
#define LWM2MOBJECTS_UDP_QUEUED_LENGTH          UINT32_C(2)
#define LWM2MOBJECTS_SMS_LENGTH                 UINT32_C(1)
#define LWM2MOBJECTS_SMS_QUEUED_LENGTH          UINT32_C(2)
#define LWM2MOBJECTS_UDP_AND_SMS_LENGTH         UINT32_C(2)
#define LWM2MOBJECTS_UDP_QUEUED_AND_SMS_LENGTH  UINT32_C(3)
#define STRING_RESOURCES_MAX_LENGTH             UINT32_C(50)
/* Object ID */
#define LWM2M_OBJECTID_DEVICE 3

struct DeviceResource_S
{
    Lwm2mResource_T manufacturer;
    Lwm2mResource_T modelNumber;
    Lwm2mResource_T serialNumber;
    Lwm2mResource_T FirmwareVersion;
    Lwm2mResource_T deviceReboot;
    Lwm2mResource_T factoryReset;
    Lwm2mResource_T availablePowerSources;
    Lwm2mResource_T powerSourceVoltage;
    Lwm2mResource_T powerSourceCurrent;
    Lwm2mResource_T batteryLevel;
    Lwm2mResource_T memoryFree;
    Lwm2mResource_T errorCode;
    Lwm2mResource_T resetErrorCode;
    Lwm2mResource_T currentTime;
    Lwm2mResource_T UTCOffset;
    Lwm2mResource_T Timezone;
    Lwm2mResource_T SupportedBindingAndModes;
};

struct Silo {

	Lwm2mResource_T status;
	Lwm2mResource_T fill;
	Lwm2mResource_T empty;
	Lwm2mResource_T stop;
	Lwm2mResource_T initialize;
//#if SILO_MODE == 4
	Lwm2mResource_T heat;
	Lwm2mResource_T mix;
	Lwm2mResource_T fillingCompleted;
	Lwm2mResource_T emptyingCompleted;
	Lwm2mResource_T heatingCompleted;
	Lwm2mResource_T mixingCompleted;
	Lwm2mResource_T targetTemperature;
//#endif
};

extern struct DeviceResource_S deviceResources;
extern struct Silo silo1;

#endif /* XDK110_LWM2MOBJECTS_H_ */
