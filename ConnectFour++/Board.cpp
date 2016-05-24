#include "Board.h"

Board::Board()
{
	Board::RefreshBoard();
	srand(static_cast<unsigned int>(time(NULL)));
}

bool Board::PlayTurn(const int& index)
{
	if (CheckInputValidity(index))
	{
		AddToColumn(index);
		if (CheckForWinner(index, NextFree[index] - 1))
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

void Board::AddToColumn(const int& ColumnID)
{
		if (PlayerCurrently == REDColour)
			CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = RED;
		else
			CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = YELLOW;
		++NextFree[ColumnID];
}

void Board::AddToColumn(const int& ColumnID, const Colours& PlayerToAdd)
{
	if (PlayerToAdd == REDColour)
		CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = RED;
	else
		CurrentBoard[5 - NextFree[ColumnID]][ColumnID] = YELLOW;
	++NextFree[ColumnID];
}

bool Board::CheckInputValidity(const int& ColumnID)
{
	return (ColumnID >= 0 && ColumnID < 7 && NextFree[ColumnID] < MAXBOARDHEIGHT);
}

void Board::CopyBoard(const BoardState copyToBoard[6][7],const int nextCopy[7], const int& numberOfTurns)
{
	for (int i = 0; i < MAXBOARDWIDTH; ++i)
	{
		for (int j = 0; j < MAXBOARDHEIGHT; ++j)
			CurrentBoard[j][i] = copyToBoard[j][i];
		NextFree[i] = nextCopy[i];
	}
	CurrentTurns = numberOfTurns;
}

void Board::RefreshBoard()
{
	for (int i = 0; i < MAXBOARDWIDTH; ++i)
	{
		for (int j = 0; j < MAXBOARDHEIGHT; ++j)
			CurrentBoard[j][i] = BLANKSPACE;
		NextFree[i] = 0;
	}
	GameFinished = false;
	NotOver = false;
	CurrentTurns = 0;
	RandomiseStartingPlayer();
}

bool Board::CheckForWinner(const int& xCoord, const int& yCoord, const int& checkAmount)
{
	++CurrentTurns;
	if (!(GameFinished || NotOver))
	{
		bool currentFoundDiag(false), currentFoundUp(false), currentFoundSide(false);
		BoardState currentPosition;
		currentPosition = CurrentBoard[5-yCoord][xCoord];
		currentFoundDiag = CheckForDiagonal(currentPosition, 5 - yCoord, xCoord, checkAmount);
		currentFoundSide = CheckSide(currentPosition, 5 - yCoord, xCoord, checkAmount);
		currentFoundUp = CheckDown(currentPosition, 5 - yCoord, xCoord, checkAmount);
		if (currentFoundDiag || currentFoundUp || currentFoundSide)
			return true;		
		if (CurrentTurns == MAXTURNS)
			NotOver = true;
	}
	return false;
}

bool Board::CheckForDiagonal(const BoardState& CurrentPosition, const int& i, const int& j, const int& checkAmount)
{
	int currentFoundDiagonal(1), diagY = i + 1, diagX = j + 1;
	while (diagX <= 6 && diagY <= 5 && CurrentPosition == CurrentBoard[diagY][diagX])
	{
		++currentFoundDiagonal;
		++diagY;
		if (currentFoundDiagonal == checkAmount)
		{
			winCoords.condition = LeftDiagonal;
			winCoords.x = j;
			winCoords.y = i;
			return true;
		}
		++diagX;
	}
	diagY = i - 1; diagX = j - 1;
	while (diagX >= 0 && diagY >= 0 && CurrentPosition == CurrentBoard[diagY][diagX])
	{
		++currentFoundDiagonal;
		--diagY;
		if (currentFoundDiagonal == checkAmount)
		{
			winCoords.condition = LeftDiagonal;
			winCoords.x = diagX;
			winCoords.y = diagY + 1;
			return true;
		}
		--diagX;
	}	
	
	currentFoundDiagonal = 1;
	diagY = i + 1;
	diagX = j - 1;
	while (diagX >= 0 && diagY <= 5 && CurrentPosition == CurrentBoard[diagY][diagX])
	{
		++currentFoundDiagonal;
		++diagY;
		if (currentFoundDiagonal == checkAmount)
		{
			winCoords.condition = RightDiagonal;
			winCoords.x = diagX + 3;
			winCoords.y = diagY - 4;
			return true;
		}
		--diagX;
	}
	diagX = j + 1;
	diagY = i - 1;
	while (diagX <= 6 && diagY >= 0 && CurrentPosition == CurrentBoard[diagY][diagX])
	{
		++currentFoundDiagonal;
		--diagY;
		if (currentFoundDiagonal == checkAmount)
		{
			winCoords.condition = RightDiagonal;
			winCoords.x = diagX;
			winCoords.y = diagY + 1;
			return true;
		}
		++diagX;
	}
	return false;
}

bool Board::CheckDown(const BoardState& CurrentPosition, const int& i, const int& j, const int& checkAmount)
{
	int currentFoundUp(1), yAxis(i + 1);
	while (yAxis <= 5 && CurrentPosition == CurrentBoard[yAxis][j])
	{
		++currentFoundUp;
		if (currentFoundUp == checkAmount)
		{
			winCoords.condition = UpDown;
			winCoords.x = j;
			winCoords.y = i;
			return true;
		}
		++yAxis;
	}
	return false;
}

bool Board::CheckSide(const BoardState& CurrentPosition, const int& i, const int& j, const int& checkAmount)
{
	int currentFoundSide(1), xAxis(j + 1);
	while (xAxis <= 6 && CurrentPosition == CurrentBoard[i][xAxis])
	{
		++currentFoundSide;
		if (currentFoundSide == checkAmount)
		{
			winCoords.condition = LeftRight;
			winCoords.x = j;
			winCoords.y = i;
			return true;
		}
		++xAxis;
	}
	xAxis = j - 1;
	while (xAxis >= 0 && CurrentPosition == CurrentBoard[i][xAxis])
	{
		++currentFoundSide;
		if (currentFoundSide == checkAmount)
		{
			winCoords.condition = LeftRight;
			winCoords.x = xAxis;
			winCoords.y = i;
			return true;
		}
		--xAxis;
	}
	return false;
}

void Board::RandomiseStartingPlayer()
{
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

