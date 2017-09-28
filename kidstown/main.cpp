#include "kidstown.h"




void main()
{
	print("Привет мир!");
	
	for (int i = 0; i <= 10; i++)
	{
		println(i);
	}

	int i = 54;
	scan(i);
	println(i);


	float f = 34.55f;
	scan(f);
	println(f);


	char ch[20] = "Привет мир";
	scan(ch, 20);
	println(ch);


	int n = random(10);
	println(n);
	if (n > 5)
	{
		cat.move(100, 100);
	}
	else
	{
		dog.move(100, 100);
	}

	int x = 0;
	int y = 0;

	while (true)
	{
		if (keyLeft)
		{
			x -= 1;
		}
		if (keyRight)
		{
			x += 1;
		}
		if (keyUp)
		{
			y -= 1;
		}
		if (keyDown)
		{
			y += 1;
		}
		robot.move(x, y);
		delay(2);
	}
}