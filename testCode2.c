const float DISTTOCENTER = 34.9;
const float DISTTOCOUNT = 180/PI;
int MAXCOLUMN = 7;
int MAXROW = 6;
//int TIMEOUT = 3000;

typedef struct
{
	int x;
	int y;
}point;

void moveVert(int rows, point pos);
void moveHorz(int columns, point pos);
void goHome(point pos);
void emergencyStop(bool & stopRobot, point pos);
bool checkForChip();
int updateBoard (int **board,int *columnHeight, point pos);



task main()
{
	point position;
	position.x = 0;
	position.y = 0;
/*nMotorEncoder[motorB] = 0;
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
goHome();*/
}
//function to move vertical motor
void moveVert(int rows, point pos) //need to take into account the radius of the gear row 1-6
//Matt
{
	bool stopRobot = true;
	const float GEAR1RAD = 41.06 / 2;//outer wheel diameter
	int encoderInitial = nMotorEncoder[motorA];
	int distance = rows - pos.y;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR1RAD;
	if  (distance > 0)
	{
		motor[motorA] =  -25;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc)&& stopRobot)
		{
		emergencyStop(stopRobot, pos);
		}
		motor[motorA] = 0; //might need to set 1 to stall
	}
	else if (distance < 0)
	{
		motor[motorA] =  25;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, pos);
		}
		motor[motorA] = 0; //might need to set 1 to stall
	}
	else
	{
		motor[motorA] = 0;
	}
	//motor stall if dist = 0?
	pos.y = rows;
}

void moveHorz(int columns, point pos)
{
	bool stopRobot = true;
	const float GEAR2RAD = 37.1  / 2;
	int encoderInitial = nMotorEncoder[motorB];
	int distance = columns - pos.x;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR2RAD;
	if (distance > 0)
	{
		motor[motorB] = -25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, pos);
		}
		motor[motorB] = 0;
	}
	else if (distance < 0)
	{
		motor[motorB] = 25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, pos);
		}
		motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = 0;
	}
	pos.x = columns;
}


//code for emergency stop (include whenever running a while loop to move)
void emergencyStop(bool & stopRobot, point pos)
{
	if (getButtonPress(buttonDown))
	{
		goHome(pos);
		stopRobot = false;
	}
	else
	{
		stopRobot = true;
	}
}
//function used to reset the positon of the robot
void goHome(point pos)
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
	pos.x = 0;
	pos.y = 0;
}

bool checkForChip()
{
	if (SensorValue[S1] == (int)colorRed)
	{
		return true;
	}
	return false;
}

int updateBoard(int **board, int *columnHeight, point pos)//Matt
{
for (int count = MAXCOLUMN; count > 0; count--)
{
int columnIndex = count - 1;//account for column height array index
moveHorz(count, pos);
wait1Msec(250);
moveVert(columnHeight[columnIndex], pos);
wait1Msec(250);
if(checkForChip())
{
		columnHeight[columnIndex]++;
		goHome(pos);
		return count;
	}
}
goHome(pos);
return -10;//error code for no player move detected use in do while loop.
}
