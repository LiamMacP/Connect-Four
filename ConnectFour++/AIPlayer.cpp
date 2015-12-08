#include <ctime>
#include <stdlib.h>     
#include "AIPlayer.h"

//TODO: Look for converting the "BoardState MainBoard[6][7]" to just Board
//		maybe through looking at a function in class "Board" that will return
//		the array of BoardState[6][7]


AIPlayer::AIPlayer()
{
	Difficulty = Hard;
	nextColumnToPlay = -1;
}

AIPlayer::~AIPlayer()
{
}

int AIPlayer::PlayAI(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	nextColumnToPlay = -1;
	switch (Difficulty)
	{
	case Easy:
		nextColumnToPlay = CanWin(MainBoard, nextCopy, numberOfTurns);
		if (nextColumnToPlay == -1)
			nextColumnToPlay = CanBlock(MainBoard, nextCopy, numberOfTurns);
		break;
	case Moderate:
		nextColumnToPlay = CanWin(MainBoard, nextCopy, numberOfTurns);
		if (nextColumnToPlay == -1)
		{
			nextColumnToPlay = CanBlock(MainBoard, nextCopy, numberOfTurns);
			if (nextColumnToPlay == -1)
				nextColumnToPlay = Win2Moves(MainBoard, nextCopy, numberOfTurns);
		}
		break;
	case Hard:
		nextColumnToPlay = CanWin(MainBoard, nextCopy, numberOfTurns);
		if (nextColumnToPlay == -1)
		{
			nextColumnToPlay = CanBlock(MainBoard, nextCopy, numberOfTurns);
			if (nextColumnToPlay == -1) 
			{
				nextColumnToPlay = Block2Moves(MainBoard, nextCopy, numberOfTurns);
				if (nextColumnToPlay == -1)
					nextColumnToPlay = Win2Moves(MainBoard, nextCopy, numberOfTurns);

			}
		}
		break;
	}
	if (nextColumnToPlay == -1)
	{
		nextColumnToPlay = RandomiseMove(MainBoard, nextCopy, numberOfTurns);
	}
	return nextColumnToPlay;
}

int AIPlayer::RandomiseMove(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	int ColumnID;
	if (nextColumnToPlay == -1)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		srand(static_cast<unsigned int>(time(NULL)));
		ColumnID = rand() % 7;
		while (AiBoard.GetNextFreeNumber(ColumnID) >= 6)
		{
			ColumnID = rand() % 7;
		}
	}
	return ColumnID;
}

int AIPlayer::CanBlock(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	for (int i = 0; i < 7; i++)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (!(AiBoard.GetNextFreeNumber(i) == 6))
		{
			if (PlayerColour == REDColour)
			{
				AiBoard.AddToColumn(i, YELLOWColour);
			}
			else
			{
				AiBoard.AddToColumn(i, REDColour);

			}
			if (AiBoard.CheckForWinner())
				return i;
		}
	}
	return -1;
}

int AIPlayer::CanWin(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	for (int i = 0; i < 7; i++)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (!(AiBoard.GetNextFreeNumber(i) == 6))
		{
			if (PlayerColour == REDColour)
			{
				AiBoard.AddToColumn(i, REDColour);
			}
			else
			{
				AiBoard.AddToColumn(i, YELLOWColour);

			}
			if (AiBoard.CheckForWinner())
				return i;
		}
	}
	return -1;
}

int AIPlayer::MakeThree(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	for (int i = 0; i < 7; i++)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (!(AiBoard.GetNextFreeNumber(i) == 6))
		{
			if (PlayerColour == REDColour)
			{
				AiBoard.AddToColumn(i, REDColour);
			}
			else
			{
				AiBoard.AddToColumn(i, YELLOWColour);

			}
			if (AiBoard.CheckForWinner(3))
				return i;
		}
	}
	return -1;
}

int AIPlayer::Win2Moves(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	Board tempTable;
	for (int i = 0; i < 7; i++)
	{
		tempTable.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (!(tempTable.GetNextFreeNumber(i) == 6))
		{
			if (PlayerColour == REDColour)
			{
				tempTable.AddToColumn(i, REDColour);
			}
			else
			{
				tempTable.AddToColumn(i, YELLOWColour);

			}
			for (int i = 0; i < 7; i++)
			{
				SecondAiBoard.CopyBoard(tempTable.CurrentBoard, tempTable.NextFree, tempTable.ReturnNumberOfTurns());
				if (!(SecondAiBoard.GetNextFreeNumber(i) == 6))
				{
					if (PlayerColour == REDColour)
					{
						SecondAiBoard.AddToColumn(i, YELLOWColour);
					}
					else
					{
						SecondAiBoard.AddToColumn(i, REDColour);

					}
					int i = CanWin(SecondAiBoard.CurrentBoard, SecondAiBoard.NextFree, SecondAiBoard.ReturnNumberOfTurns());
					if (i != -1)
						return i;
				}
			}
		}
	}
	return -1;
}

int AIPlayer::Block2Moves(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	Board tempTable;
	for (int i = 0; i < 7; i++)
	{
		tempTable.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (!(tempTable.GetNextFreeNumber(i) == 6))
		{
			if (PlayerColour == REDColour)
			{
				tempTable.AddToColumn(i, YELLOWColour);
			}
			else
			{
				tempTable.AddToColumn(i, REDColour);
			}
			for (int i = 0; i < 7; i++)
			{
				SecondAiBoard.CopyBoard(tempTable.CurrentBoard, tempTable.NextFree, tempTable.ReturnNumberOfTurns());
				if (!(SecondAiBoard.GetNextFreeNumber(i) == 6))
				{
					if (PlayerColour == REDColour)
					{
						SecondAiBoard.AddToColumn(i, REDColour);
					}
					else
					{
						SecondAiBoard.AddToColumn(i, YELLOWColour);

					}
					int i = CanBlock(SecondAiBoard.CurrentBoard, SecondAiBoard.NextFree, SecondAiBoard.ReturnNumberOfTurns());
					if (i != -1)
						return i;
				}
			}
		}
	}
	return -1;
}

bool AIPlayer::IsPlayingAi(HWND hwnd)
{
	int msgboxID = MessageBox(
		hwnd,
		L"Are you wanting to play against the AI?",
		L"Play AI!",
		MB_YESNO | MB_DEFBUTTON1
		);
	if (msgboxID == IDYES)
		return true;
	else
		return false;
}
