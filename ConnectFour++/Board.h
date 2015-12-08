#pragma once
#include <Windows.h>
#include "Player.h"
#include "Enums.h"

#define MAXTURNS 42

class Board
{
	bool GameFinished;
	bool NoWinner;
	int CurrentTurns;
	Colours PlayerCurrently;

public:
	BoardState CurrentBoard[6][7];
	int NextFree[7];
	bool PlayTurn(const int index);
	void RandomiseStartingPlayer();
	void ChangePlayer();
	void ChangeToPlayer(Colours PlayerToAdd) {PlayerCurrently = PlayerToAdd; }
	bool CheckForWinner(int checkAmount = 4);
	bool CheckInputValidity(const int ColumnID);
	void AddToColumn(const int ColumnID);
	void AddToColumn(const int ColumnID, Colours PlayerToAdd);
	void CopyBoard(BoardState copy[6][7], int nextCopy[7], int numberOfTurns);
	void RefreshBoard();
	void DisplayWinner(HWND hwnd, const Colours PlayerCurrently);
	int GetNextFreeNumber(const int ColumnID) { return NextFree[ColumnID]; }
	BoardState ReturnSelectedValue(const int i, const int j) { return CurrentBoard[i][j]; }
	Colours ReturnCurrentPlayer() { return PlayerCurrently; }
	int ReturnNumberOfTurns() { return CurrentTurns; }

	Board();
private:
	bool CheckForDiagonal(const BoardState CurrentPosition, const int i, const  int j,const  int checkAmount);
	bool CheckUpDown(const BoardState CurrentPosition, const int i, const  int j, const int checkAmount);
	bool CheckSide(const BoardState CurrentPosition, const int i, const  int j,const  int checkAmount);
};

