#include "kidstown.h"




void main()
{
	print("Привет мир!\ntest");

	AImage robot("robot.png");
	robot.draw(500, 100);


	for (int i = 0; i < 500; i++)
	{
		//cls();
		robot.draw(i, 150);
		delay(2);
	}

	for (int i = 500; i > 0; i--)
	{
		//cls();
		robot.draw(i, 150);
		delay(2);
	}
	
}