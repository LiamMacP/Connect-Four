#pragma once
#include "Player.h"
#include "Enums.h"
#include "Board.h"

class AIPlayer : public Player
{
	AIDifficulty Difficulty;
	Board AiBoard;
	Board SecondAiBoard;
	int nextColumnToPlay;

public:
	int PlayAI(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns);
	void ChangeDifficulty(const AIDifficulty& NewDifficulty) { Difficulty = NewDifficulty; }
	AIPlayer();

private:
	int RandomiseMove(const int nextCopy[7]);
	int CanBlock(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns);
	int CanWin(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns);
	int MakeThree(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns);
	int Win2Moves(const BoardState MainBoard[6][7], const int nextCopy[7], const int& numberOfTurns);
	int Block2Moves(const BoardState MainBoard[6][7],const int nextCopy[7],const int& numberOfTurns);
};

