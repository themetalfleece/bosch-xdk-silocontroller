/*
 * SiloIO.c
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "SiloIO.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TIMERBLOCKTIME  UINT32_C(0xffff)
#define VALUE_ZERO      UINT8_C(0)

// queue variables
int front = 0;
int rear = -1;
int itemCount = 0;

void set_pin(GPIO_handleInfo_t *pin, int status) {
	GPIO_setPin((GPIO_handle_tp) pin, status);
}

void print_status(GPIO_pinStates_t cur_value, char *descr) {
	int status = cur_value == GPIO_STATE_ON ? 1 : 0;
	printf("%d - %s\r\n", status, descr);
}

void notifyOwner(void) {
	retcode_t rc = RC_OK;

	/* Here URI_Path is the path points to the "state" Resource */
	Lwm2m_URI_Path_T currentOwnerUriPath = { PIPE_OBJECT_INDEX_VALUE,
			PIPE_OBJECT_INSTANCE_INDEX_VALUE, OWNER_OBJECT_RESOURCE_NUMBER };
	rc = Lwm2mReporting_resourceChanged(&currentOwnerUriPath);
	if (rc == RC_OK) {
		printf("Notification sent about owner change\r\n");
	} else {
		printf("Could not send notification\r\n");
	}
}

/* QUEUE FUNCTIONS */
//int peek(void) {
//   return owners[front];
//}
bool isEmpty(void) {
	return itemCount == 0;
}

bool isFull(void) {
	return itemCount == MAXOWNERS;
}

int size(void) {
	return itemCount;
}

void insert(char * data) {

	if (!isFull()) {

		if (rear == MAXOWNERS - 1) {
			rear = -1;
		}
		rear++;
		strcpy(owners[rear], data);
		itemCount++;
	}
	// TODO add error rc if full
}

void makeOwnersInactive(void) {
	set_pin(&processA_pin, PROCESSINACTIVE);
	set_pin(&processB_pin, PROCESSINACTIVE);
}

void removeData(void) {
	char data[50];
//	strcpy(data, owners[front]);
	if (itemCount != 0) {
		memset(&owners[front], 0, sizeof(owners[front]));
		front++;

		if (front == MAXOWNERS) {
			front = 0;
		}
		itemCount--;
		makeOwnersInactive();
	}
//   return data;
}
/* END OF QUEUE FUNCTIONS */

void makeOwnerActive(void) {
	// liqueur.server1.liqueurplant.ProcessA
	if (!strcmp(owners[front], "liqueur.server1.liqueurplant.ProcessA")){
		printf("process A\r\n");
		set_pin(&processA_pin, PROCESSACTIVE);
	}
	// liqueur.server1.liqueurplant.ProcessB
	else if (!strcmp(owners[front], "liqueur.server1.liqueurplant.ProcessB")) {
		printf("process B\r\n");
		set_pin(&processB_pin, PROCESSACTIVE);
	}
	else {
		printf("random process\r\n");
		set_pin(&processA_pin, PROCESSACTIVE);
	}
}

retcode_t exec_acquire(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Acquire Request\r\n");

	retcode_t rc = RC_OK;

	StringDescr_T tempOwner;
	rc = Lwm2mParser_getString(parser_ptr, &tempOwner);
	char owner[50];
//	memset(&owner[0], 0, sizeof(owner));
	StringDescr_copy(&tempOwner, owner);
//	printf("%s\r\n", owner);
	insert(owner);

	// if it was empty and now has that owner
	if (itemCount == 1) {
		notifyOwner();
		makeOwnerActive();
	}
	return (RC_OK);
}

retcode_t exec_empty(Lwm2mSerializer_T *serializer_ptr,
		Lwm2mParser_T *parser_ptr) {
	(void) parser_ptr;
	(void) serializer_ptr;

	printf("Empty Request\r\n");

	retcode_t rc = RC_OK;

	removeData();
	makeOwnersInactive();
	if (itemCount !=0)
		makeOwnerActive();

	notifyOwner();

	return (RC_OK);
}

static void app_circle(xTimerHandle xTimer) {
	(void) (xTimer);
}

void init_pins(void * pvParameters) {
	(void) pvParameters;

	GPIO_init((GPIO_handle_tp) &processA_pin, processA_pin.Direction,
			GPIO_STATE_OFF);

	GPIO_init((GPIO_handle_tp) &processB_pin, processB_pin.Direction,
			GPIO_STATE_OFF);

}

void ioInit(void * pvParameters) {

	(void) pvParameters;

	init_pins(pvParameters);

}
