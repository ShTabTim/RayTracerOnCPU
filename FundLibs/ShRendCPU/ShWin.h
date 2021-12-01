#pragma once
#include <Windows.h>
#include "DrawBuffer.h"
/* For users:
class MainRenderer : public ShWinRendCPU {
	bool initSim() {
		AppName = L"//name//";
		//Inittsialize yuor simulation
		return true;
	}
	bool loopSim(float dt) {
		//Draw whis db.DrawPixel/DrawLine and other
		return true;
	}
	bool finitSim() {
		//Finitsialize yuor simulation
		return true;
	}
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR sCmdLine, int iShow){
	MainRenderer simulation;
	if (simulation.init(hInst, ScrWW, ScrHH, WndProc))
		simulation.Start();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam){
	if(uiMsg == WM_CLOSE)
		PostQuitMessage(0);
	else
        return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	return 0;
}
*/
typedef struct ShWinRendCPU {
private:
	void Rend();
protected:
	DB db;

	HWND hWnd;
	std::wstring AppName;
	HANDLE OriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO OriginalConsoleInfo;
	HANDLE ConsoleHDL;
	HANDLE ConsoleInHDL;
	SMALL_RECT WindowRect;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };

	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bInFocus = true;

	virtual bool initSim() = 0;
	virtual bool loopSim(float dt) = 0;
	virtual bool finitSim() = 0;


	struct sKeyState {
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;
public:
	bool init(HINSTANCE hInst, int W, int H, WNDPROC WndProc);
	void Start();

	int ScreenW();
	int ScreenH();

	void Show(bool Maximized);

	sKeyState GetKey(int nKeyID);

	sKeyState GetMouse(int nMouseButtonID);
	int GetMouseX();
	int GetMouseY();

	bool IsFocused();
}ShWin;