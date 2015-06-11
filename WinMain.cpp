#include <windows.h>
#include <tchar.h>
#include <string>
#include <Strsafe.h>

#define ID_SENT_EDIT			1000
#define ID_GET_EDIT				1001
#define ID_SENT_BUTTON			1002
#define ID_GET_BUTTON			1004
#define ID_LIST_PORT			1005
#define ID_PORT_OPEN_BUTTON		1006

#define ID_LED1_CHECKBOX		1007
#define ID_LED2_CHECKBOX		1008
#define ID_LED3_CHECKBOX		1009
#define ID_ADC_ON_OFF			1010
#define ID_BAUDRATE_600			1011
#define ID_BAUDRATE_1200		1012

#define ID_BAUDRATE_4800		1013
#define ID_BAUDRATE_9600		1014
#define ID_BAUDRATE_14400		1015
#define ID_BAUDRATE_2400		1019

//Размер буфера
#define BUFSIZE					255

//Глобальные переменные
bool ADC_WorkFlag = false;
TCHAR ADC_MessageOff[] = _T("АЦП ВЫКЛ");
TCHAR ADC_MessageOn[]  = _T("АЦП ВКЛ");
TCHAR szMessage[] = _T("");
static HWND hPortList;
char bufrd[BUFSIZE], bufwr[BUFSIZE];
int counter;
char portName[80];
bool bComNotChosen = false;
int uItem = -1;
HANDLE reader;
DWORD signal;
static HWND hADCResultText;

OVERLAPPED overlapped;
OVERLAPPED overlappedWr;

bool bConnectionFlag = false;
TCHAR chConnectionMessage[]		= _T("Connect");
TCHAR chDisconnectionMessage[]	= _T("Disconnect");

int nBaudRate = 2400;

HINSTANCE hThisInstance;
TCHAR WinName[] = _T("WinPort");
TCHAR WinHead[] = _T("WinComPort terminal");
//std::string abs = "";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HANDLE hSerial; //Serial port
DCB dcb;//Serial port preferences
int SizeBuffer = 1200;
COMMTIMEOUTS CommTimeOuts;

int OpenPort();
int ClosePort();
DWORD WINAPI ReadThread(LPVOID);
int ReadPrinting(short a);

VOID Error(CONST HANDLE hStdOut, CONST LPCSTR szMessage) {
  DWORD dwTemp;
  TCHAR szError[256];
  MessageBox(NULL, szMessage, "Error", MB_OK);
  ExitProcess(0);
}


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
						100, 50, 600, 500,
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
	static HWND hADC_Button, hLED1_Button, hLED2_Button, hLED3_Button;
	static HWND hPortOpenButton;
	
	static HWND hBaudRate_600, hBaudRate_1200, hBaudRate_2400, hBaudRate_4800, hBaudRate_9600, hBaudRate_14400;

	

	TCHAR hBuff[80];
	TCHAR str[] = _T("");
	int cnt = 0;

	switch(message)
	{
	case WM_CREATE:
		{
			
			CreateWindow("static", "АЦП (В)", WS_CHILD|WS_VISIBLE|SS_CENTER,
						50,30,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

			hADCResultText = CreateWindow("static", "-", WS_CHILD|WS_VISIBLE|SS_CENTER,
						50,60,200,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

			hADC_Button = CreateWindow("button", ADC_MessageOn,
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						267,60,80,20,
						hWnd,
						(HMENU) ID_ADC_ON_OFF,
						hThisInstance,
						NULL
						);
			
			CreateWindow("static", "LED 1", WS_CHILD|WS_VISIBLE|SS_LEFT,
						50,90,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

			hLED1_Button = CreateWindow("button", "Зажги меня",
						WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
						120,90,110,20,
						hWnd,
						(HMENU) ID_LED1_CHECKBOX,
						hThisInstance,
						NULL
						);

			CreateWindow("static", "LED 2", WS_CHILD|WS_VISIBLE|SS_LEFT,
						50,120,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

			hLED2_Button = CreateWindow("button", "Зажги меня",
						WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
						120,120,110,20,
						hWnd,
						(HMENU) ID_LED2_CHECKBOX,
						hThisInstance,
						NULL
						);

			CreateWindow("static", "LED 3", WS_CHILD|WS_VISIBLE|SS_LEFT,
						50,150,40,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

			hLED3_Button = CreateWindow("button", "Зажги меня",
						WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
						120,150,110,20,
						hWnd,
						(HMENU) ID_LED3_CHECKBOX,
						hThisInstance,
						NULL
						);

			CreateWindow("static", "Введите строку и нажмите Send", WS_CHILD|WS_VISIBLE|SS_CENTER,
						50,240,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);


		hSendEdit = CreateWindow("checkbox", NULL,
						WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
						50,170,300,20,
						hWnd,
						(HMENU) ID_SENT_EDIT,
						hThisInstance,
						NULL);

		hSendButton = CreateWindow("button", "SEND",
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						300, 280, 50, 20,
						hWnd,
						(HMENU) ID_SENT_BUTTON,
						hThisInstance,
						NULL
						);

		CreateWindow("static", "Нажмите Get", WS_CHILD|WS_VISIBLE|SS_CENTER,
						50,320,300,20,
						hWnd,
						(HMENU) 0,
						hThisInstance,
						NULL);

		hGetEdit = CreateWindow("edit", NULL,
						WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
						50, 360, 230, 20,
						hWnd,
						(HMENU) ID_GET_EDIT,
						hThisInstance,
						NULL);

		hGetButton = CreateWindow("button", "GET",
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						300, 360, 50, 20,
						hWnd,
						(HMENU) ID_GET_BUTTON,
						hThisInstance,
						NULL
						);

		hPortList = CreateWindow("listbox", NULL, 
						WS_CHILD|WS_VISIBLE|LBS_STANDARD|LBS_WANTKEYBOARDINPUT,
						50, 400, 80, 50,
						hWnd, (HMENU) ID_LIST_PORT,
						hThisInstance,
						NULL);

		hPortOpenButton = CreateWindow("button",
						chConnectionMessage,
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
						300, 400, 100, 20,
						hWnd,
						(HMENU) ID_PORT_OPEN_BUTTON,
						hThisInstance,
						NULL
						);
		CreateWindow("button", "BaudRate", 
						WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
						150, 380, 100, 60, hWnd,
						(HMENU)0, hThisInstance, NULL
						);
					
			
		hBaudRate_600 = CreateWindow("button", "600",
						WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
						150, 400, 50, 20,
						hWnd,
						(HMENU) ID_BAUDRATE_600,
						hThisInstance,
						NULL
						);

		hBaudRate_1200 = CreateWindow("button", "1200",
						WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
						150, 420, 50, 20,
						hWnd,
						(HMENU) ID_BAUDRATE_1200,
						hThisInstance,
						NULL
						);

		hBaudRate_2400 = CreateWindow("button", "2400",
						WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
						150, 440, 50, 20,
						hWnd,
						(HMENU) ID_BAUDRATE_2400,
						hThisInstance,
						NULL
						);

		hBaudRate_4800 = CreateWindow("button", "4800",
						WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
						210, 400, 50, 20,
						hWnd,
						(HMENU) ID_BAUDRATE_4800,
						hThisInstance,
						NULL
						);

		hBaudRate_9600 = CreateWindow("button", "9600",
						WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
						210, 420, 50, 20,
						hWnd,
						(HMENU) ID_BAUDRATE_9600,
						hThisInstance,
						NULL
						);

		hBaudRate_14400 = CreateWindow("button", "14400",
						WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
						210, 440, 60, 20,
						hWnd,
						(HMENU) ID_BAUDRATE_14400,
						hThisInstance,
						NULL
						);

		switch(nBaudRate)
			{
			case 600:
				{
					SendMessage(hBaudRate_600, BM_SETCHECK, BST_CHECKED,1);
					break;
				}
			case 1200:
				{
					SendMessage(hBaudRate_1200, BM_SETCHECK, BST_CHECKED,1);
					break;
				}
			case 2400:
				{
					SendMessage(hBaudRate_2400, BM_SETCHECK, BST_CHECKED,1);
					break;
				}
			case 4800:
				{
					SendMessage(hBaudRate_4800, BM_SETCHECK, BST_CHECKED,1);
					break;
				}
			case 9600:
				{
					SendMessage(hBaudRate_9600, BM_SETCHECK, BST_CHECKED,1);
					break;
				}
			case 14400:
				{
					SendMessage(hBaudRate_14400, BM_SETCHECK, BST_CHECKED,1);
					break;
				}
			}

		DWORD cbNeeded = 0,cReturned = 0;
		//TCHAR str[] = _T("");
		std::string strName;
		EnumPorts(0,1,0,0,&cbNeeded,&cReturned); // Detection of the necessary memory amount
		if (cbNeeded)
		{
			unsigned char* info = new unsigned char[cbNeeded];

			if(EnumPorts(0, 1, info, cbNeeded, &cbNeeded, &cReturned )) // Reading the information about ports
			{
				PORT_INFO_1* pi = (PORT_INFO_1*)info; // filling the structure
				
				for(int i = 0; i < cReturned; i++)
				{
					strName = pi->pName;
					if(strName.find("COM") != -1)
					{
						strName = strName.substr(0, strName.length()-1);
						SendMessage(hPortList, LB_ADDSTRING, 0, (LPARAM)strName.c_str());
						
					}
					pi++;
				}
				
			}
		
		
		return 0;
		}
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_SENT_BUTTON:
				{
					char cwData[80];
					DWORD dwBytesWritten;
					GetWindowText(hSendEdit, cwData, 10);

					MessageBox(NULL, cwData, "Message", MB_OK);
					DWORD dwSize = sizeof(cwData);
					int cnt = 0;
					do
					{
						std::string nBuffer = "";
						nBuffer += cwData[cnt];
						if((char)cwData[cnt] == 0) break;
						WriteFile (hSerial,nBuffer.c_str(),1,&dwBytesWritten,NULL);
						cnt++;
						nBuffer.clear();
					}while(true);
					return 0;
				}
			case ID_ADC_ON_OFF:
				{
					if(!ADC_WorkFlag)
					{
						SetWindowText((HWND) lParam, ADC_MessageOff);
						ADC_WorkFlag = true;
						reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
						if(!bConnectionFlag)
						{
							MessageBox(NULL, "Порт не открыт! Откройте порт и попробуйте еще раз", "Attention", MB_OK);
						}
						return 0;
					}

					if(ADC_WorkFlag)
					{
						SetWindowText((HWND) lParam, ADC_MessageOn);
						ADC_WorkFlag = false;
						if(reader)
						{
							TerminateThread(reader,0);
							CloseHandle(overlapped.hEvent);	//нужно закрыть объект-событие
							CloseHandle(reader);
							std::auto_ptr<HANDLE> reader(new HANDLE);
						}
						//MessageBox(NULL,"Из АЦП","Error",MB_OK);
						return 0;
					}
			
					return 0;
				}
			case ID_GET_BUTTON:
				{
				
				return 0;
				}
			case ID_PORT_OPEN_BUTTON:
				{
					uItem = (int)SendMessage(
							hPortList,
							LB_GETCURSEL, 0, 0L);

					if(uItem <= -1)
					{
						MessageBox(NULL, "Выберите порт!", "Attention", MB_OK);
						return 0;
					}
					else
					{

					}

					if(!bConnectionFlag)
					{
						SetWindowText((HWND) lParam, chDisconnectionMessage);
						OpenPort();
						bConnectionFlag = true;
						
						return 0;
					}

					if(bConnectionFlag)
					{
						SetWindowText((HWND) lParam, chConnectionMessage);
						ClosePort();
						bConnectionFlag = false;
						return 0;
					}
					
					return 0;
				}
			
			case ID_BAUDRATE_600:
				{
					SendMessage(hBaudRate_600, BM_SETCHECK, BST_CHECKED,1);
					//MessageBox(NULL, "Hello, world!!!", "asd", MB_OK);
					nBaudRate = CBR_600;
					return 0;
				}
			case ID_BAUDRATE_1200:
				{
					SendMessage(hBaudRate_1200, BM_SETCHECK, BST_CHECKED, 1);
					nBaudRate = CBR_1200;
					return 0;
				}
			case ID_BAUDRATE_4800:
				{
					SendMessage(hBaudRate_4800, BM_SETCHECK, BST_CHECKED, 1);
					nBaudRate = CBR_4800;
					return 0;
				}
			case ID_BAUDRATE_9600:
				{
					SendMessage(hBaudRate_9600, BM_SETCHECK, BST_CHECKED, 1);
					nBaudRate = CBR_9600;
					return 0;
				}
			case ID_BAUDRATE_14400:
				{
					SendMessage(hBaudRate_14400, BM_SETCHECK, BST_CHECKED, 1);
					nBaudRate = CBR_14400;
					return 0;
				}
			}
	


			return 0;
		}
	case WM_DESTROY: 
		{
			if(bConnectionFlag)
				ClosePort();

			PostQuitMessage(0);
			break;
		}
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int OpenPort()
{
	
	if(uItem != LB_ERR)
	{
		SendMessage(
					hPortList,
					LB_GETTEXT,
					uItem,
					(LPARAM) portName);
	}
								
		hSerial=CreateFile(
					portName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
					SetupComm(hSerial, SizeBuffer, SizeBuffer);
					GetCommState(hSerial, &dcb);
					dcb.BaudRate = nBaudRate; 
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
						wsprintf(szMessage, TEXT("Не возможно открыть последовательный порт"));
						//MessageBox(NULL,"Не возможно открыть последовательный порт","Error",MB_OK);
						ExitProcess(1);
					}

	return 0;
}

int ClosePort()
{
	
	if(signal != WAIT_OBJECT_0)
	{
		TerminateThread(reader, 0);
		CloseHandle(overlapped.hEvent);
		CloseHandle(reader);
	}
	CloseHandle(hSerial);
	bConnectionFlag = false;
	return 0;
}

DWORD WINAPI ReadThread(LPVOID)
{
	COMSTAT comstat;
	DWORD btr, temp, mask;

	overlapped.hEvent = CreateEvent(NULL, true, true, NULL);

	SetCommMask(hSerial, EV_RXCHAR);
	while(1)
	{
		WaitCommEvent(hSerial, &mask, &overlapped);
		signal = WaitForSingleObject(overlapped.hEvent, INFINITE);
		std::string test = "";
		short b2;
		short b3;
		
		if(signal == WAIT_OBJECT_0)
		{
			if(GetOverlappedResult(hSerial, &overlapped, &temp, true))
				if((mask & EV_RXCHAR) != 0)
				{
					ClearCommError(hSerial, &temp, &comstat);
					btr = comstat.cbInQue;
					if(btr)
					{
						ReadFile(hSerial, bufrd, 2, &temp, &overlapped);
						
						b2 = bufrd[1];
						b2 = b2 << 8;
						b3 = bufrd[0];
						b3 = b3 & 0xff;
						b2 = b2 + b3;
						//MessageBox(NULL, test.c_str(), "asd", MB_OK);
						ReadPrinting(b2);
					}
				}
		}

	}
}

int ReadPrinting(short test)
{
	//std::string buf = (char*)bufrd;
	//int res = 
	//std::string = std::int
	float result = 5 * test / 1023;
	 char *ot = new char[17]; 
	itoa(result, ot, 10);//std::lexical_cast<int>(test);
	 SetWindowText(hADCResultText, ot);
	return 0;
}