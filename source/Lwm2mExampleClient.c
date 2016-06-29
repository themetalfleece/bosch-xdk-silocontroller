/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */

/* module includes ********************************************************** */

/* system header files */
#include <stdio.h>
#include "BCDS_Basics.h"

/* additional interface header files */
#include <Serval_Network.h>
#include <Serval_Lwm2m.h>
#include <Serval_HttpClient.h>
#include <Serval_Basics.h>
#include <Serval_Log.h>
#include <FreeRTOS.h>
#include <task.h>

#include "PAL_initialize_ih.h"
#include "PAL_socketMonitor_ih.h"
#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#include "AppLwm2mInterface.h"
#include "BCDS_Assert.h"
/* own header files */
#include "Lwm2mExampleClient.h"
#include "SiloIO.c"

/* constant definitions ***************************************************** */
#define LOG_MODULE "CCA"

/* local variables ********************************************************** */

/* global variables ********************************************************* */
xTaskHandle initializationTask;
static xTaskHandle siloTask;

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/* The description is in the header file. */
static retcode_t wlanConnect(void)
{
    NCI_ipSettings_t myIpSettings;
    char ipAddress[PAL_IP_ADDRESS_SIZE] = { 0 };
    Ip_Address_T* IpaddressHex = Ip_getMyIpAddr();
    WLI_connectSSID_t connectSSID;
    WLI_connectPassPhrase_t connectPassPhrase;

    if (WLI_SUCCESS != WLI_init())
    {
        return (RC_PLATFORM_ERROR);
    }

    connectSSID = (WLI_connectSSID_t) WLAN_CONNECT_WPA_SSID;
    connectPassPhrase = (WLI_connectPassPhrase_t) WLAN_CONNECT_WPA_PASS;

    if (WLI_SUCCESS == WLI_connectWPA(connectSSID, connectPassPhrase, NULL))
    {
        NCI_getIpSettings(&myIpSettings);
        *IpaddressHex = Basics_htonl(myIpSettings.ipV4);
        (void) Ip_convertAddrToString(IpaddressHex, (char *) &ipAddress);
        printf("Connected to WPA network successfully\r\n ");
        printf(" Ip address of the device %s \r\n ", ipAddress);
        return (RC_OK);
    }
    else
    {
        return (RC_PLATFORM_ERROR);
    }

}

/* global functions ********************************************************** */

/** @brief This function does the initialization of required setup to interact with lwm2m Server and the objects that are defined for this project.
 *  It includes providing the necessary data for the objects like object and resource ID, object name,
 *  its instances.
 */
void init(void)
{
    char buffer[128];
    retcode_t rc = RC_OK;

    printf("INIT Starting up\r\n");

    printf("wlanConnect\r\n");
    rc = wlanConnect();
    if (RC_OK != rc)
    {
        printf("Network init/connection failed %i \r\n", rc);
        return;
    }
    printf("Network Initialization done\r\n");

    printf("PAL_initialize\r\n");
    rc = PAL_initialize();
    if (RC_OK != rc)
    {
        printf("PAL and network initialize %i \r\n", rc);
        return;
    }
    printf("PAL Initialization done\r\n");

    LOG_INFO("Logging enabled\r\n");

    printf("PAL_socketMonitorInit\r\n");
    PAL_socketMonitorInit();
    printf("PAL SocketMonitor done\n");

    /* IP address */
    Ip_Address_T * my_ip = Ip_getMyIpAddr();
    Ip_convertAddrToString(my_ip, buffer);
    printf("IP Address of the Device is %s\r\n", buffer);

    printf("Lwm2mInterface Initialization start\r\n");
    Lwm2mInterfaceInitialize();
    printf("Lwm2m Interface done\r\n");

    printf("Lwm2m Interface start with given port\r\n");
    Lwm2mInterfaceStart(Ip_convertIntToPort(CLIENT_CONNECT_PORT));
    printf("Lwm2m Interface started\r\n");

    printf("LCA Initialization Done\r\n");
}

void application(void * pvParameters)
{
    (void) pvParameters;
    init();
    /* Task Suspended once the init done */
    vTaskSuspend(initializationTask);
}

/**
 * @brief API to de-initialize the module
 */
void deinit(void)
{
    /*do nothing*/
}

/**
 * @brief This is a template function where the user can write his custom application.
 *
 */
void appInitSystem(xTimerHandle xTimer)
{
    (void) (xTimer);
    /*initialize LCA module*/
    if (xTaskCreate(application, (const char * const ) "application",TASK_STACK_SIZE,NULL, TASK_PRIORITY, &initializationTask) != pdPASS)
    {
        assert(0);
    }

    if (xTaskCreate(ioInit, "SiloControlProcess",
    			TASK_STACK_SIZE, NULL, TASK_PRIORITY, &siloTask) != pdPASS) {
    		assert(0);
    	}
}
/** ************************************************************************* */
