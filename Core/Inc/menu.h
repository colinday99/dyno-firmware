#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

// Include necessary standard library headers
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <menu.h>

// Include necessary custom library headers
#include "dynamometer.h"
#include "tmcm1636.h"
#include "main.h"

// Include external variables
extern CAN_HandleTypeDef hcan1; // Declare the CAN handle
extern void enableMotor();
extern int initial_max_torque;
extern double desired_torque_Nm;

// Menu Function Prototypes
void displayMainMenu(void);
void handleMainMenu(uint8_t input);
void displayIsometricMenu(void);
void handleIsometricMenu(uint8_t input);
void displayIsotonicMenu(void);
void handleIsotonicMenu(uint8_t input);
void displayValidationMenu(void);
void handleValidationMenu(uint8_t input);
void handleBreakChopper(uint8_t state);

// Input Functions
uint8_t getCharacter(void);
double getFloat(void);
int floatValidInputCheck(char input_array[], int input_length);
int intValidInputCheck(char input_array[], int input_length);
uint16_t getInt(void);

// Other Functions
void waitForExitCode();

// Communication functions
// CAN Messages
void Arduino_CAN_Send(uint8_t command_number, uint8_t type_number, uint8_t motor_bank_number, uint32_t value);
void Arduino_CAN_Receive(uint8_t *);

#ifdef __cplusplus
}
#endif

#endif // MENU_H
