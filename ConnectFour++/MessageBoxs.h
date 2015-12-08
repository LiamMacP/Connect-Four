#pragma once
#include <Windows.h>
#include <sstream>
#include "Board.h"

class MessageBoxs
{
public:
	static bool PlayAgainstAI(HWND hwnd);
	static void ShowWinningPlayer(HWND hwnd, const Colours PlayerCurrently);
	static void ShowMessage(HWND hwnd, std::wstringstream stream);
};

