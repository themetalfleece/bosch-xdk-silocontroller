/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */
#ifndef XDK110_APPLWM2MINTERFACE_H_
#define XDK110_APPLWM2MINTERFACE_H_

#include <Serval_Defines.h>

#if SERVAL_ENABLE_LWM2M
#include <Serval_Types.h>
#include <Serval_CoapServer.h>
#include <Serval_Network.h>

/* public type and macro definitions */

#define OBJECT_INDEX_VALUE                   INT32_C(0)        /**< Macro used to define the object index*/
#define OBJECT_INSTANCE_INDEX_VALUE          INT32_C(0)        /**< Macro used to define object Instance number*/
#define OBJECT_RESOURCE_NUMBER               INT32_C(13)       /**< Macro used to define the resource number*/
 /**<Macro used to define the server index which can take any value between 0 to (NUMBER_OF_SERVERS -1 ).Here number of servers is configured as 1,
  *  hence to get memory of that server,the index value is passed as 0 */
#define SERVER_INDEX                         UINT32_C(0)
#define NUMBER_OF_SERVERS                    UINT32_C(1)        /**< Macro used to define number of servers*/
/**< Macro used to define the time interval to send notification to the server about the time change*/
#define TIME_CHANGE_NOTIFICATION_DELAY       5000
#define REGISTRATION_UPDATE_DELAY            20000              /**< Macro used to define delay for registration update*/
#define TIMERBLOCKTIME                       UINT32_C(0xffff)   /**< Macro used to define block time of a timer*/
#define MAX_REGISTRATION_UPDATE_RETRY        UINT8_C(10)        /**< Macro used to define maximum retry for registration update*/
/** enum to represent LED return status */
typedef enum led_return_e
{
    RETURN_FAILURE,
    RETURN_SUCCESS,
} led_return_t;
/**
 * @brief The function initializes the LWM2M Interface between Device and the LWM2M Server.
 *
 * @return  RC_OK Interface Initialization happend successfully.
 * @return  RC_SERVAL_ERROR Initialization Failure.
 */
retcode_t Lwm2mInterfaceInitialize(void);

/**
 * @brief The function Starts the LWM2M Interface to the LWM2M Server to the configured port Number.
 * @param[in] port port number of LWM2M Interface between Device and LWM2M Server
 *
 * @return  RC_OK Interface Started between Device and LWM2M Server.
 * @return  RC_SERVAL_ERROR Interface Not started to the Configured Port Number.
 */
retcode_t Lwm2mInterfaceStart(Ip_Port_T port);

#endif /* SERVAL_ENABLE_LWM2M */

#endif /* XDK110_APPLWM2MINTERFACE_H_ */
