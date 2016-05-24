#pragma once
#include "Enums.h"

class Player
{
protected:
	Colours PlayerColour;
public:
	Player() {};
	Player(const Colours& pColour) { PlayerColour = pColour; }
	void SetPlayerColour(const Colours& pColour) { PlayerColour = pColour; }
	Colours GetPlayerColour() const { return PlayerColour; }
};

