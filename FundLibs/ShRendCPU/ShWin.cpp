#include "ShWin.h"
#include <chrono>

bool ShWinRendCPU::init(HINSTANCE hInst, int W, int H, WNDPROC WndProc) {
    WNDCLASSEX wcex;

    memset(&wcex, 0, sizeof(WNDCLASSEX));

    db.init(W, H);

    initSim();

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = AppName.c_str() + 1;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0)
        return MessageBox(NULL, L"RegisterClassEx failed!", L"hhhh", MB_ERR_INVALID_CHARS), false;

    if ((hWnd = CreateWindow(
		wcex.lpszClassName,
		AppName.c_str(), 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		W, 
		H, 
		NULL, 
		NULL, 
		hInst, 
		NULL)) == NULL)
        return MessageBox(NULL, L"CreateWindowEx failed!", L"hhhh", MB_ERR_INVALID_CHARS), false;
    return 1;
}

void ShWinRendCPU::Start() {
	Show(false);
	Rend();
}

int ShWinRendCPU::ScreenW() { return db.GetW(); }
int ShWinRendCPU::ScreenH() { return db.GetH(); }


ShWinRendCPU::sKeyState ShWinRendCPU::GetKey(int nKeyID) { return m_keys[nKeyID]; }
ShWinRendCPU::sKeyState ShWinRendCPU::GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
int ShWinRendCPU::GetMouseX() { return m_mousePosX; }
int ShWinRendCPU::GetMouseY() { return m_mousePosY; }
bool ShWinRendCPU::IsFocused() { return m_bInFocus; }

void ShWinRendCPU::Rend() {

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	MSG msg;

	RECT rect;

	GetClientRect(hWnd, &rect);

	HDC dc = GetDC(hWnd);

	while (1) {
		if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> dTime = tp2 - tp1;
			tp1 = tp2;
			float dt = dTime.count();

			for (int i = 0; i < 256; i++) {
				m_keyNewState[i] = GetAsyncKeyState(i);

				m_keys[i].bPressed = false;
				m_keys[i].bReleased = false;

				if (m_keyNewState[i] != m_keyOldState[i])
					if (m_keyNewState[i] & 0x8000) {
						m_keys[i].bPressed = !m_keys[i].bHeld;
						m_keys[i].bHeld = true;
					}
					else {
						m_keys[i].bReleased = true;
						m_keys[i].bHeld = false;
					}

				m_keyOldState[i] = m_keyNewState[i];
			}

			INPUT_RECORD inBuf[32];
			DWORD events = 0;
			GetNumberOfConsoleInputEvents(ConsoleInHDL, &events);
			if (events > 0)
				ReadConsoleInput(ConsoleInHDL, inBuf, events, &events);

			for (DWORD i = 0; i < events; i++)
				switch (inBuf[i].EventType) {
				case FOCUS_EVENT:
					m_bInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
					break;

				case MOUSE_EVENT:
					switch (inBuf[i].Event.MouseEvent.dwEventFlags) {
					case MOUSE_MOVED:
						m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						break;
					case 0:
						for (int m = 0; m < 5; m++)
							m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
						break;
					}
					break;
				}

			for (int m = 0; m < 5; m++) {
				m_mouse[m].bPressed = false;
				m_mouse[m].bReleased = false;

				if (m_mouseNewState[m] != m_mouseOldState[m])
					if (m_mouseNewState[m]) {
						m_mouse[m].bPressed = true;
						m_mouse[m].bHeld = true;
					}
					else {
						m_mouse[m].bReleased = true;
						m_mouse[m].bHeld = false;
					}


				m_mouseOldState[m] = m_mouseNewState[m];
			}


			if (!loopSim(dt))
				break;

			wchar_t s[64];
			swprintf_s(s, 64, L"ConsoleRenderer - %s - FPS: %5.2f", AppName.c_str(), 1.0f / dt);

			SetWindowText(hWnd, s);

			db.OnDrawContextDrawBuffer(&dc);
		}
	}

	if (finitSim())
		db.~DB();
}

void ShWinRendCPU::Show(bool Maximized) {
    RECT dRect, wRect, cRect;

    GetWindowRect(GetDesktopWindow(), &dRect);
    GetWindowRect(hWnd, &wRect);
    GetClientRect(hWnd, &cRect);

    wRect.right += db.GetW() - cRect.right;
    wRect.bottom+= db.GetH() - cRect.bottom;

    wRect.right -= wRect.left;
    wRect.bottom -= wRect.top;

    wRect.left = dRect.right / 2 - wRect.right / 2;
    wRect.top = dRect.bottom / 2 - wRect.bottom / 2;

    MoveWindow(hWnd, wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

    ShowWindow(hWnd, Maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
}