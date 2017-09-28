#pragma once
#include <string>
#include <memory>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
//-----------------------------------------------------------------------------




 ///---------------------------------------------------------------------------
///
/// спрайты
///
///
///----------------------------------------------------------------------------
class ASprite
{
public:
	ASprite(const std::string &fileName);
	virtual ~ASprite();
	void draw(Graphics *graphics);

	void move(const int x, const int y);
	int x() const { return mX; }
	int y() const { return mY; }

	void show();
	void hide();
	bool isVisible() const { return mVisible; }

	ASprite(const ASprite&) = delete;
	void operator=(const ASprite&) = delete;

	void destroy();

private:
	Image *mImage = nullptr;
	std::wstring mName;
	int mX = 0;
	int mY = 0;
	bool mVisible = false;
	bool mInit = false;

	void init();
};
///----------------------------------------------------------------------------





 ///---------------------------------------------------------------------------
///
/// Общие функции библиотеки
///
///
///----------------------------------------------------------------------------
void print		(const std::string &text);
void println	(const std::string &text);
void print		(const int value);
void println	(const int value);
void print		(const float value);
void println	(const float value);

void cls(); //очистка экрана
void delay(const int ms); //задержка времени
void main(); //основаня точка входа
void repaint();
int  random(const int max); //генерация случайного числа

void scan(int &value); //ввод чисел
void scan(float &value); //ввод чисел
void scan(std::string &value); //ввод строчек
void scan(char* value, const int size); //ввод строк


extern ASprite robot;
extern ASprite cat;
extern ASprite dog;


extern bool keyLeft;
extern bool keyRight;
extern bool keyUp;
extern bool keyDown;
extern bool keySpace;

extern int  screenWidth;
extern int  screenHeight;