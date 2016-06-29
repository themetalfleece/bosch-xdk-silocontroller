/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */

#include <Serval_Lwm2m.h>
#include "AppLwm2mObjects.h"

#include <stdio.h>

Lwm2mObjectInstance_T objectInstances[] =
{
		{ LWM2M_OBJECTID_DEVICE, LWM2M_SINGLE_INSTANCE, LWM2M_RESOURCES(deviceResources), .permissions = {LWM2M_ACCESS_CONTROL_OWNER, 0x0, 0x0, 0x0} }
};

Lwm2mDevice_T deviceResourceInfo =
{
		.name = NULL,
		.binding = UDP,
		.sms = NULL,
		.numberOfObjectInstances = LWM2M_OBJECT_INSTANCE_COUNT(objectInstances),
		.objectInstances = objectInstances,
};
