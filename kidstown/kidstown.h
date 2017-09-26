#pragma once
#include <string>
#include <memory>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
//-----------------------------------------------------------------------------




 ///---------------------------------------------------------------------------
///
/// �������
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
	Image *mImage;
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
/// ����� ������� ����������
///
///
///----------------------------------------------------------------------------
void print		(const std::string &text);
void println	(const std::string &text);
void print		(const int value);
void println	(const int value);
void print		(const float value);
void println	(const float value);

void cls(); //������� ������
void delay(const int ms); //�������� �������
void main(); //�������� ����� �����
void repaint();



static ASprite robot("robot.png");
