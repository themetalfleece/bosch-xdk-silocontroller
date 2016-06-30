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
int32_t mixingMs = 100;
xTaskHandle mixerTask;
#endif

int get_pin(GPIO_handleInfo_t *pin) {
	return (GPIO_getPin((GPIO_handle_tp) pin) == GPIO_STATE_ON) ? 1 : 0;
}

void set_pin(GPIO_handleInfo_t *pin, int status) {
	GPIO_setPin((GPIO_handle_tp) pin, status);
}

void print_status(GPIO_pinStates_t cur_value, char *descr) {
	int status = cur_value == GPIO_STATE_ON ? 1 : 0;
	printf("%d - %s\n\r", status, descr);
}

void read_pins(void) {

	pin_data.full_pin_value = get_pin(&full_pin);
	pin_data.empty_pin_value = get_pin(&empty_pin);

}

void process_values(void) {

	if (silo_state == FILLING) {
		if (pin_data.full_pin_value == LIQUIDDETECTED) {
			pin_data.in_valve_value = VALVEDISABLED;
			silo_state = FULL;

			printf("Full\n\r");
		}
	}

	else if (silo_state == EMPTYING) {
		if (pin_data.empty_pin_value == LIQUIDNOTDETECTED) {
			pin_data.out_valve_value = VALVEDISABLED;
			silo_state = EMPTY;

			printf("Empty\n\r");
		}
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

		printf("Emptying\n\r");
	}
	// fill request
	else if (silo_state == EMPTY) {
		pin_data.in_valve_value = VALVEENABLED;
		silo_state = FILLING;

		printf("Filling\n\r");
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

		printf("Filling\n\r");
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

		printf("Emptying\n\r");
	}

	return (RC_OK);
}

retcode_t exec_stop(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Stop Request\r\n");

	if (silo_state == FILLING || silo_state == EMPTYING) {
		pin_data.out_valve_value = VALVEDISABLED;
		pin_data.in_valve_value = VALVEDISABLED;
		silo_state = STOPPED;

		printf("Stopped\n\r");
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
	silo_state = EMPTYING;

	printf("Initializing\n\r");

	return (RC_OK);
}

#if SILO_MODE == 4
retcode_t exec_heat(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Heat Request\r\n");

	if (silo_state == FULL) {
		pin_data.out_valve_value = VALVEENABLED;
		silo_state = EMPTYING;

		printf("Emptying\n\r");
	}

	return (RC_OK);
}

void mixer(void * pvParameters) {
	(void) pvParameters;
	pin_data.mixer_value = VALVEENABLED;
	set_pin(&mixer_pin, pin_data.mixer_value);
	printf("Mixing\n\r");

	vTaskDelay(mixingMs);

	printf("Finished mixing\n\r");
	pin_data.mixer_value = VALVEDISABLED;
	set_pin(&mixer_pin, pin_data.mixer_value);


//	if (mixerTask != NULL) {
//		vTaskDelete(mixerTask);
//	}

}

retcode_t exec_mix(Lwm2mSerializer_T *serializer_ptr, Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Mix Request\r\n");

	retcode_t rc = RC_OK;

//	if (silo_state == FULL) {

	rc = Lwm2mParser_getInt(parser_ptr, &mixingMs);
	printf("%d\n\r", mixingMs);
	if (rc != RC_OK) {
		return (rc);
	}

//mixer task
	xTaskCreate(mixer, "Mixer", UINT32_C(2048), NULL, UINT32_C(2), &mixerTask);


//	}

	return (RC_OK);
}
#endif

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
#endif

}

void ioInit(void * pvParameters) {

	(void) pvParameters;

	init_pins(pvParameters);

	readPinsTimer = xTimerCreate("circle", UINT32_C(600), pdTRUE, NULL,
			app_circle);




	//OS_taskSuspend(jTask);
}
