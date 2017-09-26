#pragma once
#include <string>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
//-----------------------------------------------------------------------------



 ///---------------------------------------------------------------------------
///
/// Клас картинок
///
///
///----------------------------------------------------------------------------
class AImage
{
public:
	AImage(const std::string &fileName);
	virtual ~AImage();
	void draw(const int x, const int y);

private:
	Image *mImage;
	std::wstring mName;
};
///----------------------------------------------------------------------------




 ///---------------------------------------------------------------------------
///
/// Общие функции библиотеки
///
///
///----------------------------------------------------------------------------
void print(const std::string &text);
void cls(); //очистка экрана
void delay(const int ms); //задержка времени
void main(); //основаня точка входа


Graphics* screen();
void repaint();



