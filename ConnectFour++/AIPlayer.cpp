#include <ctime>
#include <stdlib.h>     
#include "AIPlayer.h"

//TODO: Look for converting the "BoardState MainBoard[6][7]" to just Board
//		maybe through looking at a function in class "Board" that will return
//		the array of BoardState[6][7]
//TODO: Add the function to add a piece to a board (encapsulation)
//TODO: Base class constructor for parameters
//TODO: Add for each check to see whether the number of turns is == MAX so not infinite loop

AIPlayer::AIPlayer() 
{
	srand(static_cast<unsigned int>(time(NULL)));
	Difficulty = Hard;
	nextColumnToPlay = -1;
}

int AIPlayer::PlayAI(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns)
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
		nextColumnToPlay = RandomiseMove(nextCopy);
	return nextColumnToPlay;
}

int AIPlayer::RandomiseMove(const int nextCopy[7])
{
	if (nextColumnToPlay == -1)
	{
		int ColumnID = rand() % 7;
		while (nextCopy[ColumnID] == 6)
		{
			ColumnID = rand() % 7;
		}
		return ColumnID;
	}
	return -1;
}

int AIPlayer::CanBlock(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns)
{
	for (int i = 0; i < 7; ++i)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (AiBoard.GetNextFreeNumber(i) != 6)
		{
			if (PlayerColour == REDColour)
				AiBoard.AddToColumn(i, YELLOWColour);
			else
				AiBoard.AddToColumn(i, REDColour);
			if (AiBoard.CheckForWinner(i, AiBoard.GetNextFreeNumber(i) -1))
				return i;
		}
	}
	return -1;
}

int AIPlayer::CanWin(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns)
{
	for (int i = 0; i < 7; ++i)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (AiBoard.GetNextFreeNumber(i) != 6)
		{
			if (PlayerColour == REDColour)
				AiBoard.AddToColumn(i, REDColour);
			else
				AiBoard.AddToColumn(i, YELLOWColour);
			if (AiBoard.CheckForWinner(i, AiBoard.GetNextFreeNumber(i) - 1))
				return i;
		}
	}
	return -1;
}

int AIPlayer::MakeThree(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns)
{
	for (int i = 0; i < 7; ++i)
	{
		AiBoard.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (AiBoard.GetNextFreeNumber(i) != 6)
		{
			if (PlayerColour == REDColour)
				AiBoard.AddToColumn(i, REDColour);
			else
				AiBoard.AddToColumn(i, YELLOWColour);
			if (AiBoard.CheckForWinner(i, AiBoard.GetNextFreeNumber(i) - 1, 3))
				return i;
		}
	}
	return -1;
}

int AIPlayer::Win2Moves(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns)
{
	Board tempTable;
	for (int i(0); i < 7; ++i)
	{
		tempTable.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (tempTable.GetNextFreeNumber(i) != 6)
		{
			if (PlayerColour == REDColour)
				tempTable.AddToColumn(i, REDColour);
			else
				tempTable.AddToColumn(i, YELLOWColour);
			for (int i(0); i < 7; ++i)
			{
				SecondAiBoard.CopyBoard(tempTable.CurrentBoard, tempTable.NextFree, tempTable.ReturnNumberOfTurns());
				if  (SecondAiBoard.GetNextFreeNumber(i) != 6)
				{
					if (PlayerColour == REDColour)
						SecondAiBoard.AddToColumn(i, YELLOWColour);
					else
						SecondAiBoard.AddToColumn(i, REDColour);
					int i = CanWin(SecondAiBoard.CurrentBoard, SecondAiBoard.NextFree, SecondAiBoard.ReturnNumberOfTurns());
					if (i != -1)
						return i;
				}
			}
		}
	}
	return -1;
}

int AIPlayer::Block2Moves(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns)
{
	Board tempTable;
	for (int i = 0; i < 7; ++i)
	{
		tempTable.CopyBoard(MainBoard, nextCopy, numberOfTurns);
		if (tempTable.GetNextFreeNumber(i) != 6)
		{
			if (PlayerColour == REDColour)
				tempTable.AddToColumn(i, YELLOWColour);
			else
				tempTable.AddToColumn(i, REDColour);
			for (int i = 0; i < 7; ++i)
			{
				SecondAiBoard.CopyBoard(tempTable.CurrentBoard, tempTable.NextFree, tempTable.ReturnNumberOfTurns());
				if (SecondAiBoard.GetNextFreeNumber(i) != 6)
				{
					if (PlayerColour == REDColour)
						SecondAiBoard.AddToColumn(i, REDColour);
					else
						SecondAiBoard.AddToColumn(i, YELLOWColour);
					int i = CanBlock(SecondAiBoard.CurrentBoard, SecondAiBoard.NextFree, SecondAiBoard.ReturnNumberOfTurns());
					if (i != -1)
						return i;
				}
			}
		}
	}
	return -1;
}

