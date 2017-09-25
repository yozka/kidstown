#pragma once
#include <string>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
class AImage
{
public:
	AImage(const std::string &fileName);
	~AImage();
	void draw(const int x, const int y);

private:
	Image *mImage;
	std::wstring mName;
};


void cls();
void delay(const int ms);
void main();


Graphics* screen();
void repaint();



static AImage robot("robot.png");