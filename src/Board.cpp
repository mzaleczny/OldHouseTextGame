#include "Board.h"
#include "Common.h"

void TBoard::Draw(std::string& Destination)
{
	Destination.reserve(2048);
	//Destination = "";
	//Destination.re
	//Gorna pozioma ramka
	Destination = F_LT;
	for (int i = 1; i < LineLen - 1; ++i)
	{
		if (i % 2 == 1)
		{
			Destination += F_T;
		}
		else
		{
			Destination += F_TD;
		}
	}
	Destination += F_RT;
	Destination += '\n';

	for (int j = 1; j < BoardHeight - 1; ++j)
	{
		// srodkowa pozioma ramka
		Destination += F_D;
		for (int i = 1; i < LineLen - 1; ++i)
		{
			if (i % 2 == 1)
			{
				Destination += FOG;
			}
			else
			{
				Destination += F_D;
			}
		}
		Destination += F_D;
		Destination += '\n';

		// srodkowy separator
		Destination += F_L;
		for (int i = 1; i < LineLen - 1; ++i)
		{
			if (i % 2 == 1)
			{
				Destination += F_H;
			}
			else
			{
				Destination += F_CR;
			}
		}
		Destination += F_R;
		Destination += '\n';
	}

	Destination += F_D;
	for (int i = 1; i < LineLen - 1; ++i)
	{
		if (i % 2 == 1)
		{
			Destination += FOG;
		}
		else
		{
			Destination += F_D;
		}
	}
	Destination += F_D;
	Destination += '\n';

	//Gorna pozioma ramka
	Destination += F_LB;
	for (int i = 1; i < LineLen - 1; ++i)
	{
		if (i % 2 == 1)
		{
			Destination += F_T;
		}
		else
		{
			Destination += F_TU;
		}
	}
	Destination += F_RB;
	Destination += '\n';
}