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
	int PlayAI(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
	void ChangeDifficulty(AIDifficulty NewDifficulty) { Difficulty = NewDifficulty; }

	AIPlayer();
	~AIPlayer();
private:
	int RandomiseMove(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
	int CanBlock(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
	int CanWin(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
	int MakeThree(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
	int Win2Moves(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
	int Block2Moves(BoardState MainBoard[6][7], int nextCopy[7], int numberOfTurns);
};

