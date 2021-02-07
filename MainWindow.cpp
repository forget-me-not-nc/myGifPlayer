#pragma warning(disable : 4996)

#define STOPBUTTON_ID 664
#define PLAYBUTTON_ID 665
#define LOADBUTTON_ID 666
#define TRACKBAR_ID	667
#define VOLUMETRACKBAR_ID 668
#define RICHEDIT_ID 669
#define WRITESPACE_ID 670
#define ST_TRACKBARTXT 998
#define ST_MAINTXT	999
#define SKIPSONG_ID	111
#define LOADSONG_ID 112
#define STOPSONG_ID 113
#define RESUMESONG_ID 114
//--------------------------

#include "SecondaryMethods.h"
#include <stdlib.h>
//--------------------------

BOOL initFlag = FALSE;

//--------------------------

//--window message handler--
LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateTrackbar(HWND hwndParent);
HWND CreateRichedit(HWND hwndParent);
HWND CreateVolumeTrackbar(HWND hwndParent);
VOID SetRicheditTxt(HWND hwndParent);
VOID InitVars(HWND hWnd);

//--------------------------

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	srand(time(NULL));
	//--init startup struct--
	MSG message{};
	HWND hWnd;
	WNDCLASSEX wndClass{ sizeof(WNDCLASSEX) };

	//--init current dir--
	GetCurrentDirectory(MAX_PATH, currentDirectory);

	//--init gdi+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//--fill struct--
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WinProc;
	wndClass.lpszClassName = L"MainWindow";
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	//--register window class--
	if (!RegisterClassEx(&wndClass))
	{
		return EXIT_FAILURE;
	}

	//--create window--
	if (hWnd = CreateWindowEx(NULL, wndClass.lpszClassName, L"Relax", WS_OVERLAPPEDWINDOW | WS_EX_WINDOWEDGE,
		100, 100, 1000, 700, NULL, NULL, wndClass.hInstance, NULL); hWnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	//--show window--
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//--message loop--
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	free(propItem);
	delete gif;

	//--cleaning up environment--
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return static_cast<int>(message.wParam);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps{};

	switch (uMsg)
	{
		//--painting events case--
		case WM_PAINT:
		{
			if (!initFlag)
			{
				InitVars(hWnd);

				initFlag = TRUE;
			}

			_hdc = BeginPaint(hWnd, &ps);

			DrawBackground();
			DrawFramesBorders();

			EndPaint(hWnd, &ps);

			break;
		}

		case WM_CREATE:
		{
			GetClientRect(hWnd, &_windowRect);

			//--buttons 'frame'--
			HWND hFrame1 = CreateWindowEx(WS_EX_DLGMODALFRAME, L"STATIC", NULL, WS_CHILD | WS_VISIBLE, 
				0, 0, 150, 700, hWnd, NULL, _hInstance, (LPVOID)NULL);

			//--music controls 'frame'--
			HWND hFrame2 = CreateWindowEx(WS_EX_DLGMODALFRAME, L"STATIC", NULL, WS_CHILD | WS_VISIBLE,
				0, 700, 1000, 130, hWnd, NULL, _hInstance, (LPVOID)NULL);

			//--button to start play gif--
			HWND hPlayButton = CreateWindow(L"BUTTON", L"Play Gif", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 20, _windowRect.bottom - 650, 100, 25, hWnd, (HMENU)PLAYBUTTON_ID, _hInstance, NULL);

			//--button to stop gif--
			HWND hStopButton = CreateWindow(L"BUTTON", L"Stop Gif", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 20, _windowRect.bottom - 600, 100, 25, hWnd, (HMENU)STOPBUTTON_ID, _hInstance, NULL);

			//--button to load gif--
			HWND hLoadGifButton = CreateWindow(L"BUTTON", L"Load Gif", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 20, _windowRect.bottom - 550, 100, 25, hWnd, (HMENU)LOADBUTTON_ID, _hInstance, NULL);
			
			HWND hSkipSongButton = CreateWindow(L"BUTTON", L"Skip Song", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 20, _windowRect.bottom - 80, 100, 25, hWnd, (HMENU)SKIPSONG_ID, _hInstance, NULL);

			HWND hLoadSongButton = CreateWindow(L"BUTTON", L"Load Song", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 140, _windowRect.bottom - 80, 100, 25, hWnd, (HMENU)LOADSONG_ID, _hInstance, NULL);
			
			HWND hStopSongButton = CreateWindow(L"BUTTON", L"Stop Song", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 260, _windowRect.bottom - 80, 100, 25, hWnd, (HMENU)STOPSONG_ID, _hInstance, NULL);

			HWND hResumeSongButton = CreateWindow(L"BUTTON", L"Resume Song", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				_windowRect.left + 380, _windowRect.bottom - 80, 100, 25, hWnd, (HMENU)RESUMESONG_ID, _hInstance, NULL);

			//--trackbar--
			HWND hTrackbar = CreateTrackbar(hWnd);
			
			HWND hVolumeTrackbar = CreateVolumeTrackbar(hWnd);

			//--txt line--
			HWND hRichedit = CreateRichedit(hWnd);

			paintingThreadHandle = (HANDLE)_beginthreadex(NULL, 0, _glitch_painting_thread, NULL, CREATE_SUSPENDED, NULL);

			if (paintingThreadHandle) ResumeThread(paintingThreadHandle);

			musicThreadHandle = (HANDLE)_beginthreadex(NULL, 0, _bckgrnd_music_thread, NULL, CREATE_SUSPENDED, NULL);

			if (musicThreadHandle) ResumeThread(musicThreadHandle);
			
			break;
		}

		//--case to handle button events--
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case PLAYBUTTON_ID:
				{
					if (gif) Start();
					else LastError(L"Load Gif first.");

					break;
				}

				case LOADBUTTON_ID:
				{
					if (!LoadFromFileExplorer()) LastError(L"Load selected gif");

					break;
				}

				case STOPBUTTON_ID:
				{
					KillTimer(hWnd, TIMER_ID);

					break;
				}
				
				case SKIPSONG_ID:
				{
					songState = SONG_STATES::SKIP;

					break;
				}

				case LOADSONG_ID:
				{
					songState = SONG_STATES::LOAD;

					break;
				}

				case STOPSONG_ID:
				{
					songState = SONG_STATES::PAUSE;

					break;
				}

				case RESUMESONG_ID:
				{
					songState = SONG_STATES::PLAY;

					break;
				}
				
				default: break;
			}

			break;
		}

		//--timer case--
		case WM_TIMER:
		{
			switch (LOWORD(wParam))
			{
				case TIMER_ID:
				{
					if(gif != NULL) DisplayOnTimer();

					break;
				}

				default: break;
			}

			break;
		}	

		//--resize case--
		case WM_GETMINMAXINFO:
		{
			//--set to no-resize--
			LPMINMAXINFO min_max = (LPMINMAXINFO)lParam;
			min_max->ptMinTrackSize.x = 1000;
			min_max->ptMinTrackSize.y = 830;

			min_max->ptMaxTrackSize.x = 1000;
			min_max->ptMaxTrackSize.y = 830;

			break;
		}

		//--destroy window case
		case WM_DESTROY:
		{
			//--kill timer on WindowDestroy--
			KillTimer(hWnd, WM_TIMER);

			//--close threads--
			InterlockedExchangeAdd(&threadExitFlag, 1);

			DWORD paintingThreadHandleExitCode = 0;
			DWORD musicThreadHandleExitCode = 0;

			while (paintingThreadHandleExitCode == STILL_ACTIVE && musicThreadHandleExitCode == STILL_ACTIVE)
			{
				GetExitCodeThread(paintingThreadHandle, &paintingThreadHandleExitCode);
				GetExitCodeThread(musicThreadHandle, &musicThreadHandleExitCode);
			}

			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);

			break;
		}

		case WM_ERASEBKGND: return TRUE;

		//--trackbar notify-catcher case--
		case WM_HSCROLL:
		{
			if ((HANDLE)lParam == GetDlgItem(hWnd, TRACKBAR_ID))
			{
				KillTimer(hWnd, TIMER_ID);

				timerDelay = (DWORD)SendMessage(GetDlgItem(hWnd, TRACKBAR_ID), TBM_GETPOS, 0, 0);

				SetTimer(hWnd, TIMER_ID, timerDelay, NULL);

				SetRicheditTxt(GetDlgItem(hWnd, RICHEDIT_ID));
			}
			else if ((HANDLE)lParam == GetDlgItem(hWnd, VOLUMETRACKBAR_ID))
			{
				volume = (DWORD)SendMessage(GetDlgItem(hWnd, VOLUMETRACKBAR_ID), TBM_GETPOS, 0, 0);

				waveOutSetVolume(NULL, MAKELONG(volume, volume));
			}

			break;
		}

		default: break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////

HWND CreateTrackbar(HWND hwndParent)
{
	InitCommonControls();

	GetClientRect(hwndParent, &_windowRect);

	CreateWindow(L"STATIC", L"<<Change Speed>>", WS_CHILD | WS_VISIBLE, _windowRect.left + 10, _windowRect.bottom - 488, 135, 20, hwndParent, NULL, _hInstance, NULL);

	HWND hTrackbar = CreateWindowEx(NULL, TRACKBAR_CLASS , L"Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
		_windowRect.left + 10, _windowRect.bottom - 470, 120, 25, hwndParent, (HMENU)TRACKBAR_ID, _hInstance, NULL);

	if (!hTrackbar)
	{
		LastError(L"Create Trackbar");

		return NULL;
	}

	SendMessage(hTrackbar, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(20, 2000));
	SendMessage(hTrackbar, TBM_SETPAGESIZE, 0, (LPARAM)10);
	SendMessage(hTrackbar, TBM_SETSEL, FALSE,(LPARAM)MAKELONG(20, 2000));
	SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)timerDelay);

	return hTrackbar;
}

///////////////////////////////////

HWND CreateRichedit(HWND hwndParent)
{
	LoadLibrary(L"riched20.dll");

	GetClientRect(hwndParent, &_windowRect);

	HWND hRichedit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NOHIDESEL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
		_windowRect.left + 10, _windowRect.bottom - 420, 120, 40, hwndParent, reinterpret_cast<HMENU>(RICHEDIT_ID), _hInstance, NULL);

	EnableWindow(hRichedit, FALSE);

	SetRicheditTxt(hRichedit);

	return hRichedit;
}

///////////////////////////////////

VOID SetRicheditTxt(HWND hRichedit)
{
	wstring text = L"Current display speed: ";

	text += std::to_wstring(timerDelay);
	text += L".ms";

	SetWindowText(hRichedit, text.c_str());
}

///////////////////////////////////

VOID InitVars(HWND hWnd)
{
	_hWnd = hWnd;
	_hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));

	//--init gif rect--
	_gifRect.X = 80 + (_windowRect.right) / 2 - 450 / 2;
	_gifRect.Y = _windowRect.bottom / 2 - 300 / 2 - 50;
	_gifRect.Height = 300;
	_gifRect.Width = 450;
	
	//--init borders rects--
	_highBorderRect.X = 80 + (_windowRect.right) / 2 - 450 / 2 - 5;
	_highBorderRect.Y = _windowRect.bottom / 2 - 300 / 2 - 50 - 5;
	_highBorderRect.Height = 310;
	_highBorderRect.Width = 460;

	_lowBorderRect.X = 80 + (_windowRect.right) / 2 - 450 / 2;
	_lowBorderRect.Y = _windowRect.bottom / 2 - 300 / 2 - 50;
	_lowBorderRect.Height = 299;
	_lowBorderRect.Width = 449;
}

HWND CreateVolumeTrackbar(HWND hwndParent)
{
	InitCommonControls();

	GetClientRect(hwndParent, &_windowRect);

	CreateWindow(L"STATIC", L"Volume :", WS_CHILD | WS_VISIBLE, _windowRect.left + 590, _windowRect.bottom - 76, 100, 25, hwndParent, NULL, _hInstance, NULL);

	HWND hTrackbar = CreateWindowEx(NULL, TRACKBAR_CLASS, L"Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
		_windowRect.left + 650, _windowRect.bottom - 80, 250, 25, hwndParent, (HMENU)VOLUMETRACKBAR_ID, _hInstance, NULL);

	if (!hTrackbar)
	{
		LastError(L"Create Trackbar");

		return NULL;
	}

	SendMessage(hTrackbar, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(20, 5000));
	SendMessage(hTrackbar, TBM_SETPAGESIZE, 0, (LPARAM)20);
	SendMessage(hTrackbar, TBM_SETSEL, FALSE, (LPARAM)MAKELONG(20, 5000));
	SendMessage(hTrackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)volume);

	return hTrackbar;
}

