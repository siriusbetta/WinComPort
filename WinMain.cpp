#include <Windows.h>
#include <tchar.h>

#define ID_SENT_EDIT	1000
#define ID_GET_EDIT		1001
#define ID_SENT_BUTTON	1002
#define ID_GET_BUTTON	1004

HWND hWnd;
HINSTANCE hThisInstance;

LRESULT CALLBACK WindProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hThisInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR cmd,
					   int mode)
{

}

LRESULT CALLBACK WindProc(HWND hWnd, UINT nUint, WPARAM wParam, LPARAM lParam)
{
}
