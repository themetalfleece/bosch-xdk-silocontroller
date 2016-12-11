/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */
#include "AppLwm2mInterface.h"

#warning Enable and disable security here, by default security has disabled by defining SECURITY_ENABLE as 0.
#define SECURITY_ENABLE 0
#define SERVER_UNSECURE "coap://192.168.1.100:5683/" //leshan: coap://5.39.83.206:5683/ // sol: coap://192.168.43.8:5683/

#if SERVAL_ENABLE_COAP_SERVER

#include <Serval_Network.h>
#include <Serval_Log.h>
#include <Serval_Timer.h>
#include <Serval_Msg.h>
#include <Serval_Coap.h>
#include <Serval_CoapServer.h>
#include <Serval_CoapClient.h>
#include <Serval_Clock.h>
#include <Serval_Ip.h>
#include "led.h"
#include "XdkBoardHandle.h"
#include <Serval_Lwm2m.h>
#include "AppLwm2mObjects.h"
#include <FreeRTOS.h>
#include <timers.h>
#include "BCDS_PowerMgt.h"

#include "SiloIO.h"

#if SILO_MODE == 1
#define CLIENT_ENDPOINT "SILO_1"
#endif

#if SILO_MODE == 4
#define CLIENT_ENDPOINT "SILO_4"
#endif

#define LOG_MODULE "CCA"

/* global variables ********************************************************* */
static LED_handle_tp redLedHandle = (LED_handle_tp) NULL; /**< variable to store red led handle */
static LED_handle_tp yellowLedHandle = (LED_handle_tp) NULL; /**< variable to store yellow led handle */
static LED_handle_tp orangeLedHandle = (LED_handle_tp) NULL; /**< variable to store orange led handle */
static uint8_t RegistrationUpdateRetry = UINT8_C(0);

/** variable to store timer handle*/
xTimerHandle registrationUpdateTimer_ptr;
xTimerHandle timeChangeTimer_ptr;

extern Lwm2mDevice_T deviceResourceInfo;

/* local functions ********************************************************** */

/* @brief
 * This function is used to update the time when the device has registered first attempt into the Leshan Server.
 *
 * */
void timeChanged(xTimerHandle pxTimer)
{
    BCDS_UNUSED(pxTimer);

    retcode_t rc = RC_OK;

    /* Here URI_Path is the path points to the "Current Time" Resource */
    Lwm2m_URI_Path_T currentTimeUriPath = { OBJECT_INDEX_VALUE, OBJECT_INSTANCE_INDEX_VALUE, OBJECT_RESOURCE_NUMBER };
    rc = Lwm2mReporting_resourceChanged(&currentTimeUriPath);
    if (rc != RC_OK)
    {
        printf("Could not send notification\r\n");
    }
}

/* @brief
 * This function is used to update the registration.
 *
 * */
void RegistrationUpdate(xTimerHandle pxTimer)
{
    BCDS_UNUSED(pxTimer);
    retcode_t rc = RC_OK;

    rc = Lwm2mRegistration_update(SERVER_INDEX);
    if(RC_OK == rc)
    {
        RegistrationUpdateRetry = UINT8_C(0);
    }
    else
    {
        printf("registration Update failed\r\n");
        RegistrationUpdateRetry++;
    }
    if (RegistrationUpdateRetry >= MAX_REGISTRATION_UPDATE_RETRY)
    {
        printf("Max no. of retries has been done for registration update\n\r");
        if (xTimerStop(registrationUpdateTimer_ptr, TIMERBLOCKTIME) != pdTRUE)
        {
            assert(0);
        }
    }
}

/**
 * @brief The function is to get the registration status of the Device while connection to LWM2M Server 
 *	and It will toggle the Orange LED to indicate the Registration Success State and Red LED will indicate the Registration Failure state.
 */
static void registrationCallback(retcode_t status)
{
    if (status == RC_OK)
    {
        printf("Registration succeeded\r\n");
        LED_setState(redLedHandle, LED_SET_OFF);
        LED_setState(orangeLedHandle, LED_SET_ON);
        LED_setState(yellowLedHandle, LED_SET_OFF);
        if (xTimerStart(registrationUpdateTimer_ptr, TIMERBLOCKTIME) != pdTRUE)
        {
            assert(0);
        }
    }
    else
    {
        LED_setState(redLedHandle, LED_SET_ON);
        LED_setState(orangeLedHandle, LED_SET_OFF);
        LED_setState(yellowLedHandle, LED_SET_OFF);
        printf("registration failed " RC_RESOLVE_FORMAT_STR "\n", RC_RESOLVE(status));
        /* TODO maybe resue the registrationUpdateTimer_ptr for retrying...*/
    }
}
/**
 * @brief The function is to get the Updated Registration status of the Device while re-connection to LWM2M Server.
 */
static void registrationUpdateCallback(retcode_t status)
{
    if (status == RC_OK)
    {
        printf("Registration update succeeded\r\n");
    }
    else
    {
        printf("registration update failed " RC_RESOLVE_FORMAT_STR "\n", RC_RESOLVE(status));
        /* TODO maybe resue the registrationUpdateTimer_ptr for retrying...*/
    }
}
/**
 * @brief The function is to deregister the device in LWM2M Server.
 */
static void deregistrationCallback(retcode_t status)
{
    if (status == RC_OK)
    {
        printf("Deregistration succeeded\r\n");
    }
    else
    {
        printf("deregistration failed " RC_RESOLVE_FORMAT_STR "\n", RC_RESOLVE(status));
        /* TODO maybe resue the registrationUpdateTimer_ptr for retrying...*/
    }
}
/**
 * @brief The callback will get triggerd whenever there is a Notification event.
 */
static void NotificationCallback(retcode_t status)
{
    if (status == RC_OK)
    {
        printf("Notification succeeded\r\n");
        RegistrationUpdateRetry = UINT8_C(0);
        xTimerReset(registrationUpdateTimer_ptr, TIMERBLOCKTIME);
    }
    else
    {
        printf("Notification failed " RC_RESOLVE_FORMAT_STR "\n", RC_RESOLVE(status));
    }
}
/**
 * @brief The function will get called when we trying to register to the LWM2M Server with configured port Number and also It will provide the RED & Orange LED indications to represent the Registration process.
 */
static void applicationCallback(Lwm2m_Event_Type_T eventType, Lwm2m_URI_Path_T *path, retcode_t status)
{
    (void) path;

    if (eventType == LWM2M_EVENT_TYPE_REGISTRATION)
    {
        printf("Registration process has started\r\n");
        LED_setState(redLedHandle, LED_SET_ON);
        LED_setState(orangeLedHandle, LED_SET_ON);
        LED_setState(yellowLedHandle, LED_SET_OFF);
        registrationCallback(status);
    }
    else if (eventType == LWM2M_EVENT_TYPE_REGISTRATION_UPDATE)
    {
        registrationUpdateCallback(status);
    }
    else if (eventType == LWM2M_EVENT_TYPE_DEREGISTRATION)
    {
        deregistrationCallback(status);
    }
    else if (eventType == LWM2M_EVENT_TYPE_NOTIFICATION)
    {
        NotificationCallback(status);
    }
    else if (eventType == LWM2M_EVENT_TYPE_NEW_OBSERVER)
    {
    	printf("before\r\n");
    	Lwm2mReporting_resourceChanged(path);
    	printf("after\r\n");

    }

}

/**
 * @brief This is a function callback when Psk key Exchange of Server.
 */
static uint32_t serverPskCallback(const MsgSendingCtx_T *msgCtx_ptr, const char* peer_identity,
        unsigned char* key, const unsigned int key_max_len UNUSED)
{
    (void) key_max_len;
    return Lwm2mSecurity_PskCallback(&msgCtx_ptr->destAddr, msgCtx_ptr->destPort, peer_identity, (char*) peer_identity, key);
}
/**
 * @brief This is a function callback when Psk key Exchange of Client.
 */
static uint32_t clientPskCallback(const MsgSendingCtx_T *msgCtx_ptr, const char* peer_identiy,
        char* my_identity, const unsigned int id_max_len UNUSED,
        unsigned char* key, const unsigned int key_max_len UNUSED)
{
    (void) id_max_len;
    (void) key_max_len;
    return Lwm2mSecurity_PskCallback(&msgCtx_ptr->destAddr, msgCtx_ptr->destPort, peer_identiy, my_identity, key);
}
/**
 * @brief This will create handles for the LEDs
 *
 * @retval RETURN_FAILURE Led Handle is not created
 * @retval RETURN_SUCCESS Led Handle is created
 *
 */
static led_return_t createLed(void)
{
    led_return_t returnValue = RETURN_FAILURE;

    redLedHandle = LED_create(gpioRedLed_Handle, GPIO_STATE_OFF);
    if (redLedHandle != NULL)
    {
        yellowLedHandle = LED_create(gpioYellowLed_Handle, GPIO_STATE_OFF);
    }
    if (yellowLedHandle != NULL)
    {
        orangeLedHandle = LED_create(gpioOrangeLed_Handle, GPIO_STATE_OFF);
    }
    if (orangeLedHandle != NULL)
    {
        returnValue = RETURN_SUCCESS;
    }
    return (returnValue);
}
/* global functions ********************************************************* */

/* API documentation is in the header file lwm2mInterface.h*/
retcode_t Lwm2mInterfaceStart(Ip_Port_T port)
{
    retcode_t rc = RC_OK;

    Dtls_setClientPskCallback(clientPskCallback);
    Dtls_setServerPskCallback(serverPskCallback);

    Lwm2mServer_T* server = Lwm2m_getServer(SERVER_INDEX);

#if SECURITY_ENABLE==0
    printf("coap without encryption\r\n");
    strncpy(server->serverAddress, SERVER_UNSECURE, strlen(SERVER_UNSECURE));
#elif SECURITY_ENABLE==1
    printf("coap with encryption\r\n");
    strncpy(server->serverAddress, "coaps://217.92.33.109:5894", strlen("coaps://217.92.33.109:5894"));
    strncpy(server->securityInfo.peer_identity, "Leshan", strlen("Leshan"));
    strncpy(server->securityInfo.my_identity, "XDK110_0", strlen("XDK110_0"));
    strncpy(server->securityInfo.secret_key, "Leshan", strlen("Leshan"));
#endif
    server->permissions[0] = LWM2M_READ_ALLOWED;
    Lwm2m_setNumberOfServers(NUMBER_OF_SERVERS);
    /* LED created to represent the registration status */
    createLed();

    rc = Lwm2m_start(port, &applicationCallback);

    if (rc != RC_OK)
    {
        printf("Starting the Lightweight M2M module failed\r\n");
        assert(0);
    }
    uint32_t Ticks = PowerMgt_GetMsDelayTimeInSystemTicks(TIME_CHANGE_NOTIFICATION_DELAY);

    timeChangeTimer_ptr = xTimerCreate((const char * const ) "timeChangeTimer_ptr", // text name, used for debugging.
            Ticks, // The timer period in ticks.
            pdTRUE, // The timers will auto-reload themselves when they expire.
            (void *) NULL, // Assign each timer a unique id equal to its array index.
            timeChanged // On expiry of the timer this function will be called.
            );
    if (timeChangeTimer_ptr == NULL) /* Timer was not created */
    {
        assert(0);
    }
    if (xTimerStart(timeChangeTimer_ptr, TIMERBLOCKTIME) != pdTRUE)
    {
        assert(0);
    }

    uint32_t RegUpdateTicks = PowerMgt_GetMsDelayTimeInSystemTicks(REGISTRATION_UPDATE_DELAY);

    registrationUpdateTimer_ptr = xTimerCreate((const char * const ) "RegistrationUpdate_ptr", // text name, used for debugging.
            RegUpdateTicks, // The timer period in ticks.
            pdTRUE, // The timers will auto-reload themselves when they expire.
            (void *) NULL, // Assign each timer a unique id equal to its array index.
            RegistrationUpdate // On expiry of the timer this function will be called.
            );
    if (registrationUpdateTimer_ptr == NULL) /* Timer was not created */
    {
        assert(0);
    }

    rc = Lwm2mRegistration_register(SERVER_INDEX);
    if (RC_OK != rc)
    {
        printf("Registration Failed\r\n");
    }
    return rc;
}

/* API documentation is in the header file lwm2mInterface.h*/
retcode_t Lwm2mInterfaceInitialize(void)
{
    retcode_t rc = RC_OK;
#if SECURITY_ENABLE==0
    deviceResourceInfo.name = CLIENT_ENDPOINT;
    deviceResourceInfo.secure = false;
#elif SECURITY_ENABLE==1
    deviceResourceInfo.name = "XDK110_SECURE_DTLS";
    deviceResourceInfo.secure = true;
#endif

    printf("Lwm2m_initialize\r\n");
    rc = Lwm2m_initialize(&deviceResourceInfo);
    printf("done\r\n");
    return rc;
}

#endif /* SERVAL_ENABLE_COAP_SERVER */
