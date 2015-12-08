#pragma once
#include "Enums.h"

class Player
{
protected:
	Colours PlayerColour;
	int GamesWon;
public:
	Player();
	Player(Colours pColour);
	~Player();
	void SetPlayerColour(Colours pColour) { PlayerColour = pColour; }
	Colours GetPlayerColour() { return PlayerColour; }
	//string GetName() { return Name; }
	//void SetName(string name) { Name = name; }
	void IncrementGamesWon() { GamesWon++; }
	
};

