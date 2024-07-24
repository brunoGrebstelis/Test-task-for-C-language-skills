/*
 * application.c
 *
 *  Created on: Jul 23, 2024
 *      Author: bruno
 */

#include "application.h"

uint32_t time0 = 0;
uint32_t time1 = 0;
uint32_t time2 = 0;
uint32_t time3 = 0;

uint32_t min_switch_detected = UINT32_MAX;
uint32_t max_switch_detected = UINT32_MAX;
uint32_t last_min_switch_detected = 0;
uint32_t last_max_switch_detected = 0;


uint32_t DEBOUNCE_DELAY = 20;

HomingState state = STATE_INITIAL;

uint32_t MAX_ACTUATOR_TRAVEL_TIME = 10000;
uint32_t MAX_ACTUATOR_TRAVEL_TIME2 = 20000;


void homing_function(void) { // predefined motions that are required in order to configure the system's absolute position after power up
    switch(state) {
        case STATE_INITIAL: // Initial state, start shrinking
            move_actuator_shrink();
            time0 = HAL_GetTick();
            state = STATE_MIN_SWITCH_REACHED;
            break;
        case STATE_MIN_SWITCH_REACHED: // Stop at the min switch, record time1, start extending
            if (HAL_GetTick() - time0 > MAX_ACTUATOR_TRAVEL_TIME) { // Fault detection
                 handle_fault(ERROR_MIN_SWITCH_NOT_REACHED);
            } else if(HAL_GPIO_ReadPin(END_SWITCH_MIN_GPIO_Port, END_SWITCH_MIN_Pin) == GPIO_PIN_SET) {
            	if(HAL_GetTick() - min_switch_detected > DEBOUNCE_DELAY){ // Wait for debounce period
            		stop_actuator();
            		state = STATE_MAX_SWITCH_REACHED;
            		time1 = HAL_GetTick(); // Record the time
            		move_actuator_extend();
            		min_switch_detected = UINT32_MAX;
            	}
            } else {
            	min_switch_detected = UINT32_MAX;
            }
            break;
        case STATE_MAX_SWITCH_REACHED: // Stop at the max switch, record time2, start shrinking
            if (HAL_GetTick() - time1 > MAX_ACTUATOR_TRAVEL_TIME2) { // Fault detection
                 handle_fault(ERROR_MAX_SWITCH_NOT_REACHED);
        	} else if(HAL_GPIO_ReadPin(END_SWITCH_MAX_GPIO_Port, END_SWITCH_MAX_Pin) == GPIO_PIN_SET) {
            	if(HAL_GetTick() - max_switch_detected > DEBOUNCE_DELAY){ // Wait for debounce period
            		stop_actuator();
            		state = STATE_MIN_SWITCH_REACHED2;
            		time2 = HAL_GetTick(); // Record the time
            		move_actuator_shrink();
            		max_switch_detected = UINT32_MAX;
            	}
            } else {
            	max_switch_detected = UINT32_MAX;
            }
            break;
        case STATE_MIN_SWITCH_REACHED2: // Stop at the min switch, record time3, start extending
            if (HAL_GetTick() - time2 > MAX_ACTUATOR_TRAVEL_TIME) { // Fault detection
                 handle_fault(ERROR_MIN_SWITCH_NOT_REACHED);
            } else if(HAL_GPIO_ReadPin(END_SWITCH_MIN_GPIO_Port, END_SWITCH_MIN_Pin) == GPIO_PIN_SET) {
            	if(HAL_GetTick() - min_switch_detected > DEBOUNCE_DELAY){ // Wait for debounce period
            		stop_actuator();
            		state = STATE_MID_POSITION_REACHED;
            		time3 = HAL_GetTick(); // Record the time
            		move_actuator_extend();
            		min_switch_detected = UINT32_MAX;
            	}
            } else {
            	min_switch_detected = UINT32_MAX;
            }
            break;
        case STATE_MID_POSITION_REACHED: // Stop when the actuator reaches middle position
            if(HAL_GetTick() - time3 >= (time2 -time1)/2) {
            	stop_actuator();
            }
            break;
    }
}


void move_actuator_extend(void) {
   HAL_GPIO_WritePin(ACTUATOR_SHRINK_GPIO_Port, ACTUATOR_SHRINK_Pin, GPIO_PIN_RESET);
   HAL_Delay(10); // Driver protection delay
   HAL_GPIO_WritePin(ACTUATOR_EXTEND_GPIO_Port, ACTUATOR_EXTEND_Pin, GPIO_PIN_SET);
}

void move_actuator_shrink(void) {
   HAL_GPIO_WritePin(ACTUATOR_EXTEND_GPIO_Port, ACTUATOR_EXTEND_Pin, GPIO_PIN_RESET);
   HAL_Delay(10); // Driver protection delay
   HAL_GPIO_WritePin(ACTUATOR_SHRINK_GPIO_Port, ACTUATOR_SHRINK_Pin, GPIO_PIN_SET);
}

void stop_actuator(void) {
   HAL_GPIO_WritePin(ACTUATOR_EXTEND_GPIO_Port, ACTUATOR_EXTEND_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(ACTUATOR_SHRINK_GPIO_Port, ACTUATOR_SHRINK_Pin, GPIO_PIN_RESET);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { // Record the time when the switch is first triggered
    UNUSED(GPIO_Pin);
    if(GPIO_Pin == END_SWITCH_MIN_Pin){
    	if(HAL_GetTick() - last_min_switch_detected > DEBOUNCE_DELAY){
    		min_switch_detected = HAL_GetTick();
    		last_min_switch_detected = HAL_GetTick();
    	}
    }

    if(GPIO_Pin == END_SWITCH_MAX_Pin){
    	if(HAL_GetTick() - last_max_switch_detected > DEBOUNCE_DELAY){
    		max_switch_detected = HAL_GetTick();
    		last_max_switch_detected = HAL_GetTick();
    	}
    }
}

void handle_fault(ActuatorError actuatorFault) {
    stop_actuator();
    switch (actuatorFault) {
        case ERROR_MIN_SWITCH_NOT_REACHED:
            printf("Fault: Actuator not reaching the min switch.\n");
            break;
        case ERROR_MAX_SWITCH_NOT_REACHED:
            printf("Fault: Actuator not reaching the max switch/\n");
            break;
    }
}

