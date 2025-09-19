/*
 * tmcm1636.h
 *
 *  Created on: Mar 3, 2024
 *      Author: user1
 */

#ifndef INC_TMCM1636_H_
#define INC_TMCM1636_H_

// Motor Defines
#define VOLTAGE 1
#define VELOCITY 2\
#define TORQUE 3
#define CW 0
#define CCW 1
#define MAX_CURRENT 7490 // in mA

// External Variables
extern uint8_t can_message_pending;
extern double can_rx;
extern uint8_t brakeChopperState;

// TMCM1636 Functions
void TMCM1636_Init(void);
void TMCM1636_CAN_Send(uint8_t command_number, uint8_t type_number, uint8_t motor_bank_number, uint32_t value);
void TMCM1636_CAN_Receive(uint8_t *rx_data);

// TMCL Commands
void rotateMotorClockwise(uint32_t velocity_rpm);
void rotateMotorCounterClockwise(uint32_t velocity_rpm);
void stopMotor(void);
void getAxisParameter(unsigned int);
void setAxisParameter(uint8_t,uint32_t);
void getActualVelocity(void);
void getActualTorque(void);
void getActualSourceVoltage(void);
void getMaxTorque(void);
void setMaxTorque(uint32_t current_mA);
void setTargetVelocity(uint32_t velocity_rpm);
void setTargetTorque(uint32_t current_mA);
void setBrakeChopperOn();
void setBrakeChopperOff();
void setBrakeChopperFlag(uint8_t value);
void setBrakeChopperVoltage(uint32_t voltage_V);
void setBrakeChopperHysteresis(uint32_t voltage_V);

#endif /* INC_TMCM1636_H_ */
