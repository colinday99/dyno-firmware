/*
 * tmcm1636.c
 *
 *  Created on: Mar 3, 2024
 *      Author: user1
 */

#include "main.h"
#include "tmcm1636.h"
#include "dynamometer.h"

extern CAN_HandleTypeDef hcan1;

void TMCM1636_Init(void)
{
	setvbuf(stdin, NULL, _IONBF, 0); // Required for Scanf redirect
	printf("\r\nInitializing motor\r\n");
	HAL_Delay(3000); // Wait for motor to power on and initialize
	setBrakeChopperOn();

	setMaxTorque(0);
	HAL_Delay(10);
	setTargetTorque(0);
	printf("Motor Initialization complete\r\n");
	HAL_Delay(1000);
}

void TMCM1636_CAN_Send(uint8_t command_number, uint8_t type_number, uint8_t motor_bank_number, uint32_t value)
{
	// Initialize the command array
	uint8_t command[7] = {0};

	// Prepare command, convert to TMCL format
	command[0] = command_number;
	command[1] = type_number;
	command[2] = motor_bank_number;
	command[3] = (value >> 24)&0xFF;
	command[4] = (value >> 16)&0xFF;
	command[5] = (value >> 8)&0xFF;
	command[6] = (value >> 0)&0xFF;

	// Create Transmission Mailbox
	uint32_t TxMailbox;

	// Create and configure transmission header
	CAN_TxHeaderTypeDef TxHeader;
	TxHeader.DLC = 7;
	TxHeader.StdId = 0x05;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	// Transmit the message
	if( HAL_CAN_AddTxMessage(&hcan1, &TxHeader, command, &TxMailbox) != HAL_OK)
	{
		Error_Handler();
	}
}

void TMCM1636_CAN_Receive(uint8_t *rx_data)
{
	//printf("CAN_RX\r");
	// Reconstruct the value bits into a single int32_t
	int32_t received_value = {0};
	received_value = received_value | (rx_data[3] << 24);
	received_value = received_value | (rx_data[4] << 16);
	received_value = received_value | (rx_data[5] << 8);
	received_value = received_value | (rx_data[6] << 0);

	// If status OK for GAP
	if((rx_data[0] == 0x05) && (rx_data[1] == 0x64) && rx_data[2] == 0x06)
	{
		//printf("GAP OKAY\r");
		can_rx = (double)received_value;
		can_message_pending = false;
	}
}

void rotateMotorClockwise(uint32_t velocity_rpm)
{
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x28;
	uint8_t motor_bank_number = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, velocity_rpm);
}

void rotateMotorCounterClockwise(uint32_t velocity_rpm)
{
	velocity_rpm = -1*velocity_rpm;
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x28;
	uint8_t motor_bank_number = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, velocity_rpm);
}

void stopMotor(void)
{
	// Velocity 0 to stop motor
	//uint8_t command_number = 0x05;
	//uint8_t type_number = 0x28;

	// Default mst command
	uint8_t command_number = 0x03;
	uint8_t type_number = 0x00;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = 0;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void getAxisParameter(unsigned int parameter)
{
	can_message_pending = true;

	uint8_t command_number = 0x06;
	uint8_t type_number = parameter;
	uint8_t motor_bank_number = 0x00;
	uint8_t value = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setAxisParameter(uint8_t parameter,uint32_t value)
{
	can_message_pending = true;

	uint8_t command_number = 0x05;
	uint8_t type_number = parameter;
	uint8_t motor_bank_number = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void getActualVelocity(void)
{
	can_message_pending = true;

	uint8_t command_number = 0x06;
	uint8_t type_number = 0x2A;
	uint8_t motor_bank_number = 0x00;
	uint8_t value = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void getActualTorque(void)
{
	can_message_pending = true;

	uint8_t command_number = 0x06;
	uint8_t type_number = 0x1F;
	uint8_t motor_bank_number = 0x00;
	uint8_t value = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void getActualSourceVoltage(void)
{
	can_message_pending = true;
	uint8_t command_number = 0x06;
	uint8_t type_number = 0xDC;
	uint8_t motor_bank_number = 0x00;
	uint8_t value = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void getMaxTorque(void)
{
	can_message_pending = true;
	uint8_t command_number = 0x06;
	uint8_t type_number = 0x0B;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setMaxTorque(uint32_t current_mA)
{
	if(current_mA > MAX_CURRENT)
		current_mA = MAX_CURRENT; // Over-current protection
	if(current_mA < 0)
		current_mA = 0; // Ensures motor never drives forward
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x0B;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = current_mA;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setTargetTorque(uint32_t current_mA)
{
	if(current_mA > MAX_CURRENT)
		current_mA = MAX_CURRENT; // Over-current protection
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x1E;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = current_mA;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setTargetVelocity(uint32_t velocity_rpm)
{
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x28;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = velocity_rpm;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setBrakeChopperFlag(uint8_t value)
{
	brakeChopperState = value;
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x8C;
	uint8_t motor_bank_number = 0x00;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setBrakeChopperVoltage(uint32_t voltage_Vdiv10)
{
	// Caution: manual has unit as 0.1V, enter 450 for 45V
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x8D;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = voltage_Vdiv10;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}

void setBrakeChopperOn()
{
	setBrakeChopperFlag(true);
	HAL_Delay(100);
	setBrakeChopperVoltage(450);
	HAL_Delay(100);
	setBrakeChopperHysteresis(30);
	HAL_Delay(100);
}

void setBrakeChopperOff()
{
	setBrakeChopperFlag(false);
}

void setBrakeChopperHysteresis(uint32_t voltage_Vdiv10)
{
	// Caution: manual has unit as 0.1V, enter 35 for 3.5V
	uint8_t command_number = 0x05;
	uint8_t type_number = 0x8E;
	uint8_t motor_bank_number = 0x00;
	uint32_t value = voltage_Vdiv10;
	TMCM1636_CAN_Send(command_number, type_number, motor_bank_number, value);
}
