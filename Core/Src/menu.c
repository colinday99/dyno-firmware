/**
 * @file menu.c
 * @brief This is the dyno serial navigation menu.
 * To be used with the dyno serial interface.
 * When UI available, make this only for debugging.
 * 
 * So far menu structure works, but need to implement the actual functions.
 * 
 * @author
 * @date
 */

// Includes
#include "menu.h"

// Private Variables
uint8_t brakeChopperState = 1; // 1 = ON, 0 = OFF, defaults to ON
double testWeight = 0;
uint16_t maxResistingTorque = 0;
double maxVoluntaryContraction = 0;
uint32_t zeroOffset = 0;
uint16_t percentMVC = 50;
uint8_t validation_complete = false;

// Function Definitions

void displayMainMenu(void) {

    stopMotor();
	HAL_Delay(50);
	setMaxTorque(MAX_CURRENT);
	HAL_Delay(50);
	stopMotor();
	HAL_Delay(50);

    printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
    fflush(stdout);
    HAL_Delay(50);

    printf("Dynamometer v1.0\r\n"
                 "1. Isometric\r\n"
                 "2. Isotonic\r\n"
                 "3. Validation\r\n"
                 "Selection: ");
    fflush(stdout);
    HAL_Delay(10);
}

void handleMainMenu(uint8_t input) {
    switch (input) {
        case 1: // Isometric
            stopMotor();
			HAL_Delay(10);
			setMaxTorque(MAX_CURRENT);
			HAL_Delay(10);
			stopMotor();
			HAL_Delay(10);

            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            displayIsometricMenu();
            handleIsometricMenu(getCharacter());
            break;
        case 2:
            enableMotor();
            TMCM1636_Init();
            stopMotor();
            HAL_Delay(10);
            setMaxTorque(MAX_CURRENT);
            HAL_Delay(10);
            stopMotor();
            HAL_Delay(10);

            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            displayIsotonicMenu();
            handleIsotonicMenu(getCharacter());
            break;
        case 3:
            stopMotor();
            HAL_Delay(10);
            setMaxTorque(MAX_CURRENT);
            HAL_Delay(10);
            stopMotor();
            HAL_Delay(10);

            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        default:
            printf("\r\nInvalid choice, please try again\r\n");
            fflush(stdout);

            stopMotor();
			HAL_Delay(10);
			setMaxTorque(MAX_CURRENT);
			HAL_Delay(10);
			stopMotor();
			HAL_Delay(10);

            HAL_Delay(1000);
            displayMainMenu();
            handleMainMenu(getCharacter());
            break;
    }
}

void displayIsometricMenu(void) {
    printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
    fflush(stdout);
    HAL_Delay(50);

    printf("Isometric Menu\r\n"
                          "1. Freewheel\r\n"
                          "2. Measure and apply zero offset | Zero Offset: %lu\r\n"
                          "3. Measure MVC | Current value: %f (uV)\r\n"
                          "4. Back\r\n"
                          "Selection: ", zeroOffset, maxVoluntaryContraction);
    fflush(stdout);
    HAL_Delay(10);
}

void handleIsometricMenu(uint8_t input) {
    switch (input) {
            case 1:
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            

            stopMotor();
            HAL_Delay(10);
            setMaxTorque(0);
            HAL_Delay(10);
            stopMotor();
            HAL_Delay(10);

            printf("Freewheeling, press any number from 0-9 to exit\r\n");
            HAL_Delay(10);
            waitForExitCode();
            displayIsometricMenu();
            handleIsometricMenu(getCharacter());
            break;
        case 2:
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Measuring zero offset\r\n");
            Arduino_CAN_Send(0x01, 0x00, 0x00, 1); // 0x01 = state change, value 1 = zero offset calculation
            while(can_message_pending == true); //Wait for reply and store values
            zeroOffset = can_rx;
            printf("\r\nMeasured value: %lu\r\n", zeroOffset);
            fflush(stdout);
            HAL_Delay(1000);
            displayIsometricMenu();
            handleIsometricMenu(getCharacter());
            break;
        case 3:
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Measuring MVC\r\n");
            printf("Please exert maximum effort until prompted to stop (5 seconds)\r\n");
            dynoIsometricTest();
            //Implement once storage of the values is done implemented
            //maxVoluntaryContraction = measureMVC();
            //printf("\r\nMeasured value: %.3f\r\n", maxVoluntaryContraction);
            //HAL_Delay(1000);
            waitForExitCode();
            displayIsometricMenu();
            handleIsometricMenu(getCharacter());
            break;
        case 4:
            displayMainMenu();
            handleMainMenu(getCharacter());
            break;
        default:
            printf("\r\nInvalid choice, please try again\r\n");
            fflush(stdout);
            HAL_Delay(1000);
            displayIsometricMenu();
            handleIsometricMenu(getCharacter());
            break;
    }
}

void displayIsotonicMenu(void) {
    stopMotor();
			HAL_Delay(10);
			setMaxTorque(200);
			HAL_Delay(10);
			stopMotor();
			HAL_Delay(10);

    printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
    fflush(stdout);
    HAL_Delay(50);

    printf("Isotonic Menu\r\n"
                         "1. Enter patient MVC | Current value: %.3f Nm\r\n"
                         "2. Enter %%MVC for testing | Current value: %u%%\r\n"
                         "3. Begin Isotonic Test\r\n"
                         "4. Back\r\n"
                         "Selection: ", maxVoluntaryContraction, percentMVC);
    fflush(stdout);
    HAL_Delay(10);
}

void handleIsotonicMenu(uint8_t input) {
    switch (input) {
        case 1: // Manual MVC entry if isotonic testing has not been done
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Enter MVC: \r\n");
            maxVoluntaryContraction = getFloat();
            printf("\r\nMVC: %.3f\r\n", maxVoluntaryContraction);
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayIsotonicMenu();
            handleIsotonicMenu(getCharacter());
            break;
        case 2: // Enter percentage for testing, default to 50%
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Enter %%MVC for testing: \r\n");
            percentMVC = getInt();
            while(percentMVC > 100) {
                printf("\r\nInvalid value, please enter a value between 0 and 100: ");
                percentMVC = getInt();
            }
            printf("\r\n%%MVC: %u%%\r\n", percentMVC);
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayIsotonicMenu();
            handleIsotonicMenu(getCharacter());
            break;
        case 3:// Begin Isotonic Test
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            setBrakeChopperFlag(brakeChopperState);
            HAL_Delay(200);
            dynoCalibratedFall();
            validation_complete = true;
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayIsotonicMenu();
            handleIsotonicMenu(getCharacter());
            break;
        case 4:
            displayMainMenu();
            handleMainMenu(getCharacter());
            break;
        default:
            printf("\r\nInvalid choice, please try again\r\n");
            fflush(stdout);

            // DELETE
            printf("Isotonic Switch case");
            fflush(stdout);
            HAL_Delay(5000);
            // END DELETE

            HAL_Delay(1000);
            displayIsotonicMenu();
            handleIsotonicMenu(getCharacter());
            break;
    }
}

void displayValidationMenu(void) {
    printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
    fflush(stdout);
    HAL_Delay(50);

    printf("Validation Menu\r\n"
            "1. Change Brake Chopper State | Current state: %s\r\n"
            "2. Enter Test Weight: %.3f (kg)\r\n"
            "3. Enter Max Resisting Torque: %f (Nm)\r\n"
            "4. Begin Calibrated Testing\r\n"
            "5. Begin Uncalibrated Testing\r\n"
            "6. Back\r\n"
            "Selection: ", brakeChopperState ? "ON" : "OFF", testWeight, desired_torque_Nm);
    fflush(stdout);
    HAL_Delay(10);
}

void handleValidationMenu(uint8_t input) {
    switch (input) {
        case 1: // Change brake chopper state
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Current Brake Chopper state %s\r\n", brakeChopperState ? "ON" : "OFF");
            printf("\r\nON = 1, OFF = 0. \r\n"
                    "\r\nEnter Brake Chopper State: ");
            handleBreakChopper(getCharacter());
            fflush(stdout);
            fflush(stdin);
            break;
        case 2: // Enter test weight
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Current test weight: %.3f\r\n", testWeight);
            printf("Enter new test weight: ");
            testWeight = getFloat();
            printf("\r\nNew test weight: %.3f\r\n", testWeight);
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        case 3: // Enter max resisting torque
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            printf("Current max resisting torque: %f (Nm)\r\n", desired_torque_Nm);
            printf("Enter new max resisting torque: ");
            desired_torque_Nm = getFloat();
            initial_max_torque = 7400; //todo change this temporary variable - done to prevent use in uncalibrated falls
            printf("\r\nNew max resisting torque: %f (Nm)", desired_torque_Nm);
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        case 4: // Begin Calibrated Testing
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            dynoCalibratedFall();
            validation_complete = true;
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        case 5: // Begin Uncalibrated Testing
            printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
            fflush(stdout);
            HAL_Delay(50);
            //dynoUncalibratedFall(); //todo put this back
            validation_complete = true;
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        case 6:
            displayMainMenu();
            handleMainMenu(getCharacter());
            break;
        default:
            printf("\r\nInvalid choice, please try again\r\n");
            fflush(stdout);

            HAL_Delay(1000);
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
    }
}

void displayBreakChopperMenu(void)
{
    printf("\033[2J\033[H"); // ANSI escape code to clear terminal screen
    fflush(stdout);
    HAL_Delay(50);

    char *breakChopperMenu = "Brake Chopper Menu\r\n"
                             "1. ON\r\n"
                             "2. OFF\r\n"
                             "Selection: ";
    printf("%s", breakChopperMenu);
    fflush(stdout);
    HAL_Delay(10);
}

void handleBreakChopper(uint8_t state){
    switch(state)
    {
        case 0:
            printf("\r\nBrake Chopper State: OFF\r\n");
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            brakeChopperState = 0;
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        case 1:
            printf("\r\nBrake Chopper State: ON\r\n");
            fflush(stdout);
            fflush(stdin);
            HAL_Delay(1000);
            brakeChopperState = 1;
            displayValidationMenu();
            handleValidationMenu(getCharacter());
            break;
        default:
            printf("Invalid choice, please try again\r\n");
            fflush(stdout);
            fflush(stdin);

            // DELETE
            printf("Brake Chopper Switch case");
            fflush(stdout);
            HAL_Delay(5000);
            // END DELETE

            HAL_Delay(1000);
            displayBreakChopperMenu();
            handleBreakChopper(getCharacter());
            break;
    }
}

void Arduino_CAN_Send(uint8_t command_type, uint8_t type_number, uint8_t value_sign, uint32_t value)
{
  can_message_pending = true; // Set flag to indicate that a message is pending

	// Initialize the command array
	uint8_t command[7] = {0};

	// Prepare command, convert to my arbitrary format
	command[0] = command_type; // 1 for state change, 0 for value/data
	command[1] = type_number; // 0 unless there's a need for something else
	command[2] = value_sign; // 0 unless there's a negative sign in value/data type of transmission
	command[3] = (value >> 24)&0xFF;
	command[4] = (value >> 16)&0xFF;
	command[5] = (value >> 8)&0xFF;
	command[6] = (value >> 0)&0xFF;

	// Create Transmission Mailbox
	uint32_t TxMailbox;

	// Create and configure transmission header
	CAN_TxHeaderTypeDef TxHeader;
	TxHeader.DLC = 7;
	TxHeader.StdId = 0x100; // Arduino Nano ID
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	// Transmit the message
	if( HAL_CAN_AddTxMessage(&hcan1, &TxHeader, command, &TxMailbox) != HAL_OK)
	{
		Error_Handler();
	}
}

void Arduino_CAN_Receive(uint8_t* rx_data)
{
	// Reconstruct the value bits into a single int32_t
	int32_t received_value = {0};
	received_value = received_value | (rx_data[3] << 24);
	received_value = received_value | (rx_data[4] << 16);
	received_value = received_value | (rx_data[5] << 8);
	received_value = received_value | (rx_data[6] << 0);

    if(rx_data[0] == 0 && rx_data[2] == 1){
        received_value = -received_value;
    }

    can_rx = received_value;
    can_message_pending = false;
}

uint8_t getCharacter(void){
    char input[2];
    fgets(input, 2, stdin);
    return atoi(input);
}

uint16_t getInt(){
    uint16_t output;
	uint8_t valid_input = false;
	while(valid_input != true)
	{
		char input_array[100];
		scanf("%s", input_array);
		int input_length = strlen(input_array);

		valid_input = intValidInputCheck(input_array, input_length);
		if(valid_input != true)
		{
			memset(input_array, 0, input_length);
			printf("Enter an integer value between 1 & 7400: ");
		}
		else
		{
			output = atoi(input_array);
		}
	}
	return output;
}

double getFloat(){
    double output;
	uint8_t valid_input = false;
	while(valid_input != true)
	{
		char input_array[100] = {0};
		scanf("%s", input_array);
		int input_length = strlen(input_array);

		valid_input = floatValidInputCheck(input_array, input_length);
		if(valid_input != true)
		{
			printf("\r\ninput_array: %s", input_array);
			printf("\r\nInvalid input\r\n");
			memset(input_array, 0, input_length);
			printf("Enter a positive value up to 3 decimal places: ");
		}
		else
		{
			output = atof(input_array);
		}
	}
	return output;
}

int floatValidInputCheck(char input_array[], int input_length)
{
    int input_check = true;
    for(int i = 0; i < input_length; i++)
    {
        if(!((input_array[i] >= '0' && input_array[i] <= '9') || input_array[i] == '.'))
        {
        	input_check = false;
        	break;
        }
    }
    return input_check;
}

int intValidInputCheck(char input_array[], int input_length)
{
    int input_check = true;
    for(int i = 0; i < input_length; i++)
    {
        if(!(input_array[i] >= '0' && input_array[i] <= '9'))
        {
            input_check = false;
            break;
        }
    }
    return input_check;
}
