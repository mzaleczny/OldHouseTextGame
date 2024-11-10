#include "Desktop.h"
#include "Application.h"
#include "Event.h"
#include "Level.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <chrono>

TextEngine::TApplication::TApplication()
	: ScreenBuffer(nullptr), Desktop(nullptr), CurrentLevel(nullptr), LevelMarkedForDeletion(nullptr), Quit(false)
{
	if (UpdateTerminalDimensions())
	{
		ScreenBuffer = new (std::nothrow) TextEngine::TTerminalScreen();
		if (ScreenBuffer)
		{
			Desktop = new (std::nothrow) TextEngine::TDesktop(ScreenBuffer);
		}
	}
}

TextEngine::TApplication::~TApplication()
{
	// Levele musza byc usuniete przed ScreenBufferem i Desktopem, bo inaczej moze sie zdarzyc null pointer exception
	if (CurrentLevel)
	{
		delete CurrentLevel;
		CurrentLevel = nullptr;
	}
	if (LevelMarkedForDeletion)
	{
		delete LevelMarkedForDeletion;
		LevelMarkedForDeletion = nullptr;
	}
	if (ScreenBuffer)
	{
		delete ScreenBuffer;
		ScreenBuffer = nullptr;
	}
	if (Desktop)
	{
		delete Desktop;
		Desktop = nullptr;
	}
}

bool TextEngine::TApplication::UpdateTerminalDimensions()
{
	int PrevHeight = ScreenHeight;
	int PrevWidth = ScreenWidth;

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	ScreenWidth = info.srWindow.Right - info.srWindow.Left + 1;
	ScreenHeight = info.srWindow.Bottom - info.srWindow.Top + 1;
	//std::cout << columns << 'x' << rows << std::endl;
	if (ScreenHeight == ScreenHeight && PrevWidth == ScreenWidth)
	{
		return false;
	}
	return true;
}

uint64_t TextEngine::TApplication::TimeSinceEpochInMillisec()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void TextEngine::TApplication::ProcessEvent(TEvent* Event, TLevel* Level)
{
	switch (Event->Type)
	{
	case TEvent::EEventType::EET_SIZE_CHANGED:
		ScreenWidth = Event->Width;
		ScreenHeight = Event->Height;
		ScreenBuffer->Resize(ScreenWidth, ScreenHeight);
		Level->OnSizeChanged(Event);
		break;
	case TEvent::EEventType::EET_INPUT:
		Level->OnInput(Event);
		break;
	}
}

void TextEngine::TApplication::Run()
{
	TEvent Event;
	LastFrameTime = 0;
	uint64_t CurrentTime = 0;
	while (!Quit && CurrentLevel)
	{
		CurrentTime = TimeSinceEpochInMillisec();
		if (PollEvent(&Event))
		{
			ProcessEvent(&Event, CurrentLevel);
		}
		// Jesli w handlerach zdarzen zmieniono lewel, to usuwamy poprzedni zaznaczony do usuniecia
		if (LevelMarkedForDeletion)
		{
			delete LevelMarkedForDeletion;
			LevelMarkedForDeletion = nullptr;
		}

		CurrentLevel->OnUpdate(LastFrameTime);
		CurrentLevel->OnDraw();

		ScreenBuffer->PresentScreen();

		LastFrameTime = TimeSinceEpochInMillisec() - CurrentTime;
	}
	ScreenBuffer->Clear();
	ScreenBuffer->PresentScreen();
}

void TextEngine::TApplication::OpenLevel(TLevel* Level)
{
	if (Level)
	{
		LevelMarkedForDeletion = CurrentLevel;
		CurrentLevel = Level;
	}
}

bool TextEngine::TApplication::PollEvent(TEvent* Event)
{
	Event->Type = TEvent::EEventType::EET_NONE;
	Event->PrevHeight = ScreenHeight;
	Event->PrevWidth = ScreenWidth;
	if (UpdateTerminalDimensions())
	{
		Event->Type = TEvent::EEventType::EET_SIZE_CHANGED;
		Event->Height = ScreenHeight;
		Event->Width = ScreenWidth;
		return true;
	}
	else
	{
		if (CurrentLevel->GetDoInput())
		{
			COORD coord;
			coord.X = CurrentLevel->GetXInput();
			coord.Y = CurrentLevel->GetYInput();
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
			std::cout << CurrentLevel->GetInputPrompt();
			std::getline(std::cin, Event->Input);
			Event->Type = TEvent::EEventType::EET_INPUT;
			return true;
		}
	}
	return false;
}


std::string TextEngine::TApplication::GetLineFromCin()
{
	std::string line;
	std::getline(std::cin, line);
	return line;
}
