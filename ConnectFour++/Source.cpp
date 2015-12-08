#include <Windows.h>
#include <crtdbg.h>
#include <ctime>
#include <string>
#include "Board.h"
#include "AIPlayer.h"

using namespace std;

HWND hwnd;
bool showmsg;
HBITMAP ysprite, mask, sprite, background, backbuffer;
HDC hdctmp, hdcback;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnDraw(HWND hwnd);
void OnSize(UINT nShowCmd, int cx, int cy);
void OnKeyDown(UINT nChar, UINT nRpt, UINT nFlags);
void OnMouseMove(UINT nFlags, int cx, int cy);
void OnLButtonDown(UINT nFlags, int cx, int cy);
void CreateGUI(HINSTANCE hInstance);

void DrawBoardAndCounter(HDC hdc);
void ShowWinnerDialog(Colours CurrentPlayer);

int posx, posy;
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

	int width = 400;
	int height = 350;
	int offx = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int offy = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	posx = 0;
	posy = 0;

	mask = (HBITMAP) ::LoadImage(hInstance, L"RedCircleMsk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	sprite = (HBITMAP) ::LoadImage(hInstance, L"RedCircleSpt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	ysprite = (HBITMAP) ::LoadImage(hInstance, L"YellowCircleSpt.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	background = (HBITMAP) ::LoadImage(hInstance, L"Connect4Board.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

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
		playingAI = aiPlayer.IsPlayingAi(hwnd);
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

void DrawBoardAndCounter(HDC hdc)
{
	::SelectObject(hdctmp, background);
	::BitBlt(hdc, 30, 50, 350, 270, hdctmp, 0, 0, SRCCOPY);
	if (board.ReturnCurrentPlayer() == REDColour)
	{
		::SelectObject(hdctmp, mask);
		::BitBlt(hdc, posx, posy, 127, 140, hdctmp, 0, 0, SRCAND);
		::SelectObject(hdctmp, sprite);
		::BitBlt(hdc, posx, posy, 127, 140, hdctmp, 0, 0, SRCINVERT);
	}
	else
	{
		::SelectObject(hdctmp, mask);
		::BitBlt(hdc, posx, posy, 127, 140, hdctmp, 0, 0, SRCAND);
		::SelectObject(hdctmp, ysprite);
		::BitBlt(hdc, posx, posy, 127, 140, hdctmp, 0, 0, SRCINVERT);
	}
}

void DrawCurrentPoints(HDC hdc)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (board.ReturnSelectedValue(i, j) == RED)
			{
				::SelectObject(hdctmp, mask);
				::BitBlt(hdc, 38 + j * 45, 294 - (6 - i) * 40, 127, 140, hdctmp, 0, 0, SRCAND);
				::SelectObject(hdctmp, sprite);
				::BitBlt(hdc, 38 + j * 45, 294 - (6 - i) * 40, 127, 140, hdctmp, 0, 0, SRCINVERT);
			}
			else if (board.ReturnSelectedValue(i, j) == YELLOW)
			{
				::SelectObject(hdctmp, mask);
				::BitBlt(hdc, 38 + j * 45, 294 - (6 - i) * 40, 127, 140, hdctmp, 0, 0, SRCAND);
				::SelectObject(hdctmp, ysprite);
				::BitBlt(hdc, 38 + j * 45, 294 - (6 - i) * 40, 127, 140, hdctmp, 0, 0, SRCINVERT);

			}

		}
	}
}

void TryAI()
{
	player1.SetPlayerColour(YELLOWColour);
	aiPlayer.SetPlayerColour(REDColour);
	board.CurrentBoard[5][2] = RED;
	board.CurrentBoard[5][3] = YELLOW;
	board.CurrentBoard[5][4] = YELLOW;
	board.CurrentBoard[5][5] = YELLOW;
	board.NextFree[5] = 1;
	board.NextFree[4] = 1;
	board.NextFree[3] = 1;
	board.NextFree[2] = 1;
	//board.PlayTurn(aiPlayer.PlayAI(board.CurrentBoard, board.NextFree));
}

void OnDraw(HWND hwnd)
{

	RECT rect;
	::GetClientRect(hwnd, &rect);
	::FillRect(hdcback, &rect, (HBRUSH) ::GetStockObject(WHITE_BRUSH));

	void DrawBoardAndCounter(HDC hdc);
	DrawBoardAndCounter(hdcback);

	void DrawCurrentPoints(HDC hdc);
	DrawCurrentPoints(hdcback);


	HDC hdc = GetDC(hwnd);
	::BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcback, 0, 0, SRCCOPY);
	::ReleaseDC(hwnd, hdc);
}




void OnMouseMove(UINT nFlags, int cx, int cy)
{
	int index = (cx - 30) / 45;
	if (index >= 0 && index < 7)
	{
		posx = 38 + index * 45;
		posy = 10;
	}
	OnDraw(hwnd);
}

void OnLButtonDown(UINT nFlags, int cx, int cy)
{
	//TryAI();
	int index = (cx - 30) / 45;
	if (board.PlayTurn(index))
		board.DisplayWinner(hwnd, board.ReturnCurrentPlayer());
	else
	{
		if (playingAI)
		{
			if (board.PlayTurn(aiPlayer.PlayAI(board.CurrentBoard, board.NextFree, board.ReturnNumberOfTurns())))
				board.DisplayWinner(hwnd, board.ReturnCurrentPlayer());
		}
	}
	OnDraw(hwnd);
	}




		/*	if (PlayerCurrently == AiColour && playingAgainstAI)
		{
			void PlayAIMove();
			PlayAIMove();
		}*/
//int CheckAIMove(BoardState CurrentBoard[6][7]);
//void PlayAIMove()
//{
//	BoardState tempBoard[6][7] = {};
//	void CopyBoard(const BoardState CurrentBoard[6][7], BoardState(&TempBoard)[6][7]);
//	CopyBoard(BoardInputs, tempBoard);
//	int column = CheckAIMove(tempBoard);
//
//	if (column == -1)
//	{
//		column = rand() % 7;
//		while (!(BoardInputs[5 - nextFree[column]][column] == BLANKSPACE))
//		{
//			column = rand() % 7;
//		}
//	}
//
//	if (column >= 0 && column < 7 && BoardInputs[5 - nextFree[column]][column] == BLANKSPACE)
//	{
//		nextFree[column]++;
//		if (AiColour == PlayerRed)
//		{
//			BoardInputs[6 - nextFree[column]][column] = RED;
//			CheckForWinner();
//			PlayerCurrently = PlayerYellow;
//		}
//		else
//		{
//			BoardInputs[6 - nextFree[column]][column] = YELLOW;
//			CheckForWinner();
//			PlayerCurrently = PlayerRed;
//		}
//	}
//}
//



