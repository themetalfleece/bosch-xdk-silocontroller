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

#define PIPE_OBJECTID 16666

// queue variables
#define MAXOWNERS 3
char owners[MAXOWNERS][50];
extern int front;

extern retcode_t exec_acquire(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);
extern retcode_t exec_empty(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr);

typedef enum {
	PROCESSACTIVE = 1, PROCESSINACTIVE = 0
} valve_state;


typedef struct {
	int processA_value;
	int processB_value;
} Pin_Data;

static Pin_Data pin_data = {
		.processA_value = PROCESSINACTIVE,
		.processB_value = PROCESSINACTIVE,
		};

static Pin_Data pin_data;

static GPIO_handleInfo_t processA_pin = {
		.magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortD, .bitIndex = 5,
		.Direction = GPIO_DIRECTION_OUTPUT,
		.lockDirection = false,
		.initDone = false,
};

static GPIO_handleInfo_t processB_pin = {
		.magicWord = GPIO_HANDLE_MAGIC_WORD,
		.port = gpioPortD, .bitIndex = 6,
		.Direction = GPIO_DIRECTION_OUTPUT,
		.lockDirection = false, .initDone = false,
};
#endif /* SOURCE_SILOIO_H_ */
