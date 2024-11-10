#include "Cards.h"
#include "TextEngine/Desktop.h"

const char* HeartPic = R"(          
 .------. 
 |A.--. | 
 | (\/) | 
 |  \/  | 
 | '--'A| 
 `------' 
          )";
const char* DiamondPic = R"(          
 .------. 
 |S.--. | 
 | :/\: | 
 | :\/: | 
 | '--'S| 
 `------' 
          )";
const char* SpadePic = R"(          
 .------. 
 |T.--. | 
 | :/\: | 
 | (__) | 
 | '--'T| 
 `------' 
          )";
const char* ClubPic = R"(          
 .------. 
 |R.--. | 
 | ()() | 
 | :(): | 
 | '--'R| 
 `------' 
          )";

std::string GetColorName(Color color)
{
	switch (color)
	{
	case Color::Heart:
		return "Heart";
	case Color::Diamond:
		return "Diamond";
	case Color::Spade:
		return "Spade";
	case Color::Club:
		return "Club";
	default:
		return "none";
	}
}


std::string GetValueName(Value value)
{
	switch (value)
	{
	case Value::Two:
		return "Two";
	case Value::Three:
		return "Three";
	case Value::Four:
		return "Four";
	case Value::Five:
		return "Five";
	case Value::Six:
		return "Six";
	case Value::Seven:
		return "Seven";
	case Value::Eight:
		return "Eight";
	case Value::Nine:
		return "Nine";
	case Value::Ten:
		return "Ten";
	case Value::Jack:
		return "Jack";
	case Value::Queen:
		return "Queen";
	case Value::King:
		return "King";
	case Value::Ace:
		return "Ace";
	default:
		return "none";
	}
}


std::string GetValueShortName(Value value)
{
	switch (value)
	{
	case Value::Two:
		return "2";
	case Value::Three:
		return "3";
	case Value::Four:
		return "4";
	case Value::Five:
		return "5";
	case Value::Six:
		return "6";
	case Value::Seven:
		return "7";
	case Value::Eight:
		return "8";
	case Value::Nine:
		return "9";
	case Value::Ten:
		return "10";
	case Value::Jack:
		return "J";
	case Value::Queen:
		return "Q";
	case Value::King:
		return "K";
	case Value::Ace:
		return "A";
	default:
		return "none";
	}
}

int GetColorPower(Color color)
{
	return (int)Color::Number - (int)color;
}

int GetValuePower(Value value)
{
	int StartValue = 2;
	int Result;
	switch (value)
	{
	case Value::Two:
	case Value::Three:
	case Value::Four:
	case Value::Five:
	case Value::Six:
	case Value::Seven:
	case Value::Eight:
	case Value::Nine:
	case Value::Ten:
		Result = (int)value + StartValue;
		break;
	case Value::Jack:
		Result = (int)value + StartValue + 2;
		break;
	case Value::Queen:
		Result = (int)value + StartValue + 4;
		break;
	case Value::King:
		Result = (int)value + StartValue + 6;
		break;
	case Value::Ace:
		Result = (int)value + StartValue + 8;
		break;
	default:
		Result = 0;
		break;
	}

	return Result;
}

int GetCardTotalPower(Value value, Color color)
{
	// odwracamy wagi, zebu Hearts mialo najwieksza wartosc
	return GetValuePower(value) * GetColorPower(color);
}


void TCard::UpdateDimensions()
{
	std::string CardPics[4] = { HeartPic, DiamondPic, SpadePic, ClubPic };
	int NLCount = 0;
	size_t Pos = 0;
	while (Pos != std::string::npos)
	{
		Pos = CardPics[(int)m_color].find('\n', Pos);
		if ((Pos != std::string::npos))
		{
			++NLCount;
			++Pos;
		}
	}
	Height = NLCount + 1;
	Pos = CardPics[(int)m_color].find('\n');
	if (Pos != std::string::npos)
	{
		Width = static_cast<int>(Pos);
	}
	else
	{
		Width = static_cast<int>(CardPics[(int)m_color].length());
	}
}

void TCard::Draw(TextEngine::TTerminalScreen* Screen)
{
	if (Scale >= 0.8)
	{
		std::string CardPics[4] = { HeartPic, DiamondPic, SpadePic, ClubPic };
		std::string CurrentPic = CardPics[(int)m_color];
		std::string val = GetValueShortName(m_value);
		CurrentPic.replace(24, val.length(), val);
		CurrentPic.replace(63 - val.length(), val.length(), val);
		Screen->DrawClippedMultilineText(static_cast<int>(X), static_cast<int>(Y), CurrentPic);
	}
	else if (Scale >= 0.6 && Scale < 0.8)
	{
		Screen->DrawClippedMultilineText(static_cast<int>(X), static_cast<int>(Y), "      \n **** \n **** \n **** \n **** \n      ");
	}
	else if (Scale >= 0.4 && Scale < 0.6)
	{
		Screen->DrawClippedMultilineText(static_cast<int>(X), static_cast<int>(Y), "     \n *** \n *** \n *** \n     ");
	}
	else if (Scale >= 0.2 && Scale < 0.4)
	{
		Screen->DrawClippedMultilineText(static_cast<int>(X), static_cast<int>(Y), "    \n ** \n ** \n    ");
	}
	else
	{
		Screen->DrawClippedMultilineText(static_cast<int>(X), static_cast<int>(Y), "*");
	}
}



void TDeck::Initialize()
{
	int index = 0;
	for (int i = 0; i < (int)Color::Number; ++i)
	{
		for (int j = 0; j < (int)Value::Number; ++j)
		{
			m_cards[index].m_color = (Color)i;
			m_cards[index].m_value = (Value)j;
			++index;
		}
	}

	m_size = index; 
}

void TDeck::Shuffle()
{
	for (int i = 0; i < m_size; ++i)
	{
		int index = rand() % m_size;
		TCard TmpCard = m_cards[index];
		m_cards[index] = m_cards[i];
		m_cards[i] = TmpCard;
	}
}
