#include "TextEngine/Application.h"
#include "TextEngine/Event.h"
#include "TextEngine/Level.h"
#include "TextEngine/Desktop.h"
#include <string>
#include "Common.h"
#include "LevelStartScreen.h"
#include "LevelGame.h"
#include "Score.h"


TLevelStartScreen::TLevelStartScreen(TextEngine::TApplication* Application, bool showTopTenOnStart)
	: TextEngine::TLevel(Application), StartSeed(-1), GodMode(false)
{
	//Pixels = new TextEngine::TPixels(ParentApplication->GetScreenBuffer(), Title, 0);
	if (showTopTenOnStart)
	{
		ShowTopTen();
	}
}

TLevelStartScreen::~TLevelStartScreen()
{
	(*ParentApplication->GetDesktop()) -= &Wnd;
}

int TLevelStartScreen::OnDraw()
{
	static TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();
	int Width = ParentApplication->GetScreenWidth();
	//if (Pixels)
	//{
	//	Pixels->Draw(false);
	//}
	Screen->Clear();

	int Left = (Width - static_cast<int>(TitleImage.find('\n'))) / 2;
	Screen->DrawClippedMultilineText(Left, 0, TitleImage);

	Left = 3;
	Screen->DrawClippedMultilineText(Left, 0, Title);

	Left = 3;
	Screen->DrawClippedMultilineText(Left, 10, TitleMenuEmptyBg);

	Left = 6;
	int y = 12;
	Screen->DrawText(Left, y, "Menu:");
	Screen->DrawText(Left, y + 2, "1. Rozpocznij nowa gre");
	Screen->DrawText(Left, y + 3, "2. Lista najlepszych");
	Screen->DrawText(Left, y + 4, "3. Zaslugi");
	Screen->DrawText(Left, y + 5, "Q. Opusc gre");

	if (GodMode)
	{
		Screen->DrawText(Left + 7, y, "GodMode=On");
	}
	if (StartSeed != -1)
	{
		Screen->DrawText(Left + 19, y, "Seed=" + std::to_string(StartSeed));
	}

	DoInput = true;
	XInput = Left;
	YInput = y + 7;
	if (ParentApplication->GetDesktop()->GetWindowsCount() == 0)
	{
		InputPrompt = "Wybierz [1,2,3,Q]: ";
	}
	else
	{
		InputPrompt = "Wybierz [Enter,Q]: ";
	}

	ParentApplication->GetDesktop()->Refresh();

	return 0;
}

int TLevelStartScreen::OnInput(TextEngine::TEvent* Event)
{
	if (Event->Input == "")
	{
		(*ParentApplication->GetDesktop()) -= &Wnd;
	}
	else if (Event->Input == "q" || Event->Input == "Q")
	{
		
		if (ParentApplication->GetDesktop()->Contain(&Wnd))
		{
			(*ParentApplication->GetDesktop()) -= &Wnd;
		}
		else
		{
			ParentApplication->QuitApplication();
		}
	}
	// Opcje menu sa dostepny tylko gdy nie ma otwartego okna
	if (ParentApplication->GetDesktop()->GetWindowsCount() == 0)
	{
		if (Event->Input == "1")
		{
			TLevelGame* Level = new (std::nothrow) TLevelGame(ParentApplication, StartSeed, GodMode);
			if (Level)
			{
				ParentApplication->OpenLevel(Level);
			}
		}
		else if (Event->Input == "2")
		{
			ShowTopTen();
		}
		else if (Event->Input == "3")
		{
			ShowCredits();
		}
		else if (Event->Input.substr(0, 5) == "Seed=")
		{
			StartSeed = std::atoi(Event->Input.substr(5).c_str());

		}
		else if (Event->Input.substr(0, 8) == "GodMode=")
		{
			GodMode = std::atoi(Event->Input.substr(8).c_str()) != 0;

		}
	}
	return 0;
}

void TLevelStartScreen::ShowCredits()
{
	Wnd.SetContent(ParentApplication->GetScreenBuffer(), 0, 0, ' ', " Zaslugi ",
		"Wykonal: Marcin Zaleczny\n"
		"Wykorzystane fragmenty kodu:\n"
		"  Jerzy Grebosz (Opus Magnum)\n"
		"  Maciej Celmer (Kurs Unreal Engine)\n"
		"\nNacisnij [Enter], aby zamknac to okno"
	);
	Wnd.SetPos(3, 10);
	Wnd.SetHeight(13);
	(*ParentApplication->GetDesktop()) += &Wnd;
}

void TLevelStartScreen::ShowTopTen()
{
	TScore Score;
	Wnd.SetContent(ParentApplication->GetScreenBuffer(), 0, 0, ' ', " Lista najlepszych ",
		Score.GetScoreTableDataAsString()
	);
	Wnd.SetPos(3, 5);
	Wnd.SetHeight(18);
	(*ParentApplication->GetDesktop()) += &Wnd;
}
