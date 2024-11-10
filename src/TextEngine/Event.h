#pragma once
#include <string>

namespace TextEngine
{
	class TEvent
	{
	public:
		enum class EEventType
		{
			EET_NONE,
			EET_SIZE_CHANGED,
			EET_INPUT,
			Number
		};
		EEventType Type;
		int PrevWidth;
		int Width;
		int PrevHeight;
		int Height;
		std::string Input;
	};
}
