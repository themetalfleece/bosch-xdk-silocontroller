/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */

/* header definition ******************************************************** */
#ifndef XDK110_LWM2MCLIENTAPP_CH_H_
#define XDK110_LWM2MCLIENTAPP_CH_H_

/* local interface declaration ********************************************** */

/* local type and macro definitions */

#define CONNECT_TIME_INTERVAL           UINT32_C(3000)          /**< Macro to represent connect time interval */
#define TIMERBLOCKTIME                  UINT32_C(0xffff)        /**< Macro used to define blocktime of a timer*/
#define ZERO                            UINT8_C(0x00)           /**< Macro to define value zero*/
#define ONE                             UINT8_C(0x01)           /**< Macro to define value one*/
#define TASK_STACK_SIZE                 UINT32_C(2048)          /**< Macro to represent the task stack size */
#define TASK_PRIORITY                   UINT32_C(2)             /**< Macro to represent the task priority */
#define WLAN_CONNECT_WPA_SSID                "NETWORK_SSID"         /**< Macros to define WPA/WPA2 network settings */
#define WLAN_CONNECT_WPA_PASS                "NETWORK_WPA_KEY"      /**< Macros to define WPA/WPA2 network settings */
/* Server Connecting Port is user configurable */
#define CLIENT_CONNECT_PORT             UINT32_C(12345)
#if SERVAL_ENABLE_TLS_CLIENT
#define DEST_PORT_NUMBER        		UINT16_C(443)            /**< Macro to define the secure Server Port number */
#else
#define DEST_PORT_NUMBER        		UINT16_C(80)            /**< Macro to define the unsecure Server Port number */
#endif

/* local module global variable declarations */

/* local inline function definitions */

/**
 * @brief This API is called when the LWM2M Client
 *      Connecting to a WLAN Access point.
 *       This function connects to the required AP (SSID_NAME).
 *       The function will return once we are connected and have acquired IP address
 *   @warning
 *      If the WLAN connection fails or we don't acquire an IP address, We will be stuck in this function forever.
 *      Check whether the callback "SimpleLinkWlanEventHandler" or "SimpleLinkNetAppEventHandler" hits once the
 *      sl_WlanConnect() API called, if not check for proper GPIO pin interrupt configuration or for any other issue
 *
 * @retval     RC_OK       IP address returned succesffuly
 *
 * @retval     RC_PLATFORM_ERROR         Error occurred in fetching the ip address
 *
*/
static retcode_t wlanConnect(void);

#endif /* XDK110_LWM2MEXAMPLECLIENTAPP_H_ */

/** ************************************************************************* */
