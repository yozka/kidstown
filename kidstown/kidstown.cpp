#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "Kidstown.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")


HWND                hWnd;
HWND hread;

int width = 0;
int height = 0;
Bitmap* buffer = nullptr;
Graphics* graphicsbuf = nullptr;








Graphics* screen()
{
	if (graphicsbuf == nullptr)
	{
		graphicsbuf = Graphics::FromImage(buffer);
		graphicsbuf->Clear(Color(255, 255, 255, 255));
	}
	return graphicsbuf;
}


void repaint()
{
	InvalidateRect(hWnd, nullptr, true);
}


void resize(HWND hWnd)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	int iWidth = rcClient.right - rcClient.left;
	int iHeight = rcClient.bottom - rcClient.top;

	if (iWidth == width && iHeight == height)
	{
		return;
	}
	
	width = iWidth;
	height = iHeight;

	if (buffer)
	{
		delete buffer;
		buffer = nullptr;
	}

	if (graphicsbuf)
	{
		delete graphicsbuf;
		graphicsbuf = nullptr;
	}
}


VOID OnPaint(HDC hdc)
{
	Graphics graphics(hdc);

	if (buffer == nullptr)
	{
		if (graphicsbuf)
		{
			delete graphicsbuf;
			graphicsbuf = nullptr;
		}
		buffer = new Bitmap(width, height, &graphics);
	}
	else
	{
		graphics.DrawImage(buffer, 0, 0);
	}

	/*
	Pen      pen(Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);



	SolidBrush  brush(Color(255, 0, 0, 255));
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	PointF      pointF(10.0f, 20.0f);

	graphics.DrawString(L"Привет мир!", -1, &font, pointF, &brush);


	//Image image(L"robot.png");
	//graphics.DrawImage(&image, 200, 100);
	*/



}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("GettingStarted");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("GettingStarted"),   // window class name
		TEXT("Getting Started"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		CW_USEDEFAULT,            // initial x size
		CW_USEDEFAULT,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	hread = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE  | ES_LEFT, 10, 55, 500, 30, hWnd, NULL, hInstance, NULL);
	auto hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	SendMessage(hread, WM_SETFONT, WPARAM(hFont), TRUE);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	main();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	if (buffer)
	{
		delete buffer;
	}

	if (graphicsbuf)
	{
		delete graphicsbuf;
		graphicsbuf = nullptr;
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (message)
	{
	case WM_SIZE:
	{
		resize(hWnd);
		return 0;
	}

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc





AImage::AImage(const std::string &fileName)
	:
	mImage(nullptr),
	mName(std::wstring(fileName.begin(), fileName.end()).c_str())
{
}

AImage::~AImage()
{
	if (mImage)
	{
		//delete mImage;
	}
}

void AImage::draw(const int x, const int y)
{
	if (mImage == nullptr)
	{
		mImage = new Image(mName.c_str());
	}

	auto *graphics = screen();
	graphics->DrawImage(mImage, x, y);
	repaint();
}


void cls()
{
	auto *graphics = screen();
	graphics->Clear(Color(255, 255, 255, 255));

}

void delay(const int ms)
{
	MSG msg;

	for (int i = 0; i < ms; i++)
	{

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(1);
	}


}


