#pragma once

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnTimer(UINT nIDEvent);
void OnDraw(HWND hwnd);
void OnSize(UINT nShowCmd, int cx, int cy);
void OnKeyDown(UINT nChar, UINT nRpt, UINT nFlags);
void OnMouseMove(UINT nFlags, int cx, int cy);
void OnLButtonDown(UINT nFlags, int cx, int cy);
void CreateGUI(HINSTANCE hInstance);

void DrawBoardAndCounter(HDC hdc);
void StartAnimation(const int index, RECT rect, int player);

void DrawNewGameInfo(HDC hdc, RECT rect);
void DrawBoardInfo(HDC hdc, RECT rect);
void DrawGameOverInfo(HDC hdc, RECT rect);
