#include "kidstown.h"




void main()
{
	print("Привет мир!\ntest");
	println("Новая программа: ");
	print(-12);
	println(3.14f);
	println(-1030);

	int n = 0;
	while (true)
	{
		println(n++);
		for (int i = 0; i < 500; i++)
		{
			//cls();
			robot.move(i, 150);
			delay(1);
		}

		println(n++);
		for (int i = 500; i > 0; i--)
		{
			//cls();
			robot.move(i, 150);
			delay(1);
		}
	}

}