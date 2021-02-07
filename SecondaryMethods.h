#pragma once

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
