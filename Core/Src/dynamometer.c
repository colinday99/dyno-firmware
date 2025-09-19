/**
 * @file dynofunctions.c
 * @brief Description of the file's purpose.
 * 
 * Detailed description of the file and its functionality.
 * 
 * @author Your Name
 * @date Date
 */

 #include "dynamometer.h"

// Dyno Testing Variables
double test_weight = 0;
uint32_t initial_max_torque = 7400;
double desired_torque_Nm = 0;
//static unsigned int currents[N_BUFFER_SAMPLES];
static int velocities[N_BUFFER_SAMPLES];
//static uint32_t status_flags[N_BUFFER_SAMPLES];
static int velocity_pi_error_sum[N_BUFFER_SAMPLES];
static int velocity_pi_error[N_BUFFER_SAMPLES];

double frictionTorque(void)
{
	if(abs(can_rx) < 100)
		return 0;
	// Motor Calibration Constants [ From LeaveOneOut_40N ]
	// double I_Gb = 0.0092667; // kg*m^2 inertia.
	double a = -1.809368661;
	double b = 0.130673924;
	double c = 1.709569783;
	double x = (GEARBOX_RATIO*abs(can_rx))*RPM_TO_RAD;

	//friction_torque(Nm) = a*x^b + c; from results
	double friction_torque_gearbox = a*pow(x, b) + c; // In N.m

	double friction_torque_motor = friction_torque_gearbox*GEARBOX_RATIO;
	double friction_torque_motor_mA = (friction_torque_motor/TORQUE_CONSTANT)*1000;
	return friction_torque_motor_mA;
}

void dynoIsometricTest(void)
{
	printf("%cc", 27); // Clear Terminal
	printf("Time(s),Voltage(uV)\r\n");

  //TODO: Re-enable once EMI issues fixed and motor can be ON during isometric test.
	//setTargetVelocity(0);
	//HAL_Delay(10);
	//setMaxTorque(MAX_CURRENT);
	HAL_TIM_Base_Start_IT(&htim7);

	Arduino_CAN_Send(0x01, 0x00, 0x00, 2); // 0x01 = state change, value 2 = Start Isometric Test
	HAL_Delay(10);

	while(one_ms_counter<5000)
	{
		displayIsometricTorque();
	}
	
	HAL_TIM_Base_Stop_IT(&htim7);
	Arduino_CAN_Send(0x01, 0x00, 0x00, 3); // 0x01 = state change, value 3 = go to idle state
	one_ms_counter = 0;
}

void displayIsometricTorque(void)
{
  //Wait for reply and store values
  while(can_message_pending == true); //Wait for reply and store values
  printf("%.3f,%.f\r\n",(double)one_ms_counter/1000, can_rx);
  can_message_pending = false;
}

void displayVoltage(void)
{
	//Wait for reply and store values
	while(can_message_pending == true); //Wait for reply and store values
	printf("%.3f,%.2f,",(double)one_ms_counter/1000, can_rx/10);
	can_message_pending = false;
}

void displayVelocity(void)
{
	//Wait for reply and store values
	while(can_message_pending == true); //Wait for reply and store values
	printf("%.0f,",can_rx);
	can_message_pending = false;
}

void storeVelocity(void)
{
	//Wait for reply and store values
	while(can_message_pending == true); //Wait for reply and store values
	printf("%.0f,",can_rx);
	can_message_pending = false;
}


void displayTorque(void)
{
	//Wait for reply and store values
	while(can_message_pending == true); //Wait for reply and store values
	printf("%.0f,", can_rx);
	can_message_pending = false;
}

void displayMaxTorque(void)
{
	//Wait for reply and store values
	while(can_message_pending == true); //Wait for reply and store values
	printf("%.0f,", can_rx);
	can_message_pending = false;
}

void dynoUncalibratedFall(void)
{
	setBrakeChopperOn();
	printf("CJD: Set Brake Chopper On\n\r");
	printf("%cc", 27); // Clear Terminal
	printf("Time(s),Voltage(V),Velocity(rpm),Torque(mA), Weight(kg), %lf, MVC Torque, %ld, Not Calibrated\r\n", test_weight, initial_max_torque);
	setTargetVelocity(0);
	HAL_Delay(10);
	setMaxTorque(initial_max_torque);
	HAL_TIM_Base_Start_IT(&htim7);
	while(one_ms_counter<17500)
	{
		getActualSourceVoltage();
		delaySeconds(0.001);
		displayVoltage();
		getActualVelocity();
		delaySeconds(0.001);
		displayVelocity();
		setMaxTorque(initial_max_torque - 0);
		delaySeconds(0.001);
		getActualTorque();
		delaySeconds(0.001);
		displayTorque();
		delaySeconds(0.01);
		printf("\r\n");
	}
	HAL_TIM_Base_Stop_IT(&htim7);
	one_ms_counter = 0;
	waitForExitCode();
}



void colin_model(double desired_torque, unsigned int *output_current){
	// naming these is getting ambiguous okay
	/*
	 * double frictionTorque(void)
		{
			if(abs(can_rx) < 100)
				return 0;
			// Motor Calibration Constants [ From LeaveOneOut_40N ]
			// double I_Gb = 0.0092667; // kg*m^2 inertia.
			double a = -1.809368661;
			double b = 0.130673924;
			double c = 1.709569783;
			double x = (GEARBOX_RATIO*abs(can_rx))*RPM_TO_RAD;

			//friction_torque(Nm) = a*x^b + c; from results
			double friction_torque_gearbox = a*pow(x, b) + c; // In N.m

			double friction_torque_motor = friction_torque_gearbox*GEARBOX_RATIO;
			double friction_torque_motor_mA = (friction_torque_motor/TORQUE_CONSTANT)*1000;
			return friction_torque_motor_mA;
		}
	 */
	double omega = (GEARBOX_RATIO*abs(can_rx))*RPM_TO_RAD;
	double a = F_MOD;
	double b = D_MOD+E_MOD*omega;
	double c = A_MOD+B_MOD*omega+C_MOD*omega*omega-desired_torque;

	// solving the equation
	double disc = b*b-4*a*c;
	if(disc<0){
		// do not modify output_current, keep it where it was.
		return;
	}
	else{
		int result = (int)((-b+sqrt(disc))/(2*a)); // quadratic formula solve
		if(result < 0) result = 0;
		if(result > 7400) result = 7400; //todo: establish max current as a define
		*output_current = result;
		return;
	}

}

void dynoCalibratedFall(void)
{
	setBrakeChopperOn();
	printf("%cc", 27); // Clear Terminal
	//printf("Time(s),Voltage(V),Velocity(rpm), Torque(mA), maxTorque(mA), Weight(kg), %lf, MVC Torque, %ld, Calibrated Fall, Chopper State, %d\r\n,", test_weight, initial_max_torque, brakeChopperState);
	setTargetVelocity(0);
	HAL_Delay(10);
	// Report P and I
	getAxisParameter(72);
	while(can_message_pending == true);
	int velocity_p = (int)can_rx;

	getAxisParameter(73);
	while(can_message_pending == true);
	int velocity_i = (int)can_rx;
	printf("Velocity P (startup)=%d\r\n",velocity_p);
	printf("Velocity I (startup)=%d\r\n",velocity_i);

	velocity_p = 255;
	velocity_i = 0;
	printf("Setting Velocity P=%d ...\t",velocity_p);
	setAxisParameter(72,velocity_p);
	printf("Done\r\n");
	printf("Setting Velocity I=%d ...\t",velocity_i);
	setAxisParameter(73,velocity_i);
	printf("Done\r\n");

	unsigned int output_current = 0;
	colin_model(desired_torque_Nm,&output_current);
	setMaxTorque(output_current);
	HAL_TIM_Base_Start_IT(&htim7);



	unsigned int idx=0;

	while(one_ms_counter<10000 && idx<N_BUFFER_SAMPLES)
	{
		if (!sample_flag) continue;
		sample_flag = false;
		if(one_ms_counter%2==0){ // every other ms, get all the parameters
			getActualVelocity();
			while(can_message_pending == true);
			velocities[idx] = can_rx;

			colin_model(desired_torque_Nm,&output_current);
			setMaxTorque(output_current);

			getAxisParameter(77); // velocity error sum param
			while(can_message_pending == true);
			velocity_pi_error_sum[idx] = (int)can_rx;

			getAxisParameter(80); // velocity error param
			while(can_message_pending == true);
			velocity_pi_error[idx] = (int)can_rx;
			idx++;
		} else {
			// every other ms, just update the control loop
			getActualVelocity();
			while(can_message_pending == true);
			colin_model(desired_torque_Nm,&output_current);
			setMaxTorque(output_current);
		}
	}
	HAL_TIM_Base_Stop_IT(&htim7);
	float runtime_s = ((float)one_ms_counter)/1000.00;
	one_ms_counter = 0;
	unsigned int max_idx = idx;
	printf("Weight(kg)=%lf, MVC Torque=%lf, Calibrated Fall\r\n,", test_weight, desired_torque_Nm);
	printf("Ran for %.2fs, collected %d samples, sample rate of %.2fHz ",runtime_s,max_idx,((float)max_idx)/runtime_s);
	//printf("Time(s),Velocity(rpm), Max Current [mA],Status Flags\r\n");


	getAxisParameter(72);
	while(can_message_pending == true);
	velocity_p = (int)can_rx;

	getAxisParameter(73);
	while(can_message_pending == true);
	velocity_i = (int)can_rx;
	printf("Velocity P=%d\r\n",velocity_p);
	printf("Velocity I=%d\r\n",velocity_i);
	printf("Time(s),Velocity(rpm), Velocity PI Error, Velocity PI Error Sum\r\n");
	//char flagString[200];
	//uint32_t status_flag = 0;
	for(idx=0; idx<max_idx; idx++){
		/*strcpy(flagString,""); // clear the flag string
		status_flag = status_flags[idx]; // copy over the status flag so we don't have to access it each time
		if(status_flag & 0x01) strcat(flagString,"Overcurrent, ");
		if(status_flag & 0x02) strcat(flagString,"Undervoltage, ");
		if(status_flag & 0x04) strcat(flagString,"Overvoltage, ");
		if(status_flag & 0x08) strcat(flagString,"Overtemp, ");
		if(status_flag & 0x10) strcat(flagString,"Motor Halted, ");
		if(status_flag & 0x20) strcat(flagString,"Hall error, ");
		if(status_flag & 0x40) strcat(flagString,"Driver error, ");
		if(status_flag & 0x80) strcat(flagString,"Init error, ");
		if(status_flag & 0x100) strcat(flagString,"Stop mode, ");
		if(status_flag & 0x200) strcat(flagString,"Velocity mode, ");
		if(status_flag & 0x400) strcat(flagString,"Position mode, ");
		if(status_flag & 0x800) strcat(flagString,"Torque mode, ");
		if(status_flag & 0x4000) strcat(flagString,"Position end, ");
		if(status_flag & 0x8000) strcat(flagString,"Module initialized");
		printf("%.4f,%d,%d,%s\r\n",((float)idx)/500,velocities[idx],currents[idx],flagString); // divisor 1000 for time when doing direct ms, 500 for when doing every other.*/
		printf("%.4f,%d,%d,%d\r\n",((float)idx)/500,velocities[idx],velocity_pi_error[idx],velocity_pi_error_sum[idx]); // divisor 1000 for time when doing direct ms, 500 for when doing every other.
	}

	waitForExitCode();
}

void waitForExitCode()
{
	uint16_t exit_value = getInt();
	while (exit_value < 1 || exit_value > 9)
	{
		printf("Invalid entry. Please enter a valid digit between 1 and 9: ");
		exit_value = getInt();
	}
}

void delaySeconds(double seconds)
{
	uint32_t current_time = one_ms_counter;
	double stop_time = (double)current_time + seconds*1000;
	while(one_ms_counter < stop_time);
}
