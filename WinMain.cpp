#include <Windows.h>
#include <tchar.h>

#define ID_SENT_EDIT	1000
#define ID_GET_EDIT		1001
#define ID_SENT_BUTTON	1002
#define ID_GET_BUTTON	1004


HINSTANCE hThisInstance;
TCHAR WinName[] = _T("WinPort");
TCHAR WinHead[] = _T("WinComPort terminal");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hThisInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR cmd,
					   int mode)
{
	WNDCLASS wc;
	MSG msg;
	HWND hWnd;

	wc.hInstance		= hThisInstance;
	wc.lpszClassName	= WinName;
	wc.lpfnWndProc		= WndProc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.hCursor			= LoadCursor(NULL, IDI_APPLICATION);
	wc.hIcon			= LoadIcon(NULL, IDC_ARROW);
	wc.lpszMenuName		= NULL;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);

	if(!RegisterClass(&wc)) return 0;

	hWnd = CreateWindow(WinName,
						WinHead,
						WS_OVERLAPPEDWINDOW,
						100,
						50,
						500,
						350,
						HWND_DESKTOP,
						NULL,
						hThisInstance,
						NULL
						);

	ShowWindow(hWnd, mode);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hSendEdit, hSendButton, hGetEdit, hGetButton, hLabelSend, hLabelGet;
	switch(message)
	{
	case WM_CREATE:
		{
		CreateWindow("static", "Введите строку и нажмите Send", WS_CHILD|WS_VISIBLE|SS_CENTER,
						50,30,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

		hSendEdit = CreateWindow("edit", NULL,
						WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
						50, 60, 230, 20,
						hWnd,
						(HMENU) ID_SENT_EDIT,
						hThisInstance,
						NULL);
		CreateWindow("static", "Нажмите Get", WS_CHILD|WS_VISIBLE|SS_CENTER,
						50,120,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

		hGetEdit = CreateWindow("edit", NULL,
						WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
						50, 150, 230, 20,
						hWnd,
						(HMENU) ID_GET_EDIT,
						hThisInstance,
						NULL);
		return 0;
		 }
	case WM_DESTROY: PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
