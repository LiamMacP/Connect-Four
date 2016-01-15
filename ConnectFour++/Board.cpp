#include <stdlib.h>
#include <ctime>
#include "Board.h"

Board::Board()
{
	Board::RefreshBoard();
}

bool Board::PlayTurn(const int index)
{
	if (CheckInputValidity(index))
	{
		AddToColumn(index);
		if (CheckForWinner())
		{
			WinningPlayer = PlayerCurrently;
			WinnerOrNot = Winner;
			return true;
		}
		else
		{
			if (NotOver)
			{
				WinnerOrNot = NoWinner;
				return true;
			}
			ChangePlayer();
		}
	}
	return false;
}

void Board::AddToColumn(const int ColumnID)
{
		if (PlayerCurrently == REDColour)
			CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = RED;
		else
			CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = YELLOW;
		NextFree[ColumnID]++;
}

void Board::AddToColumn(const int ColumnID, Colours PlayerToAdd)
{
	if (PlayerToAdd == REDColour)
		CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = RED;
	else
		CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = YELLOW;
	NextFree[ColumnID]++;
}

bool Board::CheckInputValidity(const int ColumnID)
{
	if (ColumnID >= 0 && ColumnID < 7 && NextFree[ColumnID] < 6)
		return true;
	else
		return false;
}

void Board::CopyBoard(BoardState copyToBoard[6][7], int nextCopy[7], int numberOfTurns)
{
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 6; j++)
			CurrentBoard[j][i] = copyToBoard[j][i];
		NextFree[i] = nextCopy[i];
	}
	CurrentTurns = numberOfTurns;
}

void Board::RefreshBoard()
{
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 6; j++)
			CurrentBoard[j][i] = BLANKSPACE;
		NextFree[i] = 0;
	}
	GameFinished = false;
	NotOver = false;
	CurrentTurns = 0;
	RandomiseStartingPlayer();
}

bool Board::CheckForWinner(int checkAmount)
{
	CurrentTurns++;
	for (int i = 5; i >= 0; i--)
	{
		for (int j = 6; j >= 0; j--)
		{
			if (!(GameFinished) && !(NotOver))
			{
				bool currentFoundDiag(false), currentFoundUp(false), currentFoundSide(false);
				BoardState currentPosition;

				if (!(CurrentBoard[i][j] == BLANKSPACE))
				{
					currentPosition = CurrentBoard[i][j];

					currentFoundDiag = CheckForDiagonal(currentPosition, i, j, checkAmount);

					currentFoundSide = CheckSide(currentPosition, i, j, checkAmount);

					currentFoundUp = CheckUpDown(currentPosition, i, j, checkAmount);

					if (currentFoundDiag || currentFoundUp || currentFoundSide)
					{
						return true;
					}
				}
			}

		}

		if (CurrentTurns == MAXTURNS)
		{
			NotOver = true;
		}
	}
	return false;
}

bool Board::CheckForDiagonal(const BoardState CurrentPosition, const int i,const  int j, const int checkAmount)
{
	int currentFoundDiagonal(1);
	int diagY = i - 1;
	for (int diagX = j - 1; diagX >= 0; diagX--)
	{

		if (CurrentPosition == CurrentBoard[diagY][diagX])
		{
			currentFoundDiagonal++;
			diagY--;
		}
		else
		{
			diagX = 0;
		}
	}
	if (currentFoundDiagonal == 4)
	{
		return true;
	}

	currentFoundDiagonal = 1;
	diagY = i + 1;
	for (int diagX = j - 1; diagX >= 0; diagX--)
	{

		if (CurrentPosition == CurrentBoard[diagY][diagX])
		{
			currentFoundDiagonal++;
			diagY++;
		
		}
		else
		{
			diagX = 0;
		}
	}
	if (currentFoundDiagonal == 4)
	{
		return true;
	}
	return false;
}

bool Board::CheckUpDown(const BoardState CurrentPosition, const int i, const  int j, const int checkAmount)
{
	int currentFoundUp(1);
	for (int yAxis = i - 1; yAxis >= 0; yAxis--)
	{
		if (CurrentPosition == CurrentBoard[yAxis][j])
		{
				currentFoundUp++;
			/*	if (currentFoundUp == 3 && CurrentBoard[yAxis-1][j] != CurrentPosition)
				yAxis = 0;*/
		
		}
		else
		{
			yAxis = 0;
		}
	}
	if (currentFoundUp == 4)
	{
		return true;
	}
	return false;
}

bool Board::CheckSide(const BoardState CurrentPosition, const int i, const  int j, const int checkAmount)
{
	int currentFoundSide(1);
	for (int xAxis = j - 1; xAxis >= 0; xAxis--)
	{

		if (CurrentPosition == CurrentBoard[i][xAxis])
		{ 
			currentFoundSide++;
			if (currentFoundSide == 3 && CurrentBoard[i][xAxis - 1] != CurrentPosition)
				xAxis = 0;
		}
		else
		{
			xAxis = 0;
		}
	}
	if (currentFoundSide == 4)
	{
		return true;
	}
	return false;
}

void Board::RandomiseStartingPlayer()
{
	srand(static_cast<unsigned int>(time(NULL)));
	if (rand() % 2)
		PlayerCurrently = REDColour;
	else
		PlayerCurrently = YELLOWColour;
}

void Board::ChangePlayer()
{
	if (PlayerCurrently == REDColour)
		PlayerCurrently = YELLOWColour;
	else
		PlayerCurrently = REDColour;
}

