#include <Windows.h>
#include <Windowsx.h> //Allows for macro RGB
#include <crtdbg.h>
#include <ctime>
#include <string>
#include "Board.h"
#include "AIPlayer.h"
#include "MessageBoxs.h"

using namespace std;

HWND hwnd;
bool showmsg;
HBITMAP ysprite, mask, sprite, background, backbuffer, backgroundMask;
HDC hdctmp, hdcback;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnTimer();
void OnDraw(HWND hwnd);
void OnSize(UINT nShowCmd, int cx, int cy);
void OnKeyDown(UINT nChar, UINT nRpt, UINT nFlags);
void OnMouseMove(UINT nFlags, int cx, int cy);
void OnLButtonDown(UINT nFlags, int cx, int cy);
void CreateGUI(HINSTANCE hInstance);

void DrawBoardAndCounter(HDC hdc);
void ShowWinnerDialog(Colours CurrentPlayer);

int posx, posy, widthOfBoard, counterSize(40), counterSizeOffset(36), oldSize(0), currentIndex(0);
bool playingAI(true);
Board board;
Player player1, player2;
AIPlayer aiPlayer;

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
	int height = 400;
	int offx = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int offy = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	posx = 0;
	posy = 0;

	mask = (HBITMAP) ::LoadImage(hInstance, L"RedCircleMsk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	sprite = (HBITMAP) ::LoadImage(hInstance, L"RedCircleSpt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	ysprite = (HBITMAP) ::LoadImage(hInstance, L"YellowCircleSpt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	background = (HBITMAP) ::LoadImage(hInstance, L"ConnectFourBoardSpt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	backgroundMask = (HBITMAP) ::LoadImage(hInstance, L"ConnectFourBoardMsk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

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
		playingAI = MessageBoxs::PlayAgainstAI(hwnd);
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
		OnTimer();
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
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
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

	}

}

void DrawBoardAndCounter(HDC hdc, RECT rect)
{
	if (board.ReturnCurrentPlayer() == REDColour)
	{

		::SelectObject(hdctmp, mask);
		::StretchBlt(hdc, posx, posy, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCAND);
		::SelectObject(hdctmp, sprite);
		::StretchBlt(hdc, posx, posy, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCINVERT);
	}
	else
	{
		::SelectObject(hdctmp, mask);
		::StretchBlt(hdc, posx, posy, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCAND);
		::SelectObject(hdctmp, ysprite);
		::StretchBlt(hdc, posx, posy, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCINVERT);
	}
	::SelectObject(hdctmp, backgroundMask);
	::StretchBlt(hdc, (rect.right - widthOfBoard) / 2, (rect.bottom - widthOfBoard) / 2 + counterSize, widthOfBoard, widthOfBoard, hdctmp, 0, 0, 280, 280, SRCAND);
	::SelectObject(hdctmp, background);
	::StretchBlt(hdc, (rect.right - widthOfBoard) / 2, (rect.bottom - widthOfBoard) / 2 + counterSize, widthOfBoard, widthOfBoard, hdctmp, 0, 0, 280, 280, SRCINVERT);

}

void DrawCurrentPoints(HDC hdc, RECT rect)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (board.ReturnSelectedValue(i, j) == RED)
			{
				::SelectObject(hdctmp, mask);
				::StretchBlt(hdc, (rect.right - widthOfBoard) / 2 + j * counterSize, rect.bottom - ((rect.bottom - widthOfBoard) / 2) - (6 - i) * counterSize, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCAND);
				::SelectObject(hdctmp, sprite);
				::StretchBlt(hdc, (rect.right - widthOfBoard) / 2 + j * counterSize, rect.bottom - ((rect.bottom - widthOfBoard) / 2) - (6 - i) * counterSize, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCINVERT);
			}
			else if (board.ReturnSelectedValue(i, j) == YELLOW)
			{
				::SelectObject(hdctmp, mask);
				::StretchBlt(hdc, (rect.right - widthOfBoard) / 2 + j * counterSize, rect.bottom - ((rect.bottom - widthOfBoard) / 2) - (6 - i) * counterSize, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCAND);
				::SelectObject(hdctmp, ysprite);
				::StretchBlt(hdc, (rect.right - widthOfBoard) / 2 + j * counterSize, rect.bottom - ((rect.bottom - widthOfBoard) / 2) - (6 - i) * counterSize, counterSize, counterSize, hdctmp, 0, 0, 34, 34, SRCINVERT);

			}
		}
	}
}
	bool timerOn;

	void OnDraw(HWND hwnd)
	{

		RECT rect;
		::GetClientRect(hwnd, &rect);
		::FillRect(hdcback, &rect, (HBRUSH) ::GetStockObject(WHITE_BRUSH));
		if (rect.right < rect.bottom)
			widthOfBoard = (rect.right - 60);
		else
			widthOfBoard = (rect.bottom - 60);
		counterSizeOffset = (widthOfBoard / 7) * 0.9;
		counterSize = (widthOfBoard / 7);
		//For when the application firsts starts, makes the starting counter appear at the start.
		if (posx == 0 || rect.right != oldSize)
		{
			oldSize = rect.right;
			posx = ((((rect.right - widthOfBoard) / 2) + currentIndex * counterSize) + (counterSizeOffset *0.05));
		}
		if (!(timerOn))
		{
			void DrawCurrentPoints(HDC hdc, RECT rect);
			DrawCurrentPoints(hdcback, rect);
		}
		void DrawBoardAndCounter(HDC hdc, RECT rect);
		DrawBoardAndCounter(hdcback, rect);

		HDC hdc = GetDC(hwnd);
		::BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcback, 0, 0, SRCCOPY);
		::ReleaseDC(hwnd, hdc);
	}



	void OnMouseMove(UINT nFlags, int cx, int cy)
	{
		if (!(timerOn))
		{
			RECT rect;
			::GetClientRect(hwnd, &rect);
			int index = (cx - ((rect.right - widthOfBoard) / 2)) / counterSize;
			if (index >= 0 && index < 7)
			{
				posx = (rect.right - widthOfBoard) / 2 + index * counterSize;
				posy = (rect.bottom - widthOfBoard) / 2 - 10;
				currentIndex = index;
			}
			OnDraw(hwnd);
		}
	}

	void OnLButtonDown(UINT nFlags, int cx, int cy)
	{
		RECT rect;
		::GetClientRect(hwnd, &rect);
		int index = (cx - ((rect.right - widthOfBoard) / 2)) / counterSize;
		if (board.CheckInputValidity(index))
		{
			void StartAnimcation(const int index, RECT rect);
			StartAnimcation(index, rect);
		}
		if (board.PlayTurn(index))
			MessageBoxs::ShowWinningPlayer(hwnd, board.ReturnCurrentPlayer());
		else
		{
			if (playingAI)
			{

				if (board.PlayTurn(aiPlayer.PlayAI(board.CurrentBoard, board.NextFree, board.ReturnNumberOfTurns())))
					MessageBoxs::ShowWinningPlayer(hwnd, board.ReturnCurrentPlayer());
			}
		}
		OnDraw(hwnd);
	}
	DWORD tCount; int pixelsToTravel;
	double timeToTake;

	void StartAnimcation(const int index, RECT rect)
	{
		const int pixelsPerSecond = 72;
		const int interval = 1000 / pixelsPerSecond;
		tCount = GetTickCount();
		pixelsToTravel = (rect.bottom - ((rect.bottom - widthOfBoard) / 2) - (0 + board.ReturnNextFree(index)) * counterSize) - posy;
		timeToTake = (pixelsToTravel / (double)pixelsPerSecond);
		SetTimer(hwnd, 1, interval, NULL); //Doesn't tick correct when pixels per second is 100, otherwise the counter would be correct and the default position move -300px.
		timerOn = true;

	}

	void OnTimer()
	{
		DWORD newCount = GetTickCount();
		posy += 1;
		if (newCount - tCount >= (timeToTake * 1000))
		{
			KillTimer(hwnd, 1);
			timerOn = false;
		}
		OnDraw(hwnd);

	}



