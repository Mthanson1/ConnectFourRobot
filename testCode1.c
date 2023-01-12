const float DISTTOCENTER = 34.9;
const float DISTTOCOUNT = 180/PI;

void moveVert(int rows, int & posY);
void moveHorz(int columns, int & posX);
void goHome();
void emergencyStop(bool & stopRobot);


task main()
{
nMotorEncoder[motorB] = 0;
int posX = 3, posY = 1;
moveHorz(5, posX);
wait1Msec(1000);
moveHorz(1, posX);
wait1Msec(1000);
nMotorEncoder[motorA] = 0;
moveVert(4, posY);
wait1Msec(1000);
moveVert(2, posY);
wait1Msec(1000);
goHome();
}
//function to move vertical motor
void moveVert(int rows, int & posY) //need to take into account the radius of the gear row 1-6
//Matt
{
	bool stopRobot = true;
	const float GEAR1RAD = 41.06 / 2;//outer wheel diameter
	int encoderInitial = nMotorEncoder[motorA];
	int distance = rows - posY;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR1RAD;
	if  (distance > 0)
	{
		motor[motorA] =  -25;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc)&& stopRobot)
		{
		emergencyStop(stopRobot);
		}
		motor[motorA] = 0; //might need to set 1 to stall
	}
	else if (distance < 0)
	{
		motor[motorA] =  25;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot);
		}
		motor[motorA] = 0; //might need to set 1 to stall
	}
	else
	{
		motor[motorA] = 0;
	}
	//motor stall if dist = 0?
	posY = rows;
}

void moveHorz(int columns, int & posX)
{
	bool stopRobot = true;
	const float GEAR2RAD = 37.1  / 2;
	int encoderInitial = nMotorEncoder[motorB];
	int distance = columns - posX;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR2RAD;
	if (distance > 0)
	{
		motor[motorB] = -25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot);
		}
		motor[motorB] = 0;
	}
	else if (distance < 0)
	{
		motor[motorB] = 25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot);
		}
		motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = 0;
	}
	posX = columns;
}


//code for emergency stop (include whenever running a while loop to move)
void emergencyStop(bool & stopRobot)
{
	if (getButtonPress(buttonDown))
	{
		goHome();
		stopRobot = false;
	}
	else
	{
		stopRobot = true;
	}
}
//function used to reset the positon of the robot
void goHome()
{
	motor[motorA] = motor[motorB] = motor[motorC] = 0;
	if (nMotorEncoder[motorA] > 0)
	{
		motor[motorA] = -25;
		while (nMotorEncoder[motorA] > 0)
		{}
		motor[motorA] = 0;
	}
	else
	{
		motor[motorA] = 25;
		while (nMotorEncoder[motorA] < 0)
		{}
		motor[motorA] = 0;
	}
		if (nMotorEncoder[motorB] > 0)
	{
		motor[motorB] = -25;
		while (nMotorEncoder[motorB] > 0)
		{}
		motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = 25;
		while (nMotorEncoder[motorB] < 0)
		{}
		motor[motorB] = 0;
	}
}
