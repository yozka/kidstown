#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <thread>
#include <ctime>
#include <mutex>
#include <locale>
#include <codecvt>

#include "kidstown.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
///----------------------------------------------------------------------------


//глобальные переменные библиотеки
HWND		g_hWnd		= 0;
HWND		g_hScan		= 0;

int			g_width		= 0;
int			g_height	= 0;
Bitmap*		g_screen			= nullptr;
Graphics*	g_screenGraphics	= nullptr;
std::mutex	g_mutexScreen;

std::thread g_workerThread;

PointF		g_printPos = PointF(0, 0);
///----------------------------------------------------------------------------




///----------------------------------------------------------------------------
void destroyScreen();	//удаление двойного буфера экрана
void onResize(HWND hWnd);
void onPaint(HDC hdc);
void onProcess(const int param);
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// Основная программа
///
///
///----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	//Инциализация GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = wndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("kidstown");

	RegisterClass(&wndClass);

	g_hWnd = CreateWindow(
		TEXT("kidstown"),   // window class name
		TEXT("Kidstown"),	// window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		CW_USEDEFAULT,            // initial x size
		CW_USEDEFAULT,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	/*
	hread = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE  | ES_LEFT, 10, 55, 500, 30, hWnd, NULL, hInstance, NULL);
	auto hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	SendMessage(hread, WM_SETFONT, WPARAM(hFont), TRUE);
	*/

	ShowWindow(g_hWnd, iCmdShow);
	UpdateWindow(g_hWnd);

	//запуск потока с программой пользователя
	g_workerThread = std::thread(onProcess, 0);


	//основной процесс
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (g_workerThread.joinable())
	{
		g_workerThread.detach();
	}

	destroyScreen();
	GdiplusShutdown(gdiplusToken);
	g_hWnd = 0;
	return msg.wParam;
}  // WinMain
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// Оконная процедура
///
///
///----------------------------------------------------------------------------
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (message)
	{
	case WM_SIZE:
	{
		onResize(hWnd);
		return 0;
	}

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		onPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// Удаление двойного буфера для отрисовки
///
///
///----------------------------------------------------------------------------
void destroyScreen()
{
	g_mutexScreen.lock();
	if (g_screen)
	{
		delete g_screen;
		g_screen = nullptr;
	}

	if (g_screenGraphics)
	{
		delete g_screenGraphics;
		g_screenGraphics = nullptr;
	}
	g_mutexScreen.unlock();
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// Создание экрана 
///
///
///----------------------------------------------------------------------------
Graphics* screen()
{
	return g_screenGraphics;
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// изменение размеров
///
///
///----------------------------------------------------------------------------
void onResize(HWND hWnd)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	int iWidth = rcClient.right - rcClient.left;
	int iHeight = rcClient.bottom - rcClient.top;

	if (iWidth == g_width && iHeight == g_height)
	{
		return;
	}

	g_width = iWidth;
	g_height = iHeight;
	destroyScreen();

	//создание двойного буфера экрана
	//для отрисовки
	if (g_screen == nullptr)
	{
		g_mutexScreen.lock();
		g_screen = new Bitmap(g_width, g_height);

		if (g_screenGraphics)
		{
			delete g_screenGraphics;
		}
		g_screenGraphics = Graphics::FromImage(g_screen);
		g_screenGraphics->Clear(Color(255, 255, 255, 255));
		g_mutexScreen.unlock();
	}
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
///  Отрисовка буфера
///
///
///----------------------------------------------------------------------------
void onPaint(HDC hdc)
{
	g_mutexScreen.lock();
	Graphics graphics(hdc);


	//отрисовываем буфер
	if (g_screen)
	{
		graphics.DrawImage(g_screen, 0, 0);
	}
	//отрисовываем спрайты



	g_mutexScreen.unlock();
	/*
	Pen      pen(Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);






	//Image image(L"robot.png");
	//graphics.DrawImage(&image, 200, 100);
	*/
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
///  перерисовка
///
///
///----------------------------------------------------------------------------
void repaint()
{
	InvalidateRect(g_hWnd, nullptr, true);
}
///----------------------------------------------------------------------------









///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------





 ///---------------------------------------------------------------------------
///
/// Отдельный процесс запуска пользовательского кода
///
///
///----------------------------------------------------------------------------
void onProcess(const int param)
{
	main();
	g_workerThread.detach();
}
///----------------------------------------------------------------------------



std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}


 ///---------------------------------------------------------------------------
///
/// вывод текста
///
///
///----------------------------------------------------------------------------
void print(const std::string &text)
{
	const auto ts = s2ws(text);

	 

	auto *graphics = screen();
	g_mutexScreen.lock();
	

	SolidBrush  brush(Color(255, 0, 0, 255));
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);

	graphics->DrawString(ts.c_str(), -1, &font, g_printPos, &brush);


	g_mutexScreen.unlock();
}
///----------------------------------------------------------------------------



 ///---------------------------------------------------------------------------
///
/// очистка экрана
///
///
///----------------------------------------------------------------------------
void cls()
{
	auto *graphics = screen();
	g_mutexScreen.lock();
	graphics->Clear(Color(255, 255, 255, 255));

	g_printPos = PointF(0, 0);
	g_mutexScreen.unlock();
}
///----------------------------------------------------------------------------







 ///---------------------------------------------------------------------------
///
/// задержка
///
///
///----------------------------------------------------------------------------
void delay(const int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
///----------------------------------------------------------------------------







 ///---------------------------------------------------------------------------
///
/// Картинки
///
///
///----------------------------------------------------------------------------
AImage :: AImage(const std::string &fileName)
	:
	mImage(nullptr),
	mName(std::wstring(fileName.begin(), fileName.end()).c_str())
{
}
///----------------------------------------------------------------------------










 ///---------------------------------------------------------------------------
///
/// Деструктор
///
///
///----------------------------------------------------------------------------
AImage :: ~AImage()
{
	if (mImage)
	{
		delete mImage;
	}
}
///----------------------------------------------------------------------------










 ///---------------------------------------------------------------------------
///
/// Отрисовка
///
///
///----------------------------------------------------------------------------
void AImage :: draw(const int x, const int y)
{
	if (mImage == nullptr)
	{
		mImage = new Image(mName.c_str());
	}

	auto *graphics = screen();
	if (graphics == nullptr)
	{
		return;
	}

	g_mutexScreen.lock();
	graphics->DrawImage(mImage, x, y);
	g_mutexScreen.unlock();
	repaint();
}




