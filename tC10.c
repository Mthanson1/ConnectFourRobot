int getRobotMove()
{
	//check for availible column
	int score[MAXCOLUMN] = {0};
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
		for(int testColumn1 = 0; testColumn1 < MAXCOLUMN; testColumn1++)
		{
			if(columnH[testColumn1] != 6)
			{
				board[columnH[testColumn1]][testColumn1] = player;
				columnH[testColumn1] += 1;
				if(checkWin(testColumn1, player))
				{
					columnH[testColumn1] -= 1;
					board[columnH[testColumn1]][testColumn1] = 0;
					return testColumn1 + 1;
				}
					for(int player = 2; player > 0; player--)
				{
				for(int testColumn2 = 0; testColumn2 < MAXCOLUMN; testColumn2++)
					{
						if(columnH[testColumn2] != 6)
						{
							board[columnH[testColumn2]][testColumn2] = player;
							columnH[testColumn2] += 1;
							if(checkWin(testColumn2, player))
							{
								columnH[testColumn2] -= 1;
								board[columnH[testColumn2]][testColumn2] = 0;
								if (player == ROBOT)
									score[testColumn1] += 5;
								else
									score[testColumn1] -= 5;
							}
							/*else
							{
								columnH[testColumn2] -= 1;
								board[columnH[testColumn2]][testColumn2] = 0;
							}*/
								for(int player = 1; player <= 2; player++)
								{
									for(int testColumn3 = 0; testColumn3 < MAXCOLUMN; testColumn3++)
									{
										if(columnH[testColumn3] != 6)
										{
											board[columnH[testColumn3]][testColumn3] = player;
											columnH[testColumn3] += 1;
											if(checkWin(testColumn3, player))
											{
												columnH[testColumn3] -= 1;
												board[columnH[testColumn3]][testColumn3] = 0;
												if (player == ROBOT)
													score[testColumn1] += 3;
												else
													score[testColumn1] -= 3;
											}
											else
											{
												columnH[testColumn3] -= 1;
												board[columnH[testColumn3]][testColumn3] = 0;
											}
				columnH[testColumn1] -=1;
				board[columnH[testColumn1]][testColumn1] = 0;
			}
		}
	}
	//interpret score
	int maxScore = -1000;
	int maxIndx = 0;
	for(int indx = 0; indx < MAXCOLUMN; indx++)
	{
		if(score[indx] > maxScore)
		{
			maxScore = score[indx];
			maxIndx = indx;
		}
	}
	if (columnH[maxIndx] !=6)
		return maxIndx + 1;
	//generate random move for non-full column:
	int rmove = -1;
	do
	{
		rmove = random(6);
	}while (columnH[rmove] == 6);
	return rmove + 1;
}
