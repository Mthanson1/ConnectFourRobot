const float DISTTOCENTER = 34.9;
const float DISTTOCOUNT = 180/PI;
const int MAXCOLUMN = 7;
const int MAXROW = 6;
const int PLAYER = 2;
const int ROBOT = 1;
const int TIMEOUT = 300000;// 5 minutes
int board[MAXROW][MAXCOLUMN];//Nassar said this was ok
int columnH[MAXCOLUMN];


typedef struct
{
	int x;
	int y;
}position;

void initialize(position & loc);
void moveVert(int rows, position loc);
void moveHorz(int columns, position loc);
void goHome(position loc);
void emergencyStop(bool & stopRobot, position loc);
bool checkForChip();
int updateBoard (position & loc);
void closeClaw();
void openClaw();
void robotMove(int robotPlay, position & loc);
int checkWin(int lastPlayerMove, int player);
void playerMove(int & totalTime);
int getRobotMove();

task main()
{
	const float SECONDS = 1000;
	position loc;
	int totalTime = 0, numMoves = 0;
	float aveTime = 0;
	int gameOver = 0;
	int lastMoveP = 0, lastMoveR = 0;
	initialize(loc);
	//test with 3 moves;
	do
	{
		playerMove(totalTime);
		numMoves++;
		lastMoveP = updateBoard(loc);
		gameOver = checkWin(lastMoveP - 1, PLAYER);
		if(!gameOver)
		{
			lastMoveR = getRobotMove();
			if (lastMoveR != -10)
			{
				robotMove(lastMoveR, loc);
				gameOver = checkWin(lastMoveR-1, ROBOT);
			}
			else
			{
				gameOver = -10;
			}
		}
	}while (!gameOver);
	if (gameOver == -10)
	{
		displayString(7, "It was a tie.");
	}
	string winner;
	if(gameOver == ROBOT)
		winner = "The Robot";
	else
		winner = "You";
	aveTime = totalTime / SECONDS / numMoves;
	displayString(7, "%s Won!", winner);
	displayString(9, "You took %.2f s on average", aveTime);
	wait1Msec(10000);
}


void initialize(position & loc)
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);

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
	nMotorEncoder[motorA] = 0;
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	openClaw();
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
		motor[motorB] = -25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && stopRobot)
		{
		emergencyStop(stopRobot, loc);
		}
		motor[motorB] = 0;
	}
	else if (distance < 0)
	{
		motor[motorB] = 25;
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
				//goHome(loc);
				return count;
			}
		}
	}
	eraseDisplay();
	displayString(5, "Error: unable to read move");
	goHome(loc);
	return -10;//error code for no player move detected use in do while loop.
}

void closeClaw()//closes claw, will write once familiar with mechanical claw design //Hayden
{
	motor[motorC]=30;
	while(nMotorEncoder[motorC]<0)
	{}
	motor[motorC]=0;
}

void openClaw()//  opens claw //Hayden
{
	motor[motorC]=-30;
	while(nMotorEncoder[motorC]>-720)
	{}
	motor[motorC]=0;
}

void robotMove(int robotPlay, position & loc)//Luke
{
	int moveIndex = robotPlay + 3;
	//must open claw in main
	//openClaw();
	moveHorz(1, loc);
	wait1Msec(250);
	closeClaw();
	wait1Msec(250);
	moveHorz(moveIndex, loc);
	wait1Msec(250);
	openClaw();
	int index = robotPlay - 1;//account for index staring at 0
	board[columnH[index]][index] = ROBOT;
	columnH[index] += 1;
	wait1Msec(250);
	goHome(loc);
}

int checkWin(int lastPlayerMove, int player)
{
	//lastPlayerMove - 1 for actual player do in main
	int winningSequence = 0;
	//horizontal check
	int checkRow = columnH[lastPlayerMove] - 1;
	for (int column = 0; column < MAXCOLUMN - 3; column++)
	{
			for (int count = 0; count < 4; count++)
			{
				if (board[checkRow][column+count] == player)
					winningSequence++;
				if (winningSequence == 4)
					return player;
			}
			winningSequence = 0;
	}
	//vertical Check
	int checkColumn = lastPlayerMove;
	for (int row = 0; row < MAXROW-3; row++)
	{
		for (int count = 0; count < 4; count++)
		{
			if (board[row+count][checkColumn] == player)
				winningSequence++;
			if (winningSequence == 4)
				return player;
		}
		winningSequence = 0;
	}
	int checkRowUp = checkRow, checkRowDown = checkRow;
	//diagonal check up
	if (lastPlayerMove - 3 >= 0)
		checkColumn = lastPlayerMove - 3;
	else
		checkColumn = 0;
	for (int column = checkColumn; column < MAXCOLUMN - 3; column++)
	{
		if (columnH[lastPlayerMove] - 4 >= 0)
			checkRowUp = columnH[lastPlayerMove] - 4;
		else
			checkRowUp = 0;
		for (int row = checkRowUp; row < MAXROW - 3; row++)
		{
			for (int count = 0; count < 4; count++)
			{
				if (board[checkRowUp+count][checkColumn+count] == player)
					winningSequence++;
				if (winningSequence == 4)
					return player;
			}
			winningSequence = 0;
		}

		//diagonal check down
		if (columnH[lastPlayerMove] + 3 <= 5)
			checkRowDown = columnH[lastPlayerMove] + 2;
		else
			checkRowDown = 3;
		for (int row = checkRowDown; row < MAXROW; row++)
		{
			for (int count = 0; count < 4; count++)
			{
				if (board[checkRowDown-count][checkColumn+count] == player)
					winningSequence++;
				if (winningSequence == 4)
					return player;
			}
			winningSequence = 0;
		}
	}
	return 0;
}

int getRobotMove()
{
	int available = 0;
	for(int checkCol = 0; checkCol < MAXCOLUMN; checkCol++)
	{
		if (columnH[checkCol] < 6)
		{
			available++;
		}
	}
	if (available == 0)
	{
		return -10;
	}
	for(int player = 1; player <= 2; player++)
	{
		for(int testColumn = 0; testColumn < MAXCOLUMN; testColumn++)
		{
			board[columnH[testColumn]][testColumn] = player;
			columnH[testColumn] += 1;
			if(checkWin(testColumn, player))
			{
				columnH[testColumn] -= 1;
				board[columnH[testColumn]][testColumn] = 0;
				return testColumn + 1;
			}
			columnH[testColumn] -=1;
			board[columnH[testColumn]][testColumn] = 0;
		}
	}
	int rmove = -1;
	do
	{
		rmove = random(6);
	}while (columnH[rmove] == 6);
	return rmove + 1;
}

void playerMove(int & totalTime)//Hayden
{
	displayString(5, "It is your turn. Play a chip.");
	displayString(10, "To end turn press enter button");
	time1[T1] = 0;
	while(!getButtonPress(buttonEnter)&&time1[T1]<TIMEOUT)
	{}
	while(getButtonPress(buttonEnter))
	{}
	totalTime += time1[T1];
	eraseDisplay();
}
