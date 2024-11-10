#pragma once

#include <string>
#include "TextEngine/Desktop.h"
#include "TextEngine/Application.h"
#include "LevelStartScreen.h"


enum class Color : uint8_t
{
	Heart = 0, // kier
	Diamond, // karo
	Spade, // pik
	Club, // trefl

	Number
};

enum class Value : uint8_t
{
	Two = 0,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	Ten,
	Jack,
	Queen,
	King,
	Ace,

	Number
};

std::string GetColorName(Color color);
std::string GetValueName(Value value);
std::string GetValueShortName(Value value);
int GetColorPower(Color color);
int GetValuePower(Value value);
int GetCardTotalPower(Value value, Color color);


class TextEngine::TTerminalScreen;

class TCard
{
public:
	friend class TDeck;

	TCard() : m_color(Color::Heart), m_value(Value::Two), Visible(false) {
		UpdateDimensions();
	}
	TCard(Color color, Value value) : m_color(color), m_value(value) {
		UpdateDimensions();
	}

	void Draw(TextEngine::TTerminalScreen* Screen);
	void SetPos(float x, float y) { X = x; Y = y; }
	void SetDestPos(float x, float y) { DestX = x; DestY = y; }
	void MoveBy(float dx, float dy) { X += dx; Y += dy; }
	void SetScale(float scale) { Scale = scale; }
	void AddScale(float scale) { Scale += scale; }
	void SetVisibility(bool visibility) { Visible = visibility; }
	bool IsVisible() const { return Visible; }
	float GetX() const { return X; }
	float GetY() const { return Y; }
	int GetWidth() const { return Width; }
	int GetHeight() const { return Height; }
	Value GetValue() const { return m_value; }
	Color GetColor() const { return m_color; }
	// operator>
	// operator<

private:
	float Scale; // [0.0f, 1.0f]
	float X, Y;
	int Width;
	int Height;
	float DestX, DestY;
	Color m_color;
	Value m_value;
	bool Visible;
	void UpdateDimensions();
};



class TDeck
{
public:
	TDeck()
	{
		Initialize();
	}

	void Initialize();
	void Shuffle();

	TCard Deal()
	{
		if (m_size == 0)
			return TCard();

		TCard result = m_cards[m_size - 1];
		m_size--;
		return result;
	}

	int GetSize() const
	{
		return m_size;
	}

private:
	TCard m_cards[52];
	int m_size = 0;
};
