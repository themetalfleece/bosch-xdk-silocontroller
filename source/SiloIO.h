/*
 * SiloIO.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef SOURCE_SILOIO_H_
#define SOURCE_SILOIO_H_

/* system header files */
#include <gpio_efm32gg.h>
#include <stdint.h>
#include <stdio.h>
#include <gpio.h>
/* additional interface header files */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include "em_gpio.h"
#include "PTD_portDriver_ih.h"
#include "PTD_portDriver_ph.h"

#define SILO_MODE 1
#define SILO_OBJECTID 16663

extern retcode_t exec_fill(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
extern retcode_t exec_empty(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
extern retcode_t exec_stop(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
extern retcode_t exec_initialize(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
#if SILO_MODE == 4
extern retcode_t exec_heat(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
extern retcode_t exec_mix(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
#endif

typedef enum {
	EMPTY, FILLING, FULL, EMPTYING, STOPPED
} Silo_State;

typedef enum {
	VALVEENABLED = 0, VALVEDISABLED = 1
} valve_state;
typedef enum {
	LIQUIDNOTDETECTED = 0, LIQUIDDETECTED = 1
} sensor_detection;

#if SILO_MODE == 4
extern int32_t mixingStatus;
extern int8_t isHeating;
extern int8_t isMixing;
#endif

typedef struct {
	int full_pin_value;
	int empty_pin_value;
	int out_valve_value;
	int in_valve_value;
#if SILO_MODE == 4
	int heater_value;
	int mixer_value;
#endif
} Pin_Data;

static Pin_Data pin_data = { .full_pin_value = LIQUIDNOTDETECTED,
		.empty_pin_value = LIQUIDNOTDETECTED, .out_valve_value = VALVEDISABLED,
		.in_valve_value = VALVEDISABLED,
#if SILO_MODE == 4
		.heater_value = VALVEDISABLED, .mixer_value = VALVEDISABLED,
#endif
		};

static Pin_Data pin_data;

extern Silo_State silo_state;

static GPIO_handleInfo_t full_pin = { .magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortC, .bitIndex = 8, .Direction = GPIO_DIRECTION_INPUT,
		.lockDirection = false, .initDone = false, };

static GPIO_handleInfo_t empty_pin = { .magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortC, .bitIndex = 9, .Direction = GPIO_DIRECTION_INPUT,
		.lockDirection = false, .initDone = false, };

static GPIO_handleInfo_t outvalve_pin = { .magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortD, .bitIndex = 5, .Direction = GPIO_DIRECTION_OUTPUT,
		.lockDirection = false, .initDone = false, };

static GPIO_handleInfo_t invalve_pin = { .magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortD, .bitIndex = 6, .Direction = GPIO_DIRECTION_OUTPUT,
		.lockDirection = false, .initDone = false, };

#if SILO_MODE == 4
static GPIO_handleInfo_t heater_pin = { .magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortC, .bitIndex = 10, .Direction = GPIO_DIRECTION_OUTPUT,
		.lockDirection = false, .initDone = false, };

static GPIO_handleInfo_t mixer_pin = { .magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortA, .bitIndex = 1, .Direction = GPIO_DIRECTION_OUTPUT,
		.lockDirection = false, .initDone = false, };
#endif

#endif /* SOURCE_SILOIO_H_ */
