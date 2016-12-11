/*
 * SiloIO.c
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "SiloIO.h"

#define TIMERBLOCKTIME  UINT32_C(0xffff)
#define VALUE_ZERO      UINT8_C(0)

static xTimerHandle readPinsTimer;

Silo_State silo_state = EMPTY;

#if SILO_MODE == 4
int32_t heatingMs = 100;
int32_t mixingMs = 100;
static xTimerHandle stopHeatingTimer;
static xTimerHandle stopMixingTimer;
int8_t filling_completed = 0;
int8_t emptying_completed = 0;
int8_t heating_completed = 0;
int8_t mixing_completed = 0;
int8_t target_temperature = 32;
#endif

int get_pin(GPIO_handleInfo_t *pin) {
	return (GPIO_getPin((GPIO_handle_tp) pin) == GPIO_STATE_ON) ? 1 : 0;
}

void set_pin(GPIO_handleInfo_t *pin, int status) {
	GPIO_setPin((GPIO_handle_tp) pin, status);
}

void print_status(GPIO_pinStates_t cur_value, char *descr) {
	int status = cur_value == GPIO_STATE_ON ? 1 : 0;
	printf("%d - %s\r\n", status, descr);
}

void read_pins(void) {

	pin_data.full_pin_value = get_pin(&full_pin);
	pin_data.empty_pin_value = get_pin(&empty_pin);

}

void notifyState(void) {
	retcode_t rc = RC_OK;

	/* Here URI_Path is the path points to the "state" Resource */
	Lwm2m_URI_Path_T currentStateUriPath = { SILO_OBJECT_INDEX_VALUE, SILO_OBJECT_INSTANCE_INDEX_VALUE, STATE_OBJECT_RESOURCE_NUMBER };
	rc = Lwm2mReporting_resourceChanged(&currentStateUriPath);
	if (rc == RC_OK)
	{
		printf("Notification sent about state change\r\n");
	}
	else
	{
		printf("Could not send notification\r\n");
	}
}

void notifyFillingCompleted(void) {

	retcode_t rc = RC_OK;

		/* Here URI_Path is the path points to the "state" Resource */
		Lwm2m_URI_Path_T currentStateUriPath = { SILO_OBJECT_INDEX_VALUE, SILO_OBJECT_INSTANCE_INDEX_VALUE, FILLING_COMPLETED_OBJECT_RESOURCE_NUMBER };
		rc = Lwm2mReporting_resourceChanged(&currentStateUriPath);
		if (rc == RC_OK)
		{
			printf("Notification sent about filling completed\r\n");
		}
		else
		{
			printf("Could not send notification\r\n");
		}

}

void notifyEmptyingCompleted(void) {

	retcode_t rc = RC_OK;

		/* Here URI_Path is the path points to the "state" Resource */
		Lwm2m_URI_Path_T currentStateUriPath = { SILO_OBJECT_INDEX_VALUE, SILO_OBJECT_INSTANCE_INDEX_VALUE, EMPTYING_COMPLETED_OBJECT_RESOURCE_NUMBER };
		rc = Lwm2mReporting_resourceChanged(&currentStateUriPath);
		if (rc == RC_OK)
		{
			printf("Notification sent about emptying completed\r\n");
		}
		else
		{
			printf("Could not send notification\r\n");
		}

}

void notifyHeatingCompleted(void) {

	retcode_t rc = RC_OK;

		/* Here URI_Path is the path points to the "state" Resource */
		Lwm2m_URI_Path_T currentStateUriPath = { SILO_OBJECT_INDEX_VALUE, SILO_OBJECT_INSTANCE_INDEX_VALUE, HEATING_COMPLETED_OBJECT_RESOURCE_NUMBER };
		rc = Lwm2mReporting_resourceChanged(&currentStateUriPath);
		if (rc == RC_OK)
		{
			printf("Notification sent about heating completed\r\n");
		}
		else
		{
			printf("Could not send notification\r\n");
		}

}

void notifyMixingCompleted(void) {

	retcode_t rc = RC_OK;

		/* Here URI_Path is the path points to the "state" Resource */
		Lwm2m_URI_Path_T currentStateUriPath = { SILO_OBJECT_INDEX_VALUE, SILO_OBJECT_INSTANCE_INDEX_VALUE, MIXING_COMPLETED_OBJECT_RESOURCE_NUMBER };
		rc = Lwm2mReporting_resourceChanged(&currentStateUriPath);
		if (rc == RC_OK)
		{
			printf("Notification sent about mixing completed\r\n");
		}
		else
		{
			printf("Could not send notification\r\n");
		}

}

void process_values(void) {

	Silo_State oldState = silo_state;

	if (silo_state == FILLING) {
		if (pin_data.full_pin_value == LIQUIDDETECTED) {
			pin_data.in_valve_value = VALVEDISABLED;
			silo_state = FULL;

			filling_completed = 1;
			notifyFillingCompleted();

			printf("Full\r\n");
		}
	}

	else if (silo_state == EMPTYING || silo_state == INITIALIZING) {
		if (pin_data.empty_pin_value == LIQUIDNOTDETECTED) {
			pin_data.out_valve_value = VALVEDISABLED;
			silo_state = EMPTY;

			emptying_completed = 1;
			notifyEmptyingCompleted();

			printf("Empty\r\n");
		}
	}

	if (silo_state != oldState)
	{
		notifyState();
	}

}

void write_pins(void) {
	set_pin(&outvalve_pin, pin_data.out_valve_value);
	set_pin(&invalve_pin, pin_data.in_valve_value);
}

void emulate_exec(void) {

	//empty request
	if (silo_state == FULL) {
		pin_data.out_valve_value = VALVEENABLED;
		silo_state = EMPTYING;

		printf("Emptying\r\n");
	}
	// fill request
	else if (silo_state == EMPTY) {
		pin_data.in_valve_value = VALVEENABLED;
		silo_state = FILLING;

		printf("Filling\r\n");
	}

}

retcode_t exec_fill(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Fill Request\r\n");

	if (silo_state == EMPTY || silo_state == STOPPED) {
		pin_data.in_valve_value = VALVEENABLED;
		pin_data.out_valve_value = VALVEDISABLED;
		silo_state = FILLING;

		notifyState();
		filling_completed = 0;
		notifyFillingCompleted();

		printf("Filling\r\n");
	}

	return (RC_OK);
}

retcode_t exec_empty(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Empty Request\r\n");

	if (silo_state == FULL || silo_state == STOPPED) {
		pin_data.in_valve_value = VALVEDISABLED;
		pin_data.out_valve_value = VALVEENABLED;
		silo_state = EMPTYING;

		notifyState();
		emptying_completed = 0;
		notifyEmptyingCompleted();

		printf("Emptying\r\n");
	}

	return (RC_OK);
}

retcode_t exec_initialize(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Initialize Request\r\n");

	pin_data.in_valve_value = VALVEDISABLED;
	pin_data.out_valve_value = VALVEENABLED;

	filling_completed = 0;
	emptying_completed = 0;

#if SILO_MODE == 4
	pin_data.heater_value = VALVEDISABLED;
	set_pin(&heater_pin, pin_data.heater_value);
	heating_completed = 0;
	pin_data.mixer_value = VALVEDISABLED;
	set_pin(&mixer_pin, pin_data.mixer_value);
	mixing_completed = 0;
#endif
	silo_state = INITIALIZING;

	notifyState();
	notifyFillingCompleted();
	notifyEmptyingCompleted();
	notifyHeatingCompleted();
	notifyMixingCompleted();

	printf("Initializing\r\n");

	return (RC_OK);
}

#if SILO_MODE == 4

void startHeating(void * pvParameters) {
	(void) pvParameters;
	pin_data.heater_value = VALVEENABLED;
	set_pin(&heater_pin, pin_data.heater_value);
	heating_completed = 0;
	notifyHeatingCompleted();
	printf("Heating\r\n");

}

void stopHeating(void * pvParameters) {
	(void) pvParameters;

	pin_data.heater_value = VALVEDISABLED;
	set_pin(&heater_pin, pin_data.heater_value);
	heating_completed = 1;
	notifyHeatingCompleted();
	printf("Finished heating\r\n");
}

retcode_t exec_heat(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Heat Request\r\n");

	retcode_t rc = RC_OK;

	if (silo_state == FULL || silo_state == STOPPED) {

		rc = Lwm2mParser_getInt(parser_ptr, &heatingMs);
		heatingMs = (heatingMs-25) * 1000 + 200;
//		printf("%d\r\n", heatingMs);

		if (rc != RC_OK) {
			return (rc);
		}

		startHeating(null);
		xTimerChangePeriod(stopHeatingTimer, heatingMs, 0);
		if (xTimerStart(stopHeatingTimer, 0) != pdPASS) {
			printf("The Heating timer could not be activated\r\n");
		}
	}

	return (RC_OK);
}

void startMixing(void * pvParameters) {
	(void) pvParameters;
	pin_data.mixer_value = VALVEENABLED;
	set_pin(&mixer_pin, pin_data.mixer_value);
	mixing_completed = 0;
	notifyMixingCompleted();
	printf("Mixing\r\n");

}

void stopMixing(void * pvParameters) {
	(void) pvParameters;

	pin_data.mixer_value = VALVEDISABLED;
	set_pin(&mixer_pin, pin_data.mixer_value);
	mixing_completed = 1;
	notifyMixingCompleted();
	printf("Finished mixing\r\n");
}

retcode_t exec_mix(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Mix Request\r\n");

	retcode_t rc = RC_OK;

	if (silo_state == FULL || silo_state == STOPPED) {

		rc = Lwm2mParser_getInt(parser_ptr, &mixingMs);

		printf("%d\r\n", mixingMs);

		if (rc != RC_OK) {
			return (rc);
		}

		startMixing(null);
		xTimerChangePeriod(stopMixingTimer, mixingMs, 0);
		if (xTimerStart(stopMixingTimer, 0) != pdPASS) {
			printf("The Mixing timer could not be activated\r\n");
		}
	}

	return (RC_OK);
}
#endif

//TODO
retcode_t exec_stop(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Stop Request\r\n");

	pin_data.out_valve_value = VALVEDISABLED;
	pin_data.in_valve_value = VALVEDISABLED;
	silo_state = STOPPED;

#if SILO_MODE == 4
	if (!heating_completed) {
		xTimerStop(stopHeatingTimer, 0);
		stopHeating(null);
	}
	if (!mixing_completed) {
		xTimerStop(stopMixingTimer, 0);
		stopMixing(null);
	}

#endif

	notifyState();

	printf("Stopped\r\n");

	return (RC_OK);
}

# warning incomplete
void setTargetTemp(void){

}

static void app_circle(xTimerHandle xTimer) {
	(void) (xTimer);

	read_pins();

	process_values();

	//emulate_exec();

	//notify server (for empty/full)

	write_pins();
}

void init_pins(void * pvParameters) {
	(void) pvParameters;

	GPIO_init((GPIO_handle_tp) &full_pin, full_pin.Direction, GPIO_STATE_OFF);
	GPIO_PinModeSet(full_pin.port, full_pin.bitIndex, gpioModeInput, 0);

	GPIO_init((GPIO_handle_tp) &empty_pin, empty_pin.Direction, GPIO_STATE_OFF);
	GPIO_PinModeSet(empty_pin.port, empty_pin.bitIndex, gpioModeInput, 0);

	GPIO_init((GPIO_handle_tp) &outvalve_pin, outvalve_pin.Direction,
			GPIO_STATE_ON);

	GPIO_init((GPIO_handle_tp) &invalve_pin, invalve_pin.Direction,
			GPIO_STATE_ON);

#if SILO_MODE == 4
	GPIO_init((GPIO_handle_tp) &mixer_pin, mixer_pin.Direction, GPIO_STATE_ON);
	GPIO_init((GPIO_handle_tp) &heater_pin, mixer_pin.Direction, GPIO_STATE_ON);
#endif

}

void ioInit(void * pvParameters) {

	(void) pvParameters;

	init_pins(pvParameters);

	readPinsTimer = xTimerCreate("circle", UINT32_C(600), pdTRUE, NULL,
			app_circle);
	xTimerStart(readPinsTimer, TIMERBLOCKTIME);
#if SILO_MODE == 4
	stopHeatingTimer = xTimerCreate("stopHeating", UINT32_C(1000), pdFALSE,
			NULL, stopHeating);

	stopMixingTimer = xTimerCreate("stopMixing", UINT32_C(1000), pdFALSE, NULL,
			stopMixing);
#endif
}
