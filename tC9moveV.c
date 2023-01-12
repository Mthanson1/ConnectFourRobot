const float DISTTOCENTER = 34.9;
//Center to center dist of holes
const float DISTTOCOUNT = 180/PI;
//conversion factor for distance to degree
const int MAXCOLUMN = 7;
const int MAXROW = 6;
const int PLAYER = 2;
const int ROBOT = 1;
const int TIMEOUT = 300000;//5 minutes
//Nassar said this was ok
int board[MAXROW][MAXCOLUMN];
int columnH[MAXCOLUMN];
bool eStop = false;//bool that causes complete shutdown

typedef struct
{
//type that keeps track of the robot's current position allowing absolute movements
	int x;
	int y;
}position;

void initialize(position & loc);//Matt & Luke
void moveVert(int rows, position loc);//Matt
void moveHorz(int columns, position loc);//Matt
void goHome(position loc);//Magnus
bool emergencyStop(position loc);//Magnus
bool checkForChip();//Magnus
int updateBoard (position & loc);//Matt
void closeClaw();//Hayden
void openClaw();//Hayden
void robotMove(int robotPlay, position & loc);//Luke
int checkWin(int lastPlayerMove, int player);//Matt
void playerMove(int & totalTime, position & loc);//Hayden
int getRobotMove();//Luke

task main()
{
	const float SECONDS = 1000;
	position loc;
	int totalTime = 0, numMoves = 0;
	float aveTime = 0;
	int gameOver = 0;
	int lastMoveP = 0, lastMoveR = 0;
	initialize(loc);
	do
	{
		playerMove(totalTime, loc);
		numMoves++;
		lastMoveP = updateBoard(loc);//store player's last move
		if (lastMoveP != -10)//if a chip is read
			gameOver = checkWin(lastMoveP-1, PLAYER);
		else//if no player chip found
		{
			displayString(7, "Error: No Player Move Detected");
			eStop = true;//stops gameplayloop
		}
		if (eStop)//check if emergency stop initiated
			gameOver = -1;//error code to break gameplay loop
		if(!gameOver)//if no player win or error detected
		{
			lastMoveR = getRobotMove();//calc robots next move
			if (lastMoveR != -10)//if a move was availible
			{
				robotMove(lastMoveR, loc);
				gameOver = checkWin(lastMoveR-1, ROBOT);//check if robot won
				if (eStop)//check if eStop initiated
					gameOver = -1;
			}
			else
			{
				gameOver = -10;//if no move availible return "tie code"
			}
		}
	}while (!gameOver);//if no robot win detected continue to loop
	goHome(loc);
	closeClaw();
	if (gameOver == -10)//if "tie code" returned
	{
		displayString(7, "It was a tie.");
	}
	string winner;
	if(gameOver == ROBOT)//if robot won
		winner = "The Robot";
	else//if player won
		winner = "You";
	aveTime = totalTime / SECONDS / numMoves;
	if (gameOver != -1)//if game wasn't stopped with eStop
	{
		displayString(7, "%s Won!", winner);
		displayString(9, "You took %.2f s on average", aveTime);
	}
	wait1Msec(10000);//allow time for player to read output
}


void initialize(position & loc)//setup sensors, encoders, arrays and intializes position
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);
	loc.x = 10;//claw is aligned with column 7
	loc.y = 1;//color sensor is aligned with bottom row
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
	//Game start procedure
	time1[T2] = 0;
	displayString(5, "Welcome to Connect 4!");
	displayString(7, "Press Enter to Start");
	while (!getButtonPress(buttonEnter) && time1[T2] < TIMEOUT)
	{}
	while (getButtonPress(buttonEnter))
	{}
	eraseDisplay();
	displayString(15, "Press Down to Quit");
}

//function to move vertical motor to required row
void moveVert(int rows, position & loc)
{
	const float GEAR1RAD = 41.06 / 2;//outer wheel radius of vertical gear
	int encoderInitial = nMotorEncoder[motorA];
	int distance = rows - loc.y;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR1RAD;
	if (rows == 6)
		stopEnc -= 2;
	if  (distance > 0)
	{
		motor[motorA] =  -20;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc) && !eStop && emergencyStop(loc))
		{}
		motor[motorA] = 0;
	}
	else if (distance < 0)
	{
		motor[motorA] =  20;
		while(abs(nMotorEncoder[motorA] - encoderInitial) <  abs(stopEnc) && !eStop && emergencyStop(loc))
		{}
		motor[motorA] = 0;
	}
	else
	{
		motor[motorA] = 0;
	}
	if (rows == 6)
		moveVert(1, loc);
	loc.y = rows;//update current location
}

//function to move the robot horizontally to given column(based on color sensor)
void moveHorz(int columns, position & loc)
{
	const float GEAR2RAD = 37.1  / 2;//outer wheel radius of horizontal gear
	int encoderInitial = nMotorEncoder[motorB];
	int distance = columns - loc.x;
	int stopEnc = distance * DISTTOCENTER * DISTTOCOUNT / GEAR2RAD;
	if (distance > 0)
	{
		motor[motorB] = -25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && !eStop && emergencyStop(loc))
		{}
		motor[motorB] = 0;
	}
	else if (distance < 0)
	{
		motor[motorB] = 25;
		while (abs(nMotorEncoder[motorB] - encoderInitial) < abs(stopEnc) && !eStop && emergencyStop(loc))
		{}
		motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = 0;
	}
	loc.x = columns;//update the horizontal position
}


//code for emergency stop (include whenever running a while loop to move)
//will completely stop all movement, return to home pos and display error
bool emergencyStop(position & loc)
{
	if (getButtonPress(buttonDown))
	{
		goHome(loc);
		closeClaw();
		eStop = true;
		displayString(7, "Error: Game Terminated");
		displayString(10, "Self Destruct in 3...2...1...");
		return false;
	}
	else
	{
		return true;
	}
}

//function used to reset the positon of the robot
//drive movement motors to 0
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

//check if player chip is present
bool checkForChip()
{
	if (SensorValue[S1] == (int)colorRed)
	{
		return true;
	}
	return false;
}

//robot scans possible move locations that the player may have made
int updateBoard(position & loc)
{
	for (int count = MAXCOLUMN; count > 0; count--)
	{
		int columnIndex = count - 1;
		//account for column height array index starting at 0
		if (columnH[columnIndex] < 6)
		{
			moveHorz(count, loc);
			wait1Msec(250);
			int row = columnH[columnIndex] + 1;
			moveVert(row, loc);
			wait1Msec(250);
			if(checkForChip())
			{
				//update board data arrays:
				board[columnH[columnIndex]][columnIndex] = PLAYER;
				columnH[columnIndex] += 1;
				return count;//column played
			}
		}
	}
	if(!eStop)
	{
	eraseDisplay();
	displayString(5, "Error: unable to read move");
	displayString(7, "press enter to read again");
	displayString(10, "press down to quit");
	goHome(loc);
	while(!getButtonPress(buttonEnter) && emergencyStop(loc))
	{}
	while(getButtonPress(buttonEnter))
	{}
	updateBoard(loc);
	}
	return -10;//error code for no player move detected use in gameplay loop.
}

void closeClaw()
{
	if(!eStop)
	{
		motor[motorC]=30;
		while(nMotorEncoder[motorC]<0)
		{}
		motor[motorC]=0;
	}
}

void openClaw()
{
	if(!eStop)
	{
		motor[motorC]=-30;
		while(nMotorEncoder[motorC]>-720)
		{}
		motor[motorC]=0;
	}
}

//plays robot move based on sent column
void robotMove(int robotPlay, position & loc)
{
	int moveIndex = robotPlay + 3;
	//account for color sensor-claw offset
	moveHorz(1, loc);
	wait1Msec(250);
	closeClaw();
	wait1Msec(250);
	moveHorz(moveIndex, loc);
	wait1Msec(250);
	openClaw();
	int index = robotPlay - 1;//account for index staring at 0
	//update board arrays:
	board[columnH[index]][index] = ROBOT;
	columnH[index] += 1;
	wait1Msec(250);
	goHome(loc);
}

//checks for win based on which player and last column
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


//simulates 7 possible moves first for robot then player checking for a win after each move
//if a win is detected the robot plays that column to either win or block a win
//prioritizes winning over blocking. else returns random column
int getRobotMove()
{
	//check for availible column
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
	//generate random move for non-full column:
	int rmove = -1;
	do
	{
		rmove = random(6);
	}while (columnH[rmove] == 6);
	return rmove + 1;
}

//code to wait and accumulate time for player
void playerMove(int & totalTime, position & loc)
{
	displayString(5, "It is your turn. Play a chip.");
	displayString(10, "To end turn press enter button");
	time1[T1] = 0;
	while(!getButtonPress(buttonEnter)&&time1[T1]<TIMEOUT && emergencyStop(loc))
	{}
	while(getButtonPress(buttonEnter) && !eStop)
	{}
	totalTime += time1[T1];
	eraseDisplay();
	displayString(15, "Press Down to Quit");
}
