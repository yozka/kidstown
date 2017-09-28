#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <thread>
#include <ctime>
#include <mutex>
#include <locale>
#include <codecvt>
#include <list>
#include <random>
#include <chrono>

#include "kidstown.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
///----------------------------------------------------------------------------


ASprite robot("robot.png");
ASprite cat("cat.png");
ASprite dog("dog.png");


bool keyLeft	= false;
bool keyRight	= false;
bool keyUp		= false;
bool keyDown	= false;
bool keySpace	= false;

int  screenWidth = 0;
int  screenHeight = 0;



//глобальные переменные библиотеки
HWND		g_hWnd		= 0;
HWND		g_hScan		= 0;
bool		g_edit			= false;
WNDPROC		g_oldEditProc	= nullptr;


int			g_width		= 0;
int			g_height	= 0;
Bitmap*		g_screen			= nullptr;
Graphics*	g_screenGraphics	= nullptr;
std::mutex	g_mutexScreen;

//std::thread g_workerThread;
HANDLE		g_hThread = 0;

std::wstring g_printText; //текст для отрисовки
const int margin = 10;

static std::list<ASprite*> g_sprites; //спрайты
///----------------------------------------------------------------------------




///----------------------------------------------------------------------------
void destroyScreen();	//удаление двойного буфера экрана
void onResize(HWND hWnd);
void onPaint(HDC hdc);
void onKeydown(const WPARAM wParam);
void onKeyup(const WPARAM wParam);
DWORD WINAPI onProcess(LPVOID lpThreadParameter);
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndProcEdit(HWND, UINT, WPARAM, LPARAM);
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
	wndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
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


	g_hScan = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE  | ES_LEFT | WS_BORDER, 10, 55, 500, 30, g_hWnd, NULL, hInstance, NULL);
	auto hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	SendMessage(g_hScan, WM_SETFONT, WPARAM(hFont), TRUE);
	ShowWindow(g_hScan, SW_HIDE);
	g_oldEditProc = (WNDPROC)SetWindowLongPtr(g_hScan, GWLP_WNDPROC, (LONG_PTR)wndProcEdit);
	EnableWindow(g_hScan, false);

	ShowWindow(g_hWnd, iCmdShow);
	UpdateWindow(g_hWnd);

	//запуск потока с программой пользователя
	//g_workerThread = std::thread(onProcess, 0);
	DWORD dwThreadId = 0;
	g_hThread = CreateThread(NULL,0, onProcess, NULL, 	0, &dwThreadId);



	//основной процесс
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	/*
	if (g_workerThread.joinable())
	{
		g_workerThread.detach();
		TerminateThread(g_hThread, (unsigned long)0x00);
		CloseHandle(g_hThread);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	*/
	if (g_hThread)
	{
		TerminateThread(g_hThread, (unsigned long)0x00);
		CloseHandle(g_hThread);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}



	std::list<ASprite*> sprites;
	sprites.insert(sprites.begin(), g_sprites.begin(), g_sprites.end());
	for (const auto &sprite : sprites)
	{
		sprite->destroy();
	}
	g_sprites.clear();
	sprites.clear();

	destroyScreen();
	GdiplusShutdown(gdiplusToken);
	g_hWnd = 0;
	return (INT)msg.wParam;
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

	case WM_KEYDOWN:
	{
		onKeydown(wParam);
		return 0;
	}
	case WM_KEYUP:
	{
		//char c = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);
		onKeyup(wParam);
		return 0;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc
///----------------------------------------------------------------------------




 ///---------------------------------------------------------------------------
///
/// нажатие на кнопку
///
///
///----------------------------------------------------------------------------
void onKeydown(const WPARAM wParam)
{
	switch (wParam)
	{
	case VK_LEFT	: keyLeft	= true; keyRight = false; break;
	case VK_RIGHT	: keyRight	= true; keyLeft	 = false; break;
	case VK_UP		: keyUp		= true; keyDown  = false; break;
	case VK_DOWN	: keyDown	= true; keyUp    = false; break;
	case VK_SPACE	: keySpace	= true; break;
	}
}
///----------------------------------------------------------------------------





 ///---------------------------------------------------------------------------
///
/// отпускаем кнопку
///
///
///----------------------------------------------------------------------------
void onKeyup(const WPARAM wParam)
{
		switch (wParam)
	{
	case VK_LEFT	: keyLeft	= false; break;
	case VK_RIGHT	: keyRight	= false; break;
	case VK_UP		: keyUp		= false; break;
	case VK_DOWN	: keyDown	= false; break;
	case VK_SPACE	: keySpace	= false; break;
	}
}
///----------------------------------------------------------------------------




///---------------------------------------------------------------------------
///
/// Оконная процедура
///
///
///----------------------------------------------------------------------------
LRESULT CALLBACK wndProcEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_ENABLE:
   {
	   if (wParam == 1)
	   {
		   SetFocus(hWnd);
	   }
	   else
	   {
		   SetFocus(g_hWnd);
	   }
	   break;
   }
   
   case WM_KEYDOWN:
         switch (wParam)
         {
          case VK_RETURN:

			  g_edit = false;
			  SetFocus(g_hWnd);
              break;
		  
		  default:
			  return CallWindowProc(g_oldEditProc, hWnd, message, wParam, lParam);
         }
		 break;
    default:
         return CallWindowProc(g_oldEditProc, hWnd, message, wParam, lParam);
   }
   return 0;
}
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
	screenWidth = iWidth;
	screenHeight = iHeight;

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



	g_screenGraphics->Clear(Color(255, 255, 255, 255));

	//отрисовываем текст
	SolidBrush  brush(Color(255, 10, 5, 10));
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
	PointF		pos((float)margin, (float)margin);

	g_screenGraphics->DrawString(g_printText.c_str(), -1, &font, pos, &brush);



	//отрисовываем спрайты
	for (const auto &sprite : g_sprites)
	{
		if (sprite->isVisible())
		{
			sprite->draw(g_screenGraphics);
		}
	}



	//отрисовываем буфер
	graphics.DrawImage(g_screen, 0, 0);
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
DWORD WINAPI onProcess(LPVOID lpThreadParameter)
{
	//g_hThread = GetCurrentThread();
	//auto h = GetCurrentThreadId();
	main();
	//g_workerThread.detach();
	g_hThread = 0;
	return 0;
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// генерация случайного числа
///
///
///----------------------------------------------------------------------------
int random(const int max)
{
	static std::default_random_engine gen(
		static_cast<unsigned>(
			std::chrono::system_clock::now().time_since_epoch().count()
			)
	);
	std::uniform_int_distribution<int> distribution(0, max);
	return distribution(gen);
}




 ///---------------------------------------------------------------------------
///
/// преоброзование текста
///
///
///----------------------------------------------------------------------------
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r.substr(0, s.length());
}
///----------------------------------------------------------------------------



 ///---------------------------------------------------------------------------
///
/// преоброзование текста
///
///
///----------------------------------------------------------------------------
std::string sw2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, L'\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r.substr(0, s.length());
}
///----------------------------------------------------------------------------




 ///---------------------------------------------------------------------------
///
/// вывод текста
///
///
///----------------------------------------------------------------------------
void print(const std::string &text)
{
	const auto ts = s2ws(text);
	g_mutexScreen.lock();
	g_printText.append(ts);
	const auto n = std::count(g_printText.begin(), g_printText.end(), '\n');
	if (n > 32)
	{
		//нужно отрезать лишние
		const auto i = g_printText.find_first_of('\n') + 1;
		g_printText = g_printText.substr(i, g_printText.length() - i);
	}

	g_mutexScreen.unlock();
	repaint();
}
///----------------------------------------------------------------------------




void println(const std::string &text) 
{ 
	print(std::string("\n") + text);
}

void print(const int value)
{
	print(std::to_string(value));
}

void println(const int value)
{
	println(std::to_string(value));
}

void print(const float value)
{
	std::string str = std::to_string(value);
	str.erase(str.find_last_not_of('0') + 1, std::string::npos);
	print(str);
}

void println(const float value)
{
	std::string str = std::to_string(value);
	str.erase(str.find_last_not_of('0') + 1, std::string::npos);
	println(str);
}





 ///---------------------------------------------------------------------------
///
/// очистка экрана
///
///
///----------------------------------------------------------------------------
void cls()
{
	g_mutexScreen.lock();
	g_printText = std::wstring();
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
ASprite :: ASprite(const std::string &fileName)
	:
	mImage(nullptr),
	mName(s2ws(fileName))
{
	
}
///----------------------------------------------------------------------------







 ///---------------------------------------------------------------------------
///
/// Деструктор
///
///
///----------------------------------------------------------------------------
ASprite :: ~ASprite()
{
	destroy();
}
///----------------------------------------------------------------------------





 ///---------------------------------------------------------------------------
///
/// уничтожение
///
///
///----------------------------------------------------------------------------
void ASprite :: destroy()
{
	if (mImage)
	{
		delete mImage;
		mImage = nullptr;
	}

	if (mInit)
	{
		g_sprites.remove(this);
		mInit = false;
	}
}
///----------------------------------------------------------------------------





 ///---------------------------------------------------------------------------
///
/// Отрисовка
///
///
///----------------------------------------------------------------------------
void ASprite:: draw(Graphics *graphics)
{
	if (mImage == nullptr)
	{
		mImage = new Image(mName.c_str());
	}
	graphics->DrawImage(mImage, mX, mY);
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// перенос спрайтов
///
///
///----------------------------------------------------------------------------
void ASprite :: move(const int x, const int y)
{
	if (x != mX || y != mY || !mVisible)
	{
		init();
		mX = x;
		mY = y;
		mVisible = true;
		repaint();
	}
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// показать спрайт
///
///
///----------------------------------------------------------------------------
void ASprite :: show()
{
	init();
	mVisible = true;
	repaint();
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// скрыть спрайт
///
///
///----------------------------------------------------------------------------
void ASprite :: hide()
{
	init();
	mVisible = false;
	repaint();
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// инциализация спрайта
///
///
///----------------------------------------------------------------------------
void ASprite::init()
{
	if (!mInit)
	{
		mInit = true;
		g_sprites.push_back(this);
	}
}
///----------------------------------------------------------------------------







 ///---------------------------------------------------------------------------
///
/// ввод строчек
///
///
///----------------------------------------------------------------------------
void scan(std::string &value)
{
	g_edit = true;

	SetWindowTextA(g_hScan, value.c_str());
	ShowWindow(g_hScan, SW_SHOW);
	EnableWindow(g_hScan, true);
	SetFocus(g_hScan);
	SetActiveWindow(g_hScan);
	SendMessage(g_hScan, EM_SETSEL, 0, -1);
	
	const auto n = std::count(g_printText.begin(), g_printText.end(), '\n') + 1;
	int top = margin + (int)((float)n * 18.5f);

	SetWindowPos(g_hScan, 0, margin, top, 0, 0, SWP_NOSIZE);

	while (g_edit)
	{
		delay(100);
	}

	ShowWindow(g_hScan, SW_HIDE);
	EnableWindow(g_hScan, false);
	SetFocus(g_hWnd);
	SetActiveWindow(g_hWnd);

	
	int nLn = GetWindowTextLength(g_hScan);
	

	std::wstring str;
	str.resize(nLn + 1);

	GetWindowText(g_hScan, (LPTSTR)str.data(), nLn + 1);
	str = str.substr(0, nLn);

	value = sw2s(str);
}
///----------------------------------------------------------------------------





///---------------------------------------------------------------------------
///
/// ввод чисел
///
///
///----------------------------------------------------------------------------
void scan(int &value)
{
	std::string text;
	if (value != 0)
	{
		text = std::to_string(value);
	}
	scan(text);
	value = std::atoi(text.c_str());
}
///----------------------------------------------------------------------------





 ///---------------------------------------------------------------------------
///
/// ввод чисел
///
///
///----------------------------------------------------------------------------
void scan(float &value)
{
	std::string text;
	if (value != 0)
	{
		text = std::to_string(value);
	}
	scan(text);
	value = (float)std::atof(text.c_str());
}
///----------------------------------------------------------------------------






 ///---------------------------------------------------------------------------
///
/// ввод строк
///
///
///----------------------------------------------------------------------------
void scan(char* value, const int size)
{
	std::string text;
	text.resize(size);
	size_t sz = (size_t)(min((int)strlen(value), size));

	std::memcpy(&text.at(0), value, sz);

	scan(text);

	sz = (size_t)(max((int)text.length() + 1, size));

	std::memset(value, 0, sz);
	std::memcpy(value, &text.at(0), text.length());

}
///----------------------------------------------------------------------------
