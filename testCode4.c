const float DISTTOCENTER = 34.9;
const float DISTTOCOUNT = 180/PI;
const int MAXCOLUMN = 7;
const int MAXROW = 6;
const int PLAYER = 2;
//const int ROBOT = 1;
int board[MAXROW][MAXCOLUMN];//Nassar said this was ok
int columnH[MAXCOLUMN];

//int TIMEOUT = 3000;

typedef struct
{
	int x;
	int y;
}position;

void moveVert(int rows, position loc);
void moveHorz(int columns, position loc);
void goHome(position loc);
void emergencyStop(bool & stopRobot, position loc);
bool checkForChip();
int updateBoard (position & loc);
//int accessor(int rows, int columns, int *board);


task main()
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(2000);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(2000);
	position loc;
	loc.x = 10;
	loc.y = 1;
	//populate data arrays with 0
	for (int col = 0; col < MAXCOLUMN; col++)
	{
		columnH[col] = 0;
		for (int row = 0; row < MAXROW; row++)
		{
			board[row][col] = 0;
		}
	}

	int playerMove = updateBoard(loc);
	displayString(5, "chip played in %d", playerMove);
	wait1Msec(5000);
	playerMove = updateBoard(loc);
	displayString(7, "chip played in %d", playerMove);
	wait1Msec(5000);
	playerMove = updateBoard(loc);
	displayString(9, "chip played in %d", playerMove);
	wait1Msec(5000);
	eraseDisplay();
}
//function to move vertical motor
void moveVert(int rows, position & loc) //need to take into account the radius of the gear row 1-6
//Matt
{
	bool stopRobot = true;
	const float GEAR1RAD = 41.06 / 2;//outer wheel diameter
	int encoderInitial = nMotorEncoder[motorA];
	int distance = rows - loc.y;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR1RAD;
	if  (distance > 0)
	{
		motor[motorA] =  -25;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc)&& stopRobot)
		{
		emergencyStop(stopRobot, loc);
		}
		motor[motorA] = 0; //might need to set 1 to stall
	}
	else if (distance < 0)
	{
		motor[motorA] =  25;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, loc);
		}
		motor[motorA] = 0; //might need to set 1 to stall
	}
	else
	{
		motor[motorA] = 0;
	}
	//motor stall if dist = 0?
	loc.y = rows;
}

void moveHorz(int columns, position & loc)
{
	bool stopRobot = true;
	const float GEAR2RAD = 37.1  / 2;
	int encoderInitial = nMotorEncoder[motorB];
	int distance = columns - loc.x;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR2RAD;
	if (distance > 0)
	{
		motor[motorB] = -20;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, loc);
		}
		motor[motorB] = 0;
	}
	else if (distance < 0)
	{
		motor[motorB] = 20;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, loc);
		}
		motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = 0;
	}
	loc.x = columns;
}


//code for emergency stop (include whenever running a while loop to move)
void emergencyStop(bool & stopRobot, position & loc)
{
	if (getButtonPress(buttonDown))
	{
		goHome(loc);
		stopRobot = false;
	}
	else
	{
		stopRobot = true;
	}
}
//function used to reset the positon of the robot
void goHome(position & loc)
{
	motor[motorA] = motor[motorB] = motor[motorC] = 0;
	if (nMotorEncoder[motorA] > 0)
	{
		motor[motorA] = -20;
		while (nMotorEncoder[motorA] > 0)
		{}
		motor[motorA] = 0;
	}
	else
	{
		motor[motorA] = 20;
		while (nMotorEncoder[motorA] < 0)
		{}
		motor[motorA] = 0;
	}
		if (nMotorEncoder[motorB] > 0)
	{
		motor[motorB] = -20;
		while (nMotorEncoder[motorB] > 0)
		{}
		motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = 20;
		while (nMotorEncoder[motorB] < 0)
		{}
		motor[motorB] = 0;
	}
	loc.x = 10;
	loc.y = 1;
}

bool checkForChip()
{
	if (SensorValue[S1] == (int)colorRed)
	{
		return true;
	}
	return false;
}

int updateBoard(position & loc)//Matt
{
	for (int count = MAXCOLUMN; count > 0; count--)
	{
		int columnIndex = count - 1;//account for column height array index
		if (columnH[columnIndex] < 6)
		{
			moveHorz(count, loc);
			wait1Msec(250);
			int row = columnH[columnIndex] + 1;
			moveVert(row, loc);
			wait1Msec(250);
			if(checkForChip())
			{
				board[columnH[columnIndex]][columnIndex] = PLAYER;
				columnH[columnIndex] += 1;//update in later code
				goHome(loc);
				return count;
			}
		}
	}
	goHome(loc);
	return -10;//error code for no player move detected use in do while loop.
}
