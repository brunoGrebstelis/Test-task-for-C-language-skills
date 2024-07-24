/*
 * application.h
 *
 *  Created on: Jul 23, 2024
 *      Author: bruno
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "main.h"

typedef enum {
    STATE_INITIAL = 0,
    STATE_MIN_SWITCH_REACHED,
    STATE_MAX_SWITCH_REACHED,
	STATE_MIN_SWITCH_REACHED2,
    STATE_MID_POSITION_REACHED
} HomingState;

typedef enum {
    ERROR_MIN_SWITCH_NOT_REACHED = 0,
    ERROR_MAX_SWITCH_NOT_REACHED
} ActuatorError;

extern uint32_t time0 = 0;
extern uint32_t time1 = 0;
extern uint32_t time2 = 0;
extern uint32_t time3 = 0;

extern uint32_t min_switch_detected = UINT32_MAX;
extern uint32_t max_switch_detected = UINT32_MAX;
extern uint32_t last_min_switch_detected = 0;
extern uint32_t last_max_switch_detected = 0;


extern uint32_t DEBOUNCE_DELAY = 20;

extern HomingState state = STATE_INITIAL;

extern uint32_t MAX_ACTUATOR_TRAVEL_TIME = 10000;
extern uint32_t MAX_ACTUATOR_TRAVEL_TIME2 = 20000;

void move_actuator_extend(void);
void move_actuator_shrink(void);
void stop_actuator(void);
void homing_function(void);
void handle_fault(uint8_t actuatorFault);


#endif /* INC_APPLICATION_H_ */
