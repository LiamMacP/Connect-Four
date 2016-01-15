#include <Windows.h>
#include <Windowsx.h> //Allows for macro RGB
#include <crtdbg.h>
#include <ctime>
#include <string>
#include "Board.h"
#include "AIPlayer.h"
#include "Constants.h"
#include "Prototypes.h"

using namespace std;

HWND hwnd;
bool showmsg;
HDC hdctmp, hdcback;
HBITMAP ysprite, mask, sprite, background, backbuffer, backgroundMask;

int posx(0), posy(0), widthOfBoard(75), counterSize(40), counterSizeOffset(36), oldSize(0), currentIndex(0);
bool playingAI(true);

GameState gState = NewGameQuery;
Board board;
Player player1, player2;
AIPlayer aiPlayer;

bool timerOn;
DWORD tCount; int pixelsToTravel, countertop;
double timeToTake;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_onexit(_CrtDumpMemoryLeaks);
#endif

	showmsg = false;
	CreateGUI(hInstance);
	return 0;

}

void CreateGUI(HINSTANCE hInstance)
{
	WNDCLASSEX classname;
	classname.cbSize = sizeof(WNDCLASSEX);
	classname.style = CS_HREDRAW | CS_VREDRAW;
	classname.lpfnWndProc = (WNDPROC)WndProc;
	classname.cbClsExtra = 0;
	classname.cbWndExtra = 0;
	classname.hInstance = hInstance;
	classname.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	classname.hCursor = LoadCursor(NULL, IDC_ARROW);
	classname.hbrBackground = ::GetSysColorBrush(COLOR_3DFACE);
	classname.lpszMenuName = NULL;
	classname.lpszClassName = L"Connect 4++";
	classname.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	RegisterClassEx(&classname);

	int width = 450;
	int height = 504;
	int offx = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
	int offy = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;

	mask = (HBITMAP) ::LoadImage(hInstance, L"MskCounter.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	sprite = (HBITMAP) ::LoadImage(hInstance, L"SpriteRedCounter.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	ysprite = (HBITMAP) ::LoadImage(hInstance, L"SpriteYellowCounter.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	background = (HBITMAP) ::LoadImage(hInstance, L"SpriteBoard.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	backgroundMask = (HBITMAP) ::LoadImage(hInstance, L"MskBoard.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

	HDC hdcsrc = ::GetDC(NULL);
	hdctmp = ::CreateCompatibleDC(hdcsrc);
	hdcback = ::CreateCompatibleDC(hdcsrc);
	backbuffer = NULL;
	::SelectObject(hdctmp, background);
	::ReleaseDC(NULL, hdcsrc);

	CreateWindowEx(NULL, L"Connect 4++", L"Connect 4++", WS_OVERLAPPEDWINDOW | WS_VISIBLE, offx, offy, width, height, NULL, NULL, hInstance, NULL);

	MSG msg;
	do
	{
		if (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		::hwnd = hWnd;
		return 0;
		break;
	case WM_SIZE:
		OnSize((UINT)wParam, (int)(lParam & 0xFFFF), (int)((lParam >> 16) & 0xFFFF));
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		BeginPaint(hWnd, &paint);
		OnDraw(hWnd);
		EndPaint(hWnd, &paint);
	}
	break;
	case WM_TIMER:
		OnTimer(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, (int)(lParam & 0xFFFF), (int)((lParam >> 16) & 0xFFFF));
		break;
	case WM_MOUSEMOVE:
		OnMouseMove((UINT)wParam, (int)(lParam & 0xFFFF), (int)((lParam >> 16) & 0xFFFF));
		break;
	case WM_KEYDOWN:
		if (gState == Playing)
			OnKeyDown((UINT)wParam,1,8);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void OnKeyDown(UINT wParam, UINT nRepCnt, UINT nFlags)
{
		int index(0);
		if ((wParam - '0') >= 1 && (wParam - '0') < 8)
		{
			index = wParam - 49;
			AddFromNumerics(index);
		}
		else if (wParam >= VK_NUMPAD1 && wParam < VK_NUMPAD8)
		{
			index = wParam - 97;
			AddFromNumerics(index);
		}
}

void AddFromNumerics(const int index)
{
	if (!(timerOn))
	{
		RECT rect;
		::GetClientRect(hwnd, &rect);
		posx = (int)(((rect.right - widthOfBoard) >> 1) + index * counterSize + (counterSize *0.1));
		if (board.CheckInputValidity(index))
		{
			StartAnimation(index, rect, HUMAN_PLAYER);
		}
	}
}

void OnSize(UINT nShowCmd, int cx, int cy)
{
	if (cx > 0 && cy > 0)
	{
		if (backbuffer)
			::DeleteObject(backbuffer);

		BITMAPINFO bmpinfo;
		memset(&bmpinfo, 0, sizeof(BITMAPINFO));
		bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		bmpinfo.bmiHeader.biWidth = cx;
		bmpinfo.bmiHeader.biHeight = cy;
		bmpinfo.bmiHeader.biClrImportant = 0;
		bmpinfo.bmiHeader.biClrUsed = 0;
		bmpinfo.bmiHeader.biCompression = BI_RGB;
		bmpinfo.bmiHeader.biXPelsPerMeter = 1;
		bmpinfo.bmiHeader.biYPelsPerMeter = 1;
		bmpinfo.bmiHeader.biBitCount = 32;
		bmpinfo.bmiHeader.biPlanes = 1;
		bmpinfo.bmiHeader.biSizeImage = (cx*cy*bmpinfo.bmiHeader.biBitCount) / 8;

		backbuffer = ::CreateDIBSection(hdcback, &bmpinfo, DIB_RGB_COLORS, NULL, NULL, 0);
		::SelectObject(hdcback, backbuffer);

		::SetStretchBltMode(hdcback, HALFTONE);
		::SetBrushOrgEx(hdcback, 0, 0, NULL);

	}

}

void OnMouseMove(UINT nFlags, int cx, int cy)
{
	if (!(timerOn))
	{
		RECT rect;
		::GetClientRect(hwnd, &rect);
		int index = (cx - ((rect.right - widthOfBoard) >> 1)) / counterSize;
		if (index >= 0 && index < 7)
		{
			posx = (int)(((rect.right - widthOfBoard) >> 1) + index * counterSize + (counterSize *0.1));
			posy = ((rect.bottom - widthOfBoard) >> 1) - 10;
			currentIndex = index;
		}
		OnDraw(hwnd);
	}
}

void OnLButtonDown(UINT nFlags, int cx, int cy)
{
	RECT rect;
	::GetClientRect(hwnd, &rect);
	if (gState == NewGameQuery)
	{
		if (cx >= (rect.right >> 1) * 0.7 && cx <= (rect.right >> 1) * 1.3)
		{
			if (cy >= (rect.bottom >> 1) * 0.9 && cy <= (rect.bottom >> 1) * 1.1)
			{
				gState = Playing;
				playingAI = false;
			} if (cy >= (rect.bottom >> 1) * 1.2 && cy <= (rect.bottom >> 1) * 1.4)
			{
				gState = Playing;
				playingAI = true;
			}
		}
	}
	else if (gState == Playing)
	{

		if (!(timerOn))
		{
			if (cx >= (rect.left + 14) && cx <= (rect.left + 100) && cy >= (rect.top + 20) && cy <= rect.top + 50)
			{
				gState = Playing;
				playingAI = false;
				board.RefreshBoard();
			}
			else {
				int index = (cx - ((rect.right - widthOfBoard) >> 1)) / counterSize;
				if (board.CheckInputValidity(index))
				{
					StartAnimation(index, rect, HUMAN_PLAYER);
				}
			}
		}
	}
	else if (gState == GameOver)
	{
		if (cx >= (rect.right >> 1) * 0.7 && cx <= (rect.right >> 1) * 1.3)
		{
			if (cy >= (rect.bottom >> 1) *1.2 && cy <= (rect.bottom >> 1) * 1.4)
			{
				board.RefreshBoard();
				gState = Playing;
				playingAI = false;
			} if (cy >= (rect.bottom >> 1) * 1.5 && cy <= (rect.bottom >> 1) * 1.7)
			{
				board.RefreshBoard();
				gState = Playing;
				playingAI = true;
			}
		}
	}
	OnDraw(hwnd);
}

void OnDraw(HWND hwnd)
{
	RECT rect;
	::GetClientRect(hwnd, &rect);
	::FillRect(hdcback, &rect, (HBRUSH) ::GetStockObject(WHITE_BRUSH));
	if (gState == NewGameQuery)
	{
		DrawNewGameInfo(hdcback, rect);
	}
	else if (gState == Playing)
	{
		DrawBoardInfo(hdcback, rect);
	}
	else if (gState == GameOver)
	{
		DrawGameOverInfo(hdcback, rect);
	}

	HDC hdc = GetDC(hwnd);
	::BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcback, 0, 0, SRCCOPY);
	::ReleaseDC(hwnd, hdc);
}


void DrawBoardAndCounter(HDC hdc, RECT rect)
{
	if (board.ReturnCurrentPlayer() == REDColour)
	{
		::SelectObject(hdctmp, mask);
		::StretchBlt(hdc, posx, posy, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCAND);
		::SelectObject(hdctmp, sprite);
		::StretchBlt(hdc, posx, posy, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCINVERT);
	}
	else
	{
		::SelectObject(hdctmp, mask);
		::StretchBlt(hdc, posx, posy, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCAND);
		::SelectObject(hdctmp, ysprite);
		::StretchBlt(hdc, posx, posy, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCINVERT);
	}
	::SelectObject(hdctmp, backgroundMask);
	::StretchBlt(hdc, (rect.right - widthOfBoard) >> 1, ((rect.bottom - widthOfBoard) >> 1) + counterSize, widthOfBoard, widthOfBoard, hdctmp, 0, 0, 1050, 1050, SRCAND);
	::SelectObject(hdctmp, background);
	::StretchBlt(hdc, (rect.right - widthOfBoard) >> 1, ((rect.bottom - widthOfBoard) >> 1) + counterSize, widthOfBoard, widthOfBoard, hdctmp, 0, 0, 1050, 1050, SRCINVERT);
}

void DrawCurrentPoints(HDC hdc, RECT rect)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			int xStart = (int)(((rect.right - widthOfBoard) >> 1) + j * counterSize + (counterSize * 0.05 + j / 100));
			if (j > 3)
			{
				xStart = (int)(((rect.right - widthOfBoard) >> 1) + j * counterSize + (counterSize * (j + 3) / 100));
			}
			int leftLocation = (int)(rect.bottom - ((rect.bottom - widthOfBoard) >> 1) - (6 - i) * counterSize + (counterSize * 0.05));
			if (board.ReturnSelectedValue(i, j) == RED)
			{
				::SelectObject(hdctmp, mask);
				::StretchBlt(hdc, xStart, leftLocation, counterSizeOffset,counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCAND);
				::SelectObject(hdctmp, sprite);
				::StretchBlt(hdc, xStart, leftLocation, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCINVERT);
			}
			else if (board.ReturnSelectedValue(i, j) == YELLOW)
			{
				::SelectObject(hdctmp, mask);
				::StretchBlt(hdc, xStart, leftLocation, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCAND);
				::SelectObject(hdctmp, ysprite);
				::StretchBlt(hdc, xStart, leftLocation, counterSizeOffset, counterSizeOffset, hdctmp, 0, 0, 150, 150, SRCINVERT);

			}
		}
	}
}

void DrawNewGameInfo(HDC hdc, RECT rect)
{

	HBRUSH brush = CreateSolidBrush(RGB(210, 210, 210));
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));

	HGDIOBJ old_brush = SelectObject(hdc, brush);
	HGDIOBJ old_pen = SelectObject(hdc, pen);

	RoundRect(hdc, rect.left + 30, rect.top + 30, rect.right - 30, rect.bottom - 30, 10, 10);


	HBITMAP bWelcome = (HBITMAP)::LoadImage(NULL, L"WelcomeScreen.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	::SelectObject(hdctmp, bWelcome);
	::StretchBlt(hdc, rect.left + 40, rect.top + 40, rect.right - 80, (int)(rect.bottom * 0.3), hdctmp, 0, 0, 900, 200, SRCCOPY);

	SetBkColor(hdc, RGB(210, 210, 210));
	const int widthBoundaryLeft((int)((rect.right >> 1) * 0.7)), widthBoundaryRight((int)((rect.right >> 1) * 1.3));
	Rectangle(hdc, widthBoundaryLeft, (int)((rect.bottom >> 1) * 0.9), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.1));
	DrawText(hdc, L"Play User.", -1, new RECT{ widthBoundaryLeft, (int)((rect.bottom >> 1) * 0.9), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.1) },
		DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_CENTER);

	Rectangle(hdc, widthBoundaryLeft, (int)((rect.bottom >> 1) * 1.2), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.4));
	DrawText(hdc, L"Play AI.", -1, new RECT{ widthBoundaryLeft, (int)((rect.bottom >> 1) * 1.2), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.4) },
		DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_CENTER);

	DeleteObject(brush);
	DeleteObject(pen);

}

void DrawBoardInfo(HDC hdc, RECT rect)
{
	if (rect.right < rect.bottom)
		widthOfBoard = (rect.right - 70);
	else
		widthOfBoard = (rect.bottom - 70);
	counterSizeOffset = (int)((widthOfBoard / 7) * 0.9);
	counterSize = (int)(widthOfBoard / 7);
	//For when the application firsts starts, makes the starting counter appear at the start.
	if (posx == 0 || rect.right != oldSize)
	{
		oldSize = rect.right;
		posx = (int)((((rect.right - widthOfBoard) >> 1) + currentIndex * counterSize) + (counterSizeOffset *0.05));
	}
	//	

	DrawCurrentPoints(hdcback, rect);
	DrawBoardAndCounter(hdcback, rect);
	DrawNewGameBox(hdcback, rect);
}

void DrawGameOverInfo(HDC hdc, RECT rect)
{
	int	widthOfBoard = (rect.right - 30);

	HBRUSH brush = CreateSolidBrush(RGB(210, 210, 210));
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));

	HGDIOBJ old_brush = SelectObject(hdc, brush);
	HGDIOBJ old_pen = SelectObject(hdc, pen);

	RoundRect(hdc, rect.left + 30, rect.top + 30, rect.right - 30, rect.bottom - 30, 10, 10);


	HBITMAP bWelcome = (HBITMAP)::LoadImage(NULL, L"GameOver.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	::SelectObject(hdctmp, bWelcome);
	::StretchBlt(hdc, rect.left + 40, rect.top + 40, rect.right - 80, (int)(rect.bottom *0.3), hdctmp, 0, 0, 900, 200, SRCCOPY);

	SetBkColor(hdc, RGB(210, 210, 210));

	const int widthBoundaryLeft((int)((rect.right >> 1) * 0.7)), widthBoundaryRight((int)((rect.right >> 1) * 1.3));

	std::wstring strm;
	if (board.ReturnWonOrNot() == Winner)
	{
		if (board.ReturnWinningPlayer() == REDColour)
			strm = L"The winner of this game is: Player Red.";
		else
			strm = L"The winner of this game is: Player Yellow.";
	}
	else
		strm = L"There was no winner this game.";
	DrawText(hdc, strm.c_str(), -1, new RECT{ widthBoundaryLeft, (int)((rect.bottom >> 1) * 0.9), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.1) },
		DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_CENTER);


	Rectangle(hdc, widthBoundaryLeft, (int)((rect.bottom >> 1) * 1.2), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.4));
	DrawText(hdc, L"Play User.", -1, new RECT{ widthBoundaryLeft, (int)((rect.bottom >> 1) * 1.2), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.4) },
		DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_CENTER);

	Rectangle(hdc, widthBoundaryLeft, (int)((rect.bottom >> 1) * 1.5), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.7));
	DrawText(hdc, L"Play AI.", -1, new RECT{ widthBoundaryLeft, (int)((rect.bottom >> 1) * 1.5), widthBoundaryRight, (int)((rect.bottom >> 1) * 1.7) },
		DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_CENTER);

	DeleteObject(brush);
	DeleteObject(pen);

}

void DrawNewGameBox(HDC hdc, RECT rect)
{

	HBRUSH brush = CreateSolidBrush(RGB(210, 210, 210));
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));

	HGDIOBJ old_brush = SelectObject(hdc, brush);
	HGDIOBJ old_pen = SelectObject(hdc, pen);

	SetBkColor(hdc, RGB(210, 210, 210));

	Rectangle(hdc, rect.left + 14, (rect.top + 20), (rect.left + 100), rect.top + 50);
	DrawText(hdc, L"New Game", -1, new RECT{ rect.left + 14, rect.top + 20, rect.left + 100, rect.top + 50 },
		DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_CENTER);

	DeleteObject(brush);
	DeleteObject(pen);
}


void StartAnimation(const int index, RECT rect, int player)
{
	tCount = GetTickCount();
	countertop = posy;
	pixelsToTravel = (int)((rect.bottom - ((rect.bottom - widthOfBoard) >> 1) - (1 + board.ReturnNextFree(index)) * counterSize) - posy);
	SetTimer(hwnd, player, 50, NULL); //Interval is framerate (approx).
	timerOn = true;

}

void OnTimer(UINT nIDEvent)
{
	const int pixelsPerSecond = 1; //Speed of drop.
	DWORD newCount = GetTickCount();

	DWORD diff = newCount - tCount;
	float t = diff / 200.0f;
	posy = (int)(countertop - (40 * t) + (20 * t*t));

	if (posy > countertop + pixelsToTravel)
	{
		posy = countertop + pixelsToTravel;
		KillTimer(hwnd, nIDEvent);
		timerOn = false;

		RECT rect;
		::GetClientRect(hwnd, &rect);

		int index = (posx - ((rect.right - widthOfBoard) >> 1)) / counterSize;
		posy = countertop;
		if (board.PlayTurn(index))
		{
			gState = GameOver;
		}
		else
		{
			if (playingAI && nIDEvent == HUMAN_PLAYER)
			{
				int aiTurn = aiPlayer.PlayAI(board.CurrentBoard, board.NextFree, board.ReturnNumberOfTurns());

				posx = (int)(((rect.right - widthOfBoard) >> 1) + aiTurn * counterSize + (counterSize * 0.1));
				posy = (int)(((rect.bottom - widthOfBoard) >> 1) - 10);

				StartAnimation(index, rect, AI_PLAYER);
			}
		}
	}
	OnDraw(hwnd);
}



