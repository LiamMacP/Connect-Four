#pragma once
#include <Windows.h>
#include "Player.h"
#include "Enums.h"
#include "Constants.h"
#include <stdlib.h>
#include <ctime>

struct WinningMove
{
	WinCondition condition;
	int x, y;
};

class Board
{
	bool GameFinished;
	FinishedGame WinnerOrNot;
	bool NotOver;
	int CurrentTurns;
	Colours PlayerCurrently;
	Colours WinningPlayer; 
	WinningMove winCoords;

public:
	BoardState CurrentBoard[6][7];
	int NextFree[7];
	Board();
	bool PlayTurn(const int& index);
	void AddToColumn(const int& ColumnID);
	void AddToColumn(const int& ColumnID, const Colours& PlayerToAdd);
	bool CheckInputValidity(const int& ColumnID);
	void CopyBoard(const BoardState copyToBoard[6][7], const int nextCopy[7], const int& numberOfTurns);
	void RefreshBoard();
	bool CheckForWinner(const int& xCoord, const int& yCoord, const int& checkAmount = 4);
	void RandomiseStartingPlayer();
	void ChangePlayer();
	Colours ReturnWinningPlayer() const { return WinningPlayer; }
	FinishedGame ReturnWonOrNot() const { return WinnerOrNot; }
	WinningMove ReturnWinningMove() const { return winCoords; }
	int ReturnNumberOfTurns() const { return CurrentTurns; }
	int ReturnNextFree(const int& index) const { return NextFree[index]; }
	BoardState ReturnSelectedValue(const int& i, const int& j) const { return CurrentBoard[i][j]; }
	Colours ReturnCurrentPlayer() const { return PlayerCurrently; }
	void ChangeToPlayer(const Colours& PlayerToAdd) { PlayerCurrently = PlayerToAdd; }
	int GetNextFreeNumber(const int& ColumnID) const { return NextFree[ColumnID]; }
	
private:
	bool CheckForDiagonal(const BoardState& CurrentPosition, const int& i, const int& j, const int& checkAmount);
	bool CheckDown(const BoardState& CurrentPosition, const int& i, const int& j, const int& checkAmount);
	bool CheckSide(const BoardState& CurrentPosition, const int& i, const int& j, const int& checkAmount);
};

