#pragma once
#define TIMER_ID 228

#include <Windows.h>
#include <gdipluseffects.h>
#include <gdiplus.h>
#include <process.h>
#include <memory>
#include <commctrl.h>
#include <Richedit.h>
#include <string>
#include <algorithm>
#include <ctime>

//--------------------------

using Gdiplus::ImageCodecInfo;
using Gdiplus::Graphics;
using Gdiplus::Rect;
using Gdiplus::Point;
using Gdiplus::Pen;
using Gdiplus::Image;
using Gdiplus::PropertyItem;
using Gdiplus::Color;
using Gdiplus::Bitmap;
using Gdiplus::Status;
using Gdiplus::REAL;
using Gdiplus::ImageAttributes;

using std::unique_ptr;
using std::wstring;

//--------------------------

HWND _hWnd;  //--global MainWindow handler--
RECT _windowRect{};  //--global rect for usage--
HINSTANCE _hInstance;  //--global instance handler--
UINT currentFrame = 0;  //--for frame indexing--
UINT frameCount = 0;  //--frames amount--
DWORD timerDelay = 200;  //--for changing timer delay--
DWORD volume = 100;
HANDLE paintingThreadHandle = NULL;
HANDLE musicThreadHandle = NULL;
BOOL threadExitFlag = FALSE;
HDC _hdc = NULL;

//--------------------------

Rect _highBorderRect{};
Rect _lowBorderRect{};
Image* gif = nullptr;
Rect _gifRect{};
GUID* dimensionsId = nullptr;
PropertyItem* propItem = nullptr;
WCHAR currentDirectory[MAX_PATH];
INT framesAmount = 0;
INT songsAmount = 0;
wstring currentSong = L"";

//--------------------------

enum SONG_STATES {PAUSE, PLAY, SKIP, LOAD};
INT songState = -1;

//--------------------------

//--last error displayer--
VOID LastError(const WCHAR* phase);
//--method to load and prepare gif to display--
BOOL LoadGif(LPCWSTR fileName);
//--method to set timer and first frame to display--
VOID Start();
//--method to display frames according to timer--
VOID DisplayOnTimer();
//--method to paint borders--
VOID DrawFramesBorders();
//--method to paint background --
VOID DrawBackground();
//--method to load from fileexplorer--
BOOL LoadFromFileExplorer();
//--method to identify encoder class--
//INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
//--method to init glitches from file--
wstring FindGlitchedFrame(INT frameId);
//--method to count frames amount--
BOOL GetFramesAmount();
//--method to display glitches--
VOID DrawGlitchedBackground();
//--method to count songs--
BOOL GetSongsAmount();
//--method to find next song on skip--
VOID FindNextSong();
//--method to play current song--
VOID PlaySong(BOOL flag);
//--method to load custom song--
VOID LoadSongFromFileExplorer();
//-------------------------

unsigned __stdcall _glitch_painting_thread(void* param);
unsigned __stdcall _bckgrnd_music_thread(void* param);

//-------------------------


/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


BOOL LoadGif(LPCWSTR fileName)
{
	//--load gif--
	if (gif != NULL)
	{
		delete gif;
	}

	gif = new Image(fileName);

	if (!gif)
	{
		LastError(L"Load Gif!!");

		return FALSE;
	}

	//--getting dimension id--
	UINT frameDimensionCount = gif->GetFrameDimensionsCount();

	dimensionsId = new GUID[frameDimensionCount];
	gif->GetFrameDimensionsList(dimensionsId, frameDimensionCount);

	//--getting frame count--
	frameCount = gif->GetFrameCount(dimensionsId);

	//--getting delay between frames 
	UINT buffer = gif->GetPropertyItemSize(PropertyTagFrameDelay);
	propItem = (PropertyItem*)malloc(buffer);
	gif->GetPropertyItem(PropertyTagFrameDelay, buffer, propItem);


	HDC hdc = GetDC(_hWnd);
	Graphics* timerGraphics = new Graphics(hdc);
	timerGraphics->DrawImage(gif, _gifRect);

	ReleaseDC(_hWnd, hdc);
	delete[] dimensionsId;
	delete timerGraphics;

	return TRUE;
}

//-------------------------

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID Start()
{
	//--select fisrt frame--
	GUID guId = Gdiplus::FrameDimensionTime;
	gif->SelectActiveFrame(&guId, currentFrame);

	//--set start timer--
	SetTimer(_hWnd, TIMER_ID, ((UINT*)propItem[0].value)[0] * 10, NULL);

	//--go to next frame--
	currentFrame++;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID DisplayOnTimer()
{
	HDC localHDC = GetDC(_hWnd);

	//--draw frame--
	Graphics* timerGraphics = new Graphics(localHDC);
	timerGraphics->DrawImage(gif, _gifRect);

	delete timerGraphics;

	//--kill timer to reset it
	KillTimer(_hWnd, TIMER_ID);

	//--select next frame--
	GUID guId = Gdiplus::FrameDimensionTime;

	gif->SelectActiveFrame(&guId, currentFrame);

	//--reset timer--
	SetTimer(_hWnd, TIMER_ID, timerDelay, NULL);

	//--go to next frame--, if end start from end
	currentFrame = (++currentFrame) % frameCount;

	ReleaseDC(_hWnd, localHDC);
}


/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

//INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
//{
//	UINT  num = 0;
//	UINT  size = 0;
//
//	ImageCodecInfo* pImageCodecInfo = nullptr;
//
//	Gdiplus::GetImageEncodersSize(&num, &size);
//
//	if (size == 0) return -1;
//
//	pImageCodecInfo = (ImageCodecInfo*)malloc(size);
//
//	if (pImageCodecInfo == NULL) return -1;
//
//	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
//
//	for (UINT j = 0; j < num; ++j)
//	{
//		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
//		{
//			*pClsid = pImageCodecInfo[j].Clsid;
//			free(pImageCodecInfo);
//
//			return j;
//		}
//	}
//
//	free(pImageCodecInfo);
//
//	return -1;
//}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID LastError(const WCHAR* phase)
{
	std::wstring temp = std::to_wstring(GetLastError());

	MessageBox(_hWnd, phase, temp.c_str(), MB_OK);
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID DrawFramesBorders()
{
	Color bordersColor(0, 0, 0);
	Pen* pen = new Pen(bordersColor, 2);

	pen->SetDashStyle(Gdiplus::DashStyleDot);

	Graphics* tempGraphics = new Graphics(_hdc);

	tempGraphics->DrawRectangle(pen, _highBorderRect);
	tempGraphics->DrawRectangle(pen, _lowBorderRect);

	pen->SetDashStyle(Gdiplus::DashStyleSolid);
	pen->SetWidth(3);

	//--corner lines--
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X, _highBorderRect.Y), Point(_lowBorderRect.X, _lowBorderRect.Y));
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X + _highBorderRect.Width, _highBorderRect.Y), Point(_lowBorderRect.X + _lowBorderRect.Width, _lowBorderRect.Y));
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X, _highBorderRect.Y + _highBorderRect.Height), Point(_lowBorderRect.X, _lowBorderRect.Y + _lowBorderRect.Height));
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X + _highBorderRect.Width, _highBorderRect.Y + _highBorderRect.Height), Point(_lowBorderRect.X + _lowBorderRect.Width, _lowBorderRect.Y + _lowBorderRect.Height));

	pen->SetWidth(5);

	//--middle lines--
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X, _highBorderRect.Y + +_highBorderRect.Height / 2), Point(_lowBorderRect.X, _lowBorderRect.Y + _lowBorderRect.Height / 2));
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X + _highBorderRect.Width / 2, _highBorderRect.Y), Point(_lowBorderRect.X + _lowBorderRect.Width / 2, _lowBorderRect.Y));
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X + _highBorderRect.Width / 2, _highBorderRect.Y + _highBorderRect.Height), Point(_lowBorderRect.X + _lowBorderRect.Width / 2, _lowBorderRect.Y + _lowBorderRect.Height));
	tempGraphics->DrawLine(pen, Point(_highBorderRect.X + _highBorderRect.Width, _highBorderRect.Y + _highBorderRect.Height / 2), Point(_lowBorderRect.X + _lowBorderRect.Width, _lowBorderRect.Y + _lowBorderRect.Height / 2));

	delete pen;
	delete tempGraphics;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID DrawBackground()
{
	SetCurrentDirectory(currentDirectory);

	Graphics* tempGraphics = new Graphics(_hdc);

	Bitmap* backgroundBitmap = new Bitmap(L"Save\\Background\\background.bmp");

	tempGraphics->DrawImage(backgroundBitmap, 110, 0, 900, 700);

	if (gif != NULL)
	{
		tempGraphics->DrawImage(gif, _gifRect);
	}

	if (backgroundBitmap != NULL)
	{
		delete backgroundBitmap;
	}

	delete tempGraphics;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

BOOL LoadFromFileExplorer()
{
	OPENFILENAME ofn;
	TCHAR FileName[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(FileName, MAX_PATH);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = _hWnd;
	ofn.lpstrFilter = L"Gif Files(*.gif)\0*.gif";
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileName(&ofn))
	{
		if (!LoadGif(ofn.lpstrFile)) return FALSE;
	}
	else return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

unsigned __stdcall _glitch_painting_thread(void* param)
{
	if (!GetFramesAmount()) return 0;

	INT time = 0;

	while (TRUE)
	{
		if (threadExitFlag) return 0;

		time = rand() % 8000 + 3000;

		Sleep(time);

		DrawGlitchedBackground();
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

BOOL GetFramesAmount()
{
	SetCurrentDirectory(currentDirectory);

	wstring fileExtension = L"";

	WIN32_FIND_DATA findData{};

	HANDLE file = FindFirstFile(L"Save\\GlitchesDB\\*", &findData);

	if (file == INVALID_HANDLE_VALUE) return FALSE;

	while (FindNextFileW(file, &findData))
	{
		fileExtension = findData.cFileName;
		fileExtension = fileExtension.substr(fileExtension.find_last_of(L"."), +fileExtension.size() - fileExtension.find_last_of(L"."));
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

		if (fileExtension._Equal(L".png") || fileExtension._Equal(L".bmp") || fileExtension._Equal(L".jpg") || fileExtension._Equal(L".jpeg") || fileExtension._Equal(L".wmf"))
		{
			framesAmount++;
		}
	}

	FindClose(file);

	return (framesAmount > 0);
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

wstring FindGlitchedFrame(INT frameId)
{
	SetCurrentDirectory(currentDirectory);

	wstring resStr = L"Save\\GlitchesDB\\";
	WIN32_FIND_DATA findData{};
	HANDLE file = FindFirstFile(L"Save\\GlitchesDB\\*", &findData);
	wstring fileExtension = L"";

	if (file == INVALID_HANDLE_VALUE) return L"";

	while (FindNextFileW(file, &findData))
	{
		fileExtension = findData.cFileName;
		fileExtension = fileExtension.substr(fileExtension.find_last_of(L"."), +fileExtension.size() - fileExtension.find_last_of(L"."));
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

		if (fileExtension._Equal(L".png") || fileExtension._Equal(L".bmp") || fileExtension._Equal(L".jpg") || fileExtension._Equal(L".jpeg") || fileExtension._Equal(L".wmf"))
		{
			if (frameId == 0)
			{
				FindClose(file);
				resStr += findData.cFileName;
				return resStr;
			}

			frameId--;
		}

	}

	return L"";
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID DrawGlitchedBackground()
{
	SetCurrentDirectory(currentDirectory);

	HDC localHDC = GetDC(_hWnd);

	Graphics* tempGraphics = new Graphics(localHDC);

	Image* tempImage = nullptr;

	for (INT i = 0; i < 4; i++)
	{
		tempImage = new Image(FindGlitchedFrame(rand() % framesAmount).c_str());
		tempGraphics->DrawImage(tempImage, 150, 0, 834, 700);

		if (gif != NULL) tempGraphics->DrawImage(gif, _gifRect);

		delete tempImage;

		Sleep(60);
	}

	ReleaseDC(_hWnd, localHDC);

	delete tempGraphics;

	_windowRect.left = 150;
	_windowRect.right = 1000;

	InvalidateRect(_hWnd, &_windowRect, TRUE);
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

unsigned __stdcall _bckgrnd_music_thread(void* param)
{
	GetSongsAmount();

	while (TRUE)
	{
		if (threadExitFlag) return 0;

		switch (songState)
		{
		case SONG_STATES::LOAD:
		{
			LoadSongFromFileExplorer();

			mciSendString(L"close bckgSound", 0, 0, 0);

			PlaySong(FALSE);

			songState = -1;

			break;
		}

		case SONG_STATES::PLAY:
		{
			PlaySong(TRUE);

			songState = -1;

			break;
		}

		case SONG_STATES::SKIP:
		{
			mciSendString(L"close bckgSound", 0, 0, 0);

			FindNextSong();

			songState = SONG_STATES::PLAY;

			break;
		}

		case SONG_STATES::PAUSE:
		{
			mciSendString(L"pause bckgSound", 0, 0, 0);

			songState = -1;

			break;
		}

		default: break;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

BOOL GetSongsAmount()
{
	WIN32_FIND_DATA findData{};

	wstring fileExtension = L"";

	wstring path = currentDirectory;
	path += L"\\Save\\MusicDB\\*";

	HANDLE hFile = FindFirstFile(path.c_str(), &findData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	while (FindNextFile(hFile, &findData))
	{
		fileExtension = findData.cFileName;
		fileExtension = fileExtension.substr(fileExtension.find_last_of(L"."), +fileExtension.size() - fileExtension.find_last_of(L"."));
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

		if (fileExtension._Equal(L".mp3"))
		{
			songsAmount++;

			if (songsAmount == 1) currentSong = findData.cFileName;
		}

	}

	FindClose(hFile);

	return (songsAmount > 0);
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID FindNextSong()
{
	WIN32_FIND_DATA findData{};

	INT songId = rand() % songsAmount;

	wstring path = currentDirectory;
	path += L"\\Save\\MusicDB\\*";

	wstring fileExtension = L"";

	HANDLE hFile = FindFirstFile(path.c_str(), &findData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	while (FindNextFile(hFile, &findData))
	{
		fileExtension = findData.cFileName;
		fileExtension = fileExtension.substr(fileExtension.find_last_of(L"."), +fileExtension.size() - fileExtension.find_last_of(L"."));
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

		if (fileExtension._Equal(L".mp3"))
		{
			if (songId == 0)
			{
				currentSong = findData.cFileName;

				break;
			}

			songId--;
		}
	}

	FindClose(hFile);
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


VOID PlaySong(BOOL flag)
{
	wstring commandString = L"";

	if (flag)
	{
		commandString += L"open \"";
		commandString += currentDirectory;
		commandString += L"\\Save\\MusicDB\\";
		commandString += currentSong;
		commandString += L"\" type mpegvideo alias bckgSound";
	}
	else
	{
		commandString += L"open \"";
		commandString += currentSong;
		commandString += L"\" type mpegvideo alias bckgSound";
	}

	mciSendString(commandString.c_str(), 0, 0, 0);
	mciSendString(L"play bckgSound", 0, 0, 0);

	waveOutSetVolume(NULL, MAKELONG(volume, volume));

	commandString = L"";
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

VOID LoadSongFromFileExplorer()
{
	OPENFILENAME ofn;
	TCHAR FileName[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(FileName, MAX_PATH);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = _hWnd;
	ofn.lpstrFilter = L"Gif Files(*.mp3)\0*.mp3";
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileName(&ofn))
	{
		currentSong = ofn.lpstrFile;
	}
	else LastError(L"Load from FileExplorer");
}