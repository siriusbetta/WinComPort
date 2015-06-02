#include <Windows.h>
#include <tchar.h>

#define ID_SENT_EDIT			1000
#define ID_GET_EDIT				1001
#define ID_SENT_BUTTON			1002
#define ID_GET_BUTTON			1004
#define ID_LIST_PORT			1005
#define ID_PORT_OPEN_BUTTON		1006

HINSTANCE hThisInstance;
TCHAR WinName[] = _T("WinPort");
TCHAR WinHead[] = _T("WinComPort terminal");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HANDLE hSerial; //Serial port
DCB dcb;//Serial port preferences
int SizeBuffer = 1200;
COMMTIMEOUTS CommTimeOuts;

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
	static HWND hSendEdit, hSendButton, hGetEdit, hGetButton, hLabelSend, hLabelGet, hPortList;
	static HWND hPortOpenButton;
	TCHAR hBuff[80];

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

		hSendButton = CreateWindow("button", "SEND",
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						300, 60, 50, 20,
						hWnd,
						(HMENU) ID_SENT_BUTTON,
						hThisInstance,
						NULL
						);

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

		hGetButton = CreateWindow("button", "GET",
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						300, 150, 50, 20,
						hWnd,
						(HMENU) ID_GET_BUTTON,
						hThisInstance,
						NULL
						);

		hPortList = CreateWindow("listbox", NULL, 
						WS_CHILD|WS_VISIBLE|LBS_STANDARD|LBS_WANTKEYBOARDINPUT,
						50, 200, 50, 20,
						hWnd, (HMENU) ID_LIST_PORT,
						hThisInstance,
						NULL);

		hPortOpenButton = CreateWindow("button",
						_T("Connect"),
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						120, 200, 70, 20,
						hWnd,
						(HMENU) ID_PORT_OPEN_BUTTON,
						hThisInstance,
						NULL
						);
		return 0;
		 }

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_SENT_BUTTON:
				{
					GetWindowText(hSendEdit, hBuff, 80);
					MessageBox(hWnd, hBuff, _T("Send sucesfull"), MB_OK);
					return 0;
				}
			case ID_GET_BUTTON:
				MessageBox(hWnd, _T("Get"), _T("Get sucesfull"), MB_OK);
				return 0;
			case ID_PORT_OPEN_BUTTON:
				{
					hSerial=CreateFile("COM6",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

					SetupComm(hSerial, SizeBuffer, SizeBuffer);
					GetCommState(hSerial, &dcb);
					dcb.BaudRate = CBR_9600; 
					dcb.fBinary = TRUE; 
					dcb.fOutxCtsFlow = FALSE; 
					dcb.fOutxDsrFlow = FALSE; 
					dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; 
					dcb.fDsrSensitivity = FALSE; 
					dcb.fNull = FALSE; 
					dcb.fRtsControl = RTS_CONTROL_DISABLE; 
					dcb.fAbortOnError = FALSE; 
					dcb.ByteSize = 8; 
					dcb.Parity = NOPARITY; 
					dcb.StopBits = 1; 
					SetCommState(hSerial, &dcb);
					CommTimeOuts.ReadIntervalTimeout= 10; 
					CommTimeOuts.ReadTotalTimeoutMultiplier = 1; 
					// значений этих тайм – аутов вполне хватает для уверенного приема 
					// даже на скорости 110 бод 
					CommTimeOuts.ReadTotalTimeoutConstant = 100; 
					// используется в данном случае как время ожидания посылки 
					CommTimeOuts.WriteTotalTimeoutMultiplier = 0; 
					CommTimeOuts.WriteTotalTimeoutConstant = 0; 
					SetCommTimeouts(hSerial, &CommTimeOuts);
					PurgeComm(hSerial, PURGE_RXCLEAR); 
					PurgeComm(hSerial, PURGE_TXCLEAR);

					if(hSerial==INVALID_HANDLE_VALUE)
					{
						MessageBox(NULL,"Не возможно открыть последовательный порт","Error",MB_OK);
						ExitProcess(1);
					}
					CHAR data[] = _T("asdf");
					DWORD dwSize = sizeof(data);
					DWORD dwBytesWritten;

					WriteFile (hSerial,data,dwSize,&dwBytesWritten,NULL);
					DWORD iSize;
					TCHAR sReceivedChar[80];
					
					ReadFile(hSerial, &sReceivedChar,80, &iSize, 0);  // получаем 1 байт
            
					MessageBox(NULL,sReceivedChar,"Succesfull",MB_OK);
					return 0;
				}

			}
	


			return 0;
		}
	case WM_DESTROY: 
		{
			CloseHandle(hSerial);
			PostQuitMessage(0);
			break;
		}
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
