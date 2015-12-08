#include "MessageBoxes.h"

bool MessageBoxs::PlayAgainstAI(HWND hwnd)
{
	int msgboxID = MessageBox(
		hwnd,
		L"Are you wanting to play against the AI?",
		L"Play AI!",
		MB_YESNO | MB_DEFBUTTON1
		);
	if (msgboxID == IDYES)
		return true;
	else
		return false;
}

void MessageBoxs::ShowWinningPlayer(HWND hwnd, const Colours PlayerCurrently)
{
	std::wstringstream strm;
	if (PlayerCurrently == REDColour)
		strm << "The winner of this game is: Player Red.";
	else
		strm << "The winner of this game is: Player Yellow.";
	MessageBox(hwnd, strm.str().c_str(), NULL, NULL);
}

void MessageBoxs::ShowMessage(HWND hwnd, std::wstringstream stream)
{
	MessageBox(hwnd, stream.str().c_str(), NULL, NULL);
}