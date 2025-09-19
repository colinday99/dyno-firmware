#ifndef DYNAMOMETER_H
#define DYNAMOMETER_H

#ifdef __cplusplus
extern "C" {
#endif

// Include necessary standard libraries
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include <string.h>
// Define any macros or constants
#define TORQUE_CONSTANT 0.0961281708945 // in N.m/A
#define RPM_TO_RAD 0.10471975512
#define GEARBOX_RATIO 0.0625
#define A_MOD 0.6926
#define B_MOD 1.41
#define C_MOD -0.05234
#define D_MOD 0.003506
#define E_MOD -0.0001236
#define F_MOD -1.196e-07

#define N_BUFFER_SAMPLES 6800

// Declare any global variables or extern variables
extern TIM_HandleTypeDef htim7;
extern uint32_t one_ms_counter;
extern uint8_t can_message_pending;
extern double can_rx;
extern uint8_t brakeChopperState;
extern volatile bool sample_flag;

// Declare externally defined functions

// Defined in tmcm1636.c
extern void rotateMotorClockwise(uint32_t velocity_rpm);
extern void rotateMotorCounterClockwise(uint32_t velocity_rpm);
extern void stopMotor(void);
extern void setAxisParameter(uint8_t parameter,uint32_t value);
extern void getAxisParameter(unsigned int);
extern void getActualVelocity(void);
extern void getActualTorque(void);
extern void getActualSourceVoltage(void);
extern void getMaxTorque(void);
extern void setMaxTorque(uint32_t current_mA);
extern void setTargetVelocity(uint32_t velocity_rpm);
extern void setTargetTorque(uint32_t current_mA);
extern void setBrakeChopperOn(void);
extern void setBrakeChopperOff(void);
extern void setBrakeChopperFlag(uint8_t value);
extern void setBrakeChopperVoltage(uint32_t voltage_Vdiv10);
extern void setBrakeChopperHysteresis(uint32_t voltage_Vdiv10);
extern void Arduino_CAN_Send(uint8_t command_number, uint8_t type_number, uint8_t motor_bank_number, uint32_t value);
extern void Arduino_CAN_Receive(uint8_t *);

// Defined in menu.c
extern uint16_t getInt(void);

// Declare any enums or typedefs

// Declare function prototypes

//UART Messages
void displayVoltage(void);
void displayVelocity(void);
void displayTorque(void);
void displayIsometricTorque(void);

//Timing and other misc functions
void waitForExitCode();
void delaySeconds(double seconds);

//Dynamometer functions
double frictionTorque(void);
void dynoIsometricTest(void);
void dynoCalibratedFall(void);
void dynoIsometricTest(void);

#ifdef __cplusplus
}
#endif
#endif // DYNAMOMETER_H
