#include "TextEngine/Application.h"
#include "TextEngine/Event.h"
#include "TextEngine/Level.h"
#include "TextEngine/Desktop.h"
#include <list>
#include <fstream>
#include "Common.h"
#include "LevelGame.h"
#include "LevelStartScreen.h"
#include "Adventure.h"
#include "Enemy.h"
#include "Score.h"

std::string Board;

TLevelGame::TLevelGame(TextEngine::TApplication* Application, int startSeed, bool godMode)
	: TextEngine::TLevel(Application), Board(0), CurrentPlayerTurn(TURN_IDLE), Lp(-1), StartSeed(startSeed), GodMode(godMode), AnimationSteps(0), AnimationFrameDuration(0)
{
	//static int Seed = 0;
	if (StartSeed == -1)
	{
		StartSeed = static_cast<unsigned int>(time(NULL));
	}
	//StartSeed = Seed;
	srand(StartSeed);
	//Seed++;

	// zapisujemy seeda do pliku, zeby w razie napotkania petli nieskonczonej moc zrekonstruowac i zdebugowac problem
	std::ofstream LogFile("output.log", std::ios::out | std::ios::trunc);
	LogFile << "StartSeed=" << StartSeed << std::endl;
	LogFile.close();
	Board.SetLineLength(BoardWidth);
	Board.Draw(BoardContent);
	// Na starcie player jest w lewym gornym rogu planszy
	Player.PlayerPos = Board.GetLineLength() + 1 + 1;
	Player.Draw(BoardContent);
	EmptyFields.reserve((BoardWidth / 2) * BoardHeight);
	// dodajemy puste pole dla biezacej poczatkowej pozycji gracza
	AddEmptyFieldForCurrentPlayerPosition();
	TEnemy *Dragon = new (std::nothrow) TEnemy((EEnemyType)EEnemyType::EET_DRAGON, (BoardWidth + 1) * ((BoardHeight - 1) * 2) - 3, 999);
	if (Dragon)
	{
		AddEnemy(Dragon);
	}

	Deck.Initialize();
	Deck.Shuffle();

	TAdventure::AdventuresCounter = 0;
	TAdventure::GetMessageAdventure(&Player, Adventure, "Information");
	ShowTextWindow(
		"Wlasnie wszedles do starego domu. Podjales sie zadania\n"
		"zabicia straszliwego smoka, ktory opanowal ta rezydencje.\n"
		"Otworzyl on piekielny portal i sprowadza niezliczone hordy\n"
		"wrogow, ktorzy zagrazaja porzadkowi tego swiata.\n"
	);
}

TLevelGame::~TLevelGame()
{
	(*ParentApplication->GetDesktop()) -= &AdventureWindow;
	(*ParentApplication->GetDesktop()) -= &MsgWindow;
	for (int i = 0; i < Enemies.size(); ++i)
	{
		delete Enemies[i];
		Enemies[i] = nullptr;
	}
	Enemies.clear();
}

int TLevelGame::OnSizeChanged(TextEngine::TEvent* Event)
{
	int WindowX = (ParentApplication->GetScreenWidth() - AdventureWindow.GetWidth()) / 2;
	int WindowY = 6;
	AdventureWindow.SetPos(WindowX, WindowY);

	float x, y = static_cast<float>(AdventureWindow.GetY() + 5);
	x = static_cast<float>(ParentApplication->GetScreenWidth() / 2 - 22);
	PlayerCard.SetPos(x, y);
	x = static_cast<float>(ParentApplication->GetScreenWidth() / 2 - 7);
	OponentCard.SetPos(x, y);
	return 0;
}

int TLevelGame::OnUpdate(uint64_t LastFrameDuration)
{
	static uint64_t TimePeriod = 0;
	TimePeriod += LastFrameDuration;

	if (AnimationSteps == 0)
	{
		if (CurrentPlayerTurn == TURN_PLAYER)
		{
			// Po zakonczeniu rozdawania karty dla gracza rozpoczynamy rozdawanie karty dla wroga
			CurrentPlayerTurn = TURN_ENEMY;
			GetTopCardFromDeck();
		}
		else if (CurrentPlayerTurn == TURN_ENEMY)
		{
			CurrentPlayerTurn = TURN_WINNER;
			TimePeriod = 0;
		}
		else
		{
			TimePeriod = 0;
		}
		return 0;
	}
	
	// Po uplywie okreslonego czasu w miliskeundach robimy nastepna klatke animacji
	if (TimePeriod > AnimationFrameDuration)
	{
		AnimationSteps--;
		TimePeriod = 0;
		AnimateCard();
	}

	return 0;
}

int TLevelGame::OnDraw()
{
	TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();
	int Width = ParentApplication->GetScreenWidth();
	int Height = ParentApplication->GetScreenHeight();
	int Left = 0;
	int y = 0;

	Screen->Clear();
	// jesli GameOver, to wypisujemy stosowny napis
	if (Player.GetEndurance() < 1)
	{
		Screen->DrawClippedMultilineText(Left, 0, GameOver);
		Screen->DrawText(0, Height - 4, "Niestety nie udalo Ci sie pokonac smoka. Sila wroga zwyciezyla i zalala swa masa caly swiat niosac zniszczenie i pozoge.");
		return 0;
	}
	// jesli Victory, to wypisujemy stosowny napis
	else if (Player.GetEndurance() == MAXINT32)
	{
		Screen->DrawClippedMultilineText(Left, 0, Victory);
		Screen->DrawText(0, Height - 4, "Udalo sie! Pokonales smoka i jego hordy. Swiat zostal uratowany a Ty bedziesz mial co opowiadac wnukom.");
		return 0;
	}

	Left = (Width - Board.GetLineLength()) / 2;
	// rysujemy puste pola na planszy
	for (int i = 0; i < EmptyFields.size(); ++i)
	{
		BoardContent[EmptyFields[i]] = ' ';
	}
	// rysujemy wrogow
	for (int i = 0; i < Enemies.size(); ++i)
	{
		Enemies[i]->Draw(BoardContent);
	}
	Player.Draw(BoardContent);
	Screen->DrawClippedMultilineText(Left, 2, BoardContent);
	Player.DrawStats(Screen);
	std::string Str = std::string("Karty: ") + std::to_string(Deck.GetSize()) + " [*]";
	Screen->DrawText(Width - static_cast<int>(Str.length()), 0, Str);
	y = BoardHeight * 2 + 1;
	DrawActionBox(Left, y);

	ParentApplication->GetDesktop()->Refresh();

	// Karty rysujemy na koncu, zeby byly nad opcjonalnymi okienkami
	if (PlayerCard.IsVisible())
	{
		PlayerCard.Draw(Screen);
	}
	if (OponentCard.IsVisible())
	{
		OponentCard.Draw(Screen);
	}

	if (CurrentPlayerTurn == TURN_WINNER)
	{
		DrawWinningInformation();
	}

	DoInput = true;
	YInput = y + 1;
	
	InputPrompt = "Wybieram: ";

	return 0;
}

int TLevelGame::OnInput(TextEngine::TEvent* Event)
{
	bool CanCloseAdventureWindow = Adventure.GetType() != EAdventureType::EAT_EQUIPMENT_FOUND && Adventure.GetType() != EAdventureType::EAT_TRADER;

	// Jesli koniec gry i gracz zdobytl odpowiednia ilosc punktow, to gracz musi podac swoja nazwe i nastepuje wyjscie
	if (Adventure.Text == "Finish" && Lp != -1)
	{
		std::string Input = Event->Input;
		// Trim Left spaces in place
		Input.erase(Input.begin(), std::find_if(Input.begin(), Input.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
		// Trim right spaces in place
		Input.erase(std::find_if(Input.rbegin(), Input.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), Input.end());
		if (Input.length() > 0)
		{
			TScore Score;
			Score.Insert(Player.GetPoints(), Input);
			Adventure.Zero();
			ZeroAdventureAndCloseWindow();
			return 0;
		}
	}

	// zamykamy okienko jesli nie jest wyswietlone okienko wymagajace podjecia akcji
	if (Event->Input == "" && CanCloseAdventureWindow)
	{
		if (FinishBattleTurns() < 1)
		{
			ZeroAdventureAndCloseWindow();
		}
	}
	else
	{
		if (Event->Input == "q" || Event->Input == "Q")
		{
			// jesli jest otwarte okno, to je zamykamy
			if (ParentApplication->GetDesktop()->GetWindowsCount() > 0)
			{
				// zamykamy okienko jesli nie jest wyswietlone okienko wymagajace podjecia akcji
				if (CanCloseAdventureWindow)
				{
					if (FinishBattleTurns() < 1)
					{
						ZeroAdventureAndCloseWindow();
					}
				}
			}
			else
			{
				// W przeciwnym razie wracamy do ekranu startowego
				TLevelStartScreen* Level = new TLevelStartScreen(ParentApplication);
				ParentApplication->OpenLevel(Level);
			}
		}
		if (ParentApplication->GetDesktop()->GetWindowsCount() == 0 /*Adventure.GetType() == EAdventureType::EAT_NONE*/)
		{
			OnInputForMovementAction(Event);
		}
		else if (Adventure.GetType() == EAdventureType::EAT_EQUIPMENT_FOUND)
		{
			OnInputForEquipmentPickingAdventure(Event);
		}
		else if (Adventure.GetType() == EAdventureType::EAT_TRADER)
		{
			OnInputForTrader(Event);
		}
	}
	return 0;
}

void TLevelGame::OnInputForMovementAction(TextEngine::TEvent* Event)
{
	if (Event->Input == "l" || Event->Input == "L")
	{
		if (BoardContent[Player.PlayerPos - 2] != '\n')
		{
			Player.PlayerPos -= 2;
			// Dodajemy biezaca pozycje gracza do listy pol odwiedzonych
			AddEmptyFieldForCurrentPlayerPosition();
			DoAdventure();
		}
	}
	else if (Event->Input == "p" || Event->Input == "P")
	{
		if (BoardContent[Player.PlayerPos + 2] != '\n')
		{
			Player.PlayerPos += 2;
			// Dodajemy biezaca pozycje gracza do listy pol odwiedzonych
			AddEmptyFieldForCurrentPlayerPosition();
			DoAdventure();
		}
	}
	else if (Event->Input == "g" || Event->Input == "G")
	{
		if (Player.PlayerPos - 2 * (BoardWidth + 1) >= 0)
		{
			Player.PlayerPos -= 2 * (BoardWidth + 1);
			// Dodajemy biezaca pozycje gracza do listy pol odwiedzonych
			AddEmptyFieldForCurrentPlayerPosition();
			DoAdventure();
		}
	}
	else if (Event->Input == "d" || Event->Input == "D")
	{
		if (Player.PlayerPos + 2 * (BoardWidth + 1) < BoardContent.length())
		{
			Player.PlayerPos += 2 * (BoardWidth + 1);
			// Dodajemy biezaca pozycje gracza do listy pol odwiedzonych
			AddEmptyFieldForCurrentPlayerPosition();
			DoAdventure();
		}
	}
	else if (Event->Input == "r" || Event->Input == "R")
	{
		OnLookAround();
	}
}

void TLevelGame::OnInputForEquipmentPickingAdventure(TextEngine::TEvent* Event)
{
	if (Event->Input == "w" || Event->Input == "W")
	{
		ZeroAdventureAndCloseWindow();
	}
	else if (Event->Input == "p" || Event->Input == "P")
	{
		Player.SetWeapon(Adventure.GetWeapon());
		ZeroAdventureAndCloseWindow();
	}
}

void TLevelGame::OnInputForTrader(TextEngine::TEvent* Event)
{
	if (Event->Input == "" || ParentApplication->GetDesktop()->GetWindowsCount() == 2)
	{
		if (Event->Input == "" || Event->Input == "q" || Event->Input == "Q")
		{
			*ParentApplication->GetDesktop() -= &MsgWindow;
		}
	}
	else if (Event->Input == "q" || Event->Input == "Q")
	{
		ZeroAdventureAndCloseWindow();
	}
	else if (!ParentApplication->GetDesktop()->Contain(&MsgWindow))
	{
		int Option = std::atoi(Event->Input.c_str());
		if (Option >= 1 && Option < (int)EWeapon::Number)
		{
			TWeapon w = Adventure.GetWeaponAtTrader(Option-1);
			if (w.Cost > 0 && Player.GetGold() >= w.Cost)
			{
				Player.SetWeapon(w);
				Player.AddToGold(-w.Cost);
				*ParentApplication->GetDesktop() -= &AdventureWindow;
				std::string Text = std::string("Kupiles ") + Player.GetWeapon().GetName() + " (+" + std::to_string(Player.GetWeapon().GetTotalAttack()) + ").\n";
				TAdventure::GetMessageAdventure(&Player, Adventure, Text);
				ShowTextWindow(Text);
			}
			else if (w.Cost > 0)
			{
				ShowTextWindow(
					"Nie masz wystarczajacej ilosci zlota, zeby kupic ta bron.\n"
					"\n"
					"Nacisnij [Enter], aby zamknac to okno",
					&MsgWindow);
			}
			else
			{
				ShowTextWindow(
					"Niepoprawny numer broni.\n"
					"\n"
					"Nacisnij [Enter], aby zamknac to okno",
					&MsgWindow);
			}
		}
		else
		{
			ShowTextWindow("Niepoprawny numer broni.\n"
				"\n"
				"Nacisnij [Enter], aby zamknac to okno",
				&MsgWindow);
		}
	}
}

void TLevelGame::DrawActionBox(int Left, int Top)
{
	switch (Adventure.GetType())
	{
	case EAdventureType::EAT_NONE:
	case EAdventureType::EAT_ENEMY:
		DrawMovementInformationInActionBox(Left, Top);
		break;
	case EAdventureType::EAT_PLAIN_MESSAGE:
		DrawCloseWindowInformationInActionBox(Left, Top);
		break;
	case EAdventureType::EAT_EQUIPMENT_FOUND:
		DrawPickInformationInActionBox(Left, Top);
		break;
	case EAdventureType::EAT_TRADER:
		DrawTraderOptionsInActionBox(Left, Top);
		break;
	}
}

void TLevelGame::DrawMovementInformationInActionBox(int Left, int Top)
{
	TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();

	int PlayerPos = Player.PlayerPos;

	std::string Actions;
	Actions.reserve(120);
	int y = Top;
	Actions += "Co robisz?  ";
	if (BoardContent[Player.PlayerPos - 2] != '\n')
	{
		Actions += "L. Idz w lewo | ";
	}
	if (BoardContent[Player.PlayerPos + 2] != '\n')
	{
		Actions += "P. Idz w prawo | ";
	}
	if (Player.PlayerPos - 2 * (BoardWidth + 1) >= 0)
	{
		Actions += "G. Idz do gory | ";
	}
	if (Player.PlayerPos + 2 * (BoardWidth + 1) < BoardContent.length())
	{
		Actions += "D. Idz na dol | ";
	}
	Actions += "R. Rozejrzyj sie | ";
	Actions += "Q. Wyjdz z gry";
	Left = (ParentApplication->GetScreenWidth() - static_cast<int>(Actions.length())) / 2;
	Screen->DrawText(Left, y, Actions);
	XInput = Left;
}

void TLevelGame::DrawCloseWindowInformationInActionBox(int Left, int Top)
{
	TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();

	int PlayerPos = Player.PlayerPos;

	std::string Actions;
	Actions.reserve(120);
	int y = Top;
	Actions += "Co robisz?  ";
	Actions += "Q. Zamknij okienko z informacja | ";
	Actions += "Enter. Zamknij okienko z informacja";
	Left = (ParentApplication->GetScreenWidth() - static_cast<int>(Actions.length())) / 2;
	Screen->DrawText(Left, y, Actions);
	XInput = Left;
}

void TLevelGame::DrawPickInformationInActionBox(int Left, int Top)
{
	TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();

	int PlayerPos = Player.PlayerPos;

	std::string Actions;
	Actions.reserve(120);
	int y = Top;
	Actions += "Co robisz?  ";
	Actions += "P. Podnies | ";
	Actions += "W. Wyrzuc";
	Left = (ParentApplication->GetScreenWidth() - static_cast<int>(Actions.length())) / 2;
	Screen->DrawText(Left, y, Actions);
	XInput = Left;
}

void TLevelGame::DrawTraderOptionsInActionBox(int Left, int Top)
{
	TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();

	int PlayerPos = Player.PlayerPos;

	std::string Actions;
	Actions.reserve(120);
	int y = Top;
	Actions += "Co robisz?  ";
	Actions += std::string("[1-") + std::to_string((int)EWeapon::Number) + "]. Kup wybrana bron  | ";
	Actions += "Q. Pozegnaj kupca";
	Left = (ParentApplication->GetScreenWidth() - static_cast<int>(Actions.length())) / 2;
	Screen->DrawText(Left, y, Actions);
	XInput = Left;
}

void TLevelGame::AddEmptyFieldForCurrentPlayerPosition()
{
	if (EmptyFields.size() == 0)
	{
		EmptyFields.push_back(Player.PlayerPos);
		return;
	}

	int Found = GetIndexOfEmptyField(Player.PlayerPos);
	auto it = EmptyFields.begin() + Found;
	if (it != EmptyFields.end() && *it == Player.PlayerPos)
		return;
	EmptyFields.insert(it, Player.PlayerPos);
}

int TLevelGame::GetIndexOfEmptyField(int Pos)
{
	if (EmptyFields.size() == 0)
	{
		return -1;
	}

	int Left = 0;
	int Right = static_cast<int>(EmptyFields.size()) - 1;
	int Mid;
	while (Left <= Right)
	{
		Mid = (Left + Right) / 2;
		if (EmptyFields[Mid] == Pos)
		{
			return Mid;
		}
		if (EmptyFields[Mid] < Pos) Left = Mid + 1;
		else Right = Mid - 1;
	}
	return Left;
}

bool TLevelGame::IsPosInEmptyFields(int Pos)
{
	int Indeks = GetIndexOfEmptyField(Pos);
	if (Indeks < EmptyFields.size() && EmptyFields[Indeks] == Pos)
	{
		return true;
	}
	return false;
}

void TLevelGame::AddEnemy(TEnemy* Enemy, int Pos)
{
	if (Enemies.size() == 0)
	{
		if (Pos != -1) Enemy->SetPos(Pos);
		Enemies.push_back(Enemy);
		return;
	}

	int Found = GetIndexOfEnemy(Pos);
	auto it = Enemies.begin() + Found;
	if (it != Enemies.end() && (*it)->GetPos() == Pos)
		return;
	if (Pos != -1) Enemy->SetPos(Pos);
	Enemies.insert(it, Enemy);
}

int TLevelGame::GetIndexOfEnemy(int Pos)
{
	if (Enemies.size() == 0)
	{
		return -1;
	}

	int Left = 0;
	int Right = static_cast<int>(Enemies.size()) - 1;
	int Mid;
	while (Left <= Right)
	{
		Mid = (Left + Right) / 2;
		if (Enemies[Mid]->GetPos() == Pos)
		{
			return Mid;
		}
		if (Enemies[Mid]->GetPos() < Pos) Left = Mid + 1;
		else Right = Mid - 1;
	}
	return Left;
}

bool TLevelGame::IsPosInEnemies(int Pos)
{
	int Indeks = GetIndexOfEnemy(Pos);
	if (Indeks < Enemies.size() && Enemies[Indeks]->GetPos() == Pos)
	{
		return true;
	}
	return false;
}

TEnemy* TLevelGame::GetEnemyAtPos(int Pos)
{
	int Indeks = GetIndexOfEnemy(Pos);
	if (Indeks < Enemies.size() && Enemies[Indeks]->GetPos() == Pos)
	{
		return Enemies[Indeks];
	}
	return nullptr;
}

void TLevelGame::ShowTextWindow(const std::string& Text, TextEngine::TWindow* Window)
{
	if (!Window)
	{
		Window = &AdventureWindow;
	}

	int NLCount = 0;
	size_t Pos = 0;
	while (Pos != std::string::npos)
	{
		Pos = Text.find('\n', Pos);
		if ((Pos != std::string::npos))
		{
			++NLCount;
			++Pos;
		}
	}
	int WindowHeight = NLCount + 2 /* dodatkowe dwie linie ponizej wlasciwego tekstu */ + 2 /* ramki gorna i dolna */ + 2 /* padding 1 od gory i 1 od dolu */;

	int IsFight = false;
	// Jesli to walka, to robimy miejsce na karty
	if (Text.find("Zaatakowal") != std::string::npos)
	{
		WindowHeight += 14;
		IsFight = true;
	}
	int PlayerX, PlayerY;
	Player.GetPlayerCoords(&PlayerX, &PlayerY);

	Window->SetHeight(WindowHeight);
	if (!IsFight && (Adventure.GetType() != EAdventureType::EAT_TRADER && Adventure.GetType() != EAdventureType::EAT_EQUIPMENT_FOUND))
	{
		Window->SetContent(ParentApplication->GetScreenBuffer(), 0, 0, ' ', " Przygoda ", Text + "\nNacisnij [Enter], aby zamknac to okno");
	}
	else if (Adventure.GetType() == EAdventureType::EAT_TRADER || Adventure.GetType() == EAdventureType::EAT_EQUIPMENT_FOUND)
	{
		Window->SetContent(ParentApplication->GetScreenBuffer(), 0, 0, ' ', " Przygoda ", Text);
		Window->SetHeight(WindowHeight - 2);
	}
	else
	{
		Window->SetContent(ParentApplication->GetScreenBuffer(), 0, 0, ' ', " Przygoda ", Text);
		Window->SetWidth(50);
	}

	int WindowX = (ParentApplication->GetScreenWidth() - Window->GetWidth()) / 2;
	int WindowY = 6;

	if (Window == &MsgWindow && ParentApplication->GetDesktop()->GetWindowsCount() == 2)
	{
		WindowX = AdventureWindow.GetX();
		WindowY = AdventureWindow.GetY();
		Window->SetWidth(AdventureWindow.GetWidth());
		Window->SetHeight(AdventureWindow.GetHeight());
	}
	Window->SetPos(WindowX, WindowY);
	(*ParentApplication->GetDesktop()) += Window;
}

void TLevelGame::DrawWinningInformation()
{
	TextEngine::TTerminalScreen* Screen = ParentApplication->GetScreenBuffer();
	TEnemy* Enemy = GetEnemyAtPos(Player.PlayerPos);

	int PaddingLeft = 2;
	int PlayerPower = Player.GetStrength() * GetCardTotalPower(PlayerCard.GetValue(), PlayerCard.GetColor()) + Player.GetWeapon().GetTotalAttack();
	int OponentPower = 0;
	if (Enemy)
	{
		OponentPower = Enemy->GetStrength() * GetCardTotalPower(OponentCard.GetValue(), OponentCard.GetColor());
	}
	Screen->DrawText(static_cast<int>(PlayerCard.GetX()) + 1, static_cast<int>(PlayerCard.GetY()) + PlayerCard.GetHeight() - 1, std::string("Atak: ") + std::to_string(PlayerPower));
	Screen->DrawText(static_cast<int>(OponentCard.GetX()) + 1, static_cast<int>(OponentCard.GetY()) + OponentCard.GetHeight() - 1, std::string("Atak: ") + std::to_string(OponentPower));
	Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, AdventureWindow.GetY() + AdventureWindow.GetHeight() - 2, "Nacisnij [Enter], aby zamknac to okno");
	Screen->DrawText(static_cast<int>(PlayerCard.GetX()) + 1, static_cast<int>(PlayerCard.GetY()), std::string("Gracz (") + GetColorName(PlayerCard.GetColor()) + ")");

	int InfoY = static_cast<int>(PlayerCard.GetY()) + 2;
	Screen->DrawText(AdventureWindow.GetX() + AdventureWindow.GetWidth() - 18, static_cast<int>(PlayerCard.GetY()) + 1, "Przeliczniki:");
	Screen->DrawText(AdventureWindow.GetX() + AdventureWindow.GetWidth() - 18, InfoY++, "Wartosc za kolor:");
	for (int i = 0; i < (int)Color::Number; ++i)
	{
		Screen->DrawText(AdventureWindow.GetX() + AdventureWindow.GetWidth() - 15, InfoY++, GetColorName((Color)i) + " (" + std::to_string(GetColorPower((Color)i)) + ")");
	}
	Screen->DrawText(AdventureWindow.GetX() + AdventureWindow.GetWidth() - 18, InfoY++, std::string("Wartosc kart"));
	Screen->DrawText(AdventureWindow.GetX() + AdventureWindow.GetWidth() - 16, InfoY++, std::string("gracza: ") + std::to_string(GetValuePower(PlayerCard.GetValue())));
	Screen->DrawText(AdventureWindow.GetX() + AdventureWindow.GetWidth() - 16, InfoY++, std::string("wroga: ") + std::to_string(GetValuePower(OponentCard.GetValue())));
	Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, InfoY, "Atak = Kolor * Wartosc karty * Sila");

	if (Enemy)
	{
		Screen->DrawText(static_cast<int>(OponentCard.GetX()) + 1, static_cast<int>(OponentCard.GetY()),
			std::string(Enemy->GetName()) + " (" + GetColorName(OponentCard.GetColor()) + ")"
		);
	}

	if (PlayerPower - OponentPower > 0)
	{
		int Exp = Enemy->GetStrength();
		int Points = Enemy->GetStrength() * ((int)Enemy->GetType() + 1);

		std::string ExperienceAmount = std::to_string(Exp);
		std::string ExperiencePunktyOdmiana = "punktow";
		if (ExperienceAmount == "1")
			ExperiencePunktyOdmiana = "punkt";
		else if (ExperienceAmount[ExperienceAmount.length() - 1] == '2' || ExperienceAmount[ExperienceAmount.length() - 1] == '3' || ExperienceAmount[ExperienceAmount.length() - 1] == '4')
			ExperiencePunktyOdmiana = "punkty";

		std::string PointsAmount = std::to_string(Points);
		std::string PointsPunktyOdmiana = "punktow";
		if (PointsPunktyOdmiana == "1")
			PointsPunktyOdmiana = "punkt";
		else if (PointsAmount[PointsAmount.length() - 1] == '2' || PointsAmount[PointsAmount.length() - 1] == '3' || PointsAmount[PointsAmount.length() - 1] == '4')
			PointsPunktyOdmiana = "punkty";

		Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, AdventureWindow.GetY() + AdventureWindow.GetHeight() - 5, "Zwyciezyl Gracz,  Zdobywasz:");
		Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, AdventureWindow.GetY() + AdventureWindow.GetHeight() - 4,
			ExperienceAmount + " " + ExperiencePunktyOdmiana + " doswiadczenia oraz " + PointsAmount + " " + PointsPunktyOdmiana);

		Adventure.setStatsOnBattleFinished(Exp, Points, 0);
	}
	else if (PlayerPower - OponentPower < 0)
	{
		if (Enemy)
		{
			Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, AdventureWindow.GetY() + AdventureWindow.GetHeight() - 5, std::string("Zwyciezyl") + Enemy->FemaleSuffix() + " " + Enemy->GetName());
		}
		Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, AdventureWindow.GetY() + AdventureWindow.GetHeight() - 4, "Tracisz punkt wytrzymalosci");
		Adventure.setStatsOnBattleFinished(0, 0, -1);
	}
	else
	{
		Screen->DrawText(AdventureWindow.GetX() + PaddingLeft, AdventureWindow.GetY() + AdventureWindow.GetHeight() - 5, "REMIS! Walka nie zostala rozstrzygnieta.");
		Adventure.setStatsOnBattleFinished(0, 0, 0);
	}
}

void TLevelGame::OnLookAround()
{
	std::string Text;
	TEnemy* Enemy = nullptr;
	if (Player.PlayerPos - 2 * (BoardWidth + 1) >= 0)
	{
		if (IsPosInEmptyFields(Player.PlayerPos - 2 * (BoardWidth + 1)))
		{
			Enemy = GetEnemyAtPos(Player.PlayerPos - 2 * (BoardWidth + 1));
			if (Enemy)
			{
				Text += std::string("U gory znajduje sie ") + EnemyNames[(int)Enemy->GetType()] + "  Sila: " + std::to_string(Enemy->GetStrength()) + ".\n";
			}
			else
			{
				Text += "U gory juz byles, ale w tym momencie nic tam nie widzisz.\n";
			}
		}
		else
		{
			Text += "U gory jeszcze nie byles, mozesz spodziewac sie wszystkiego.\n";
		}
	}
	if (BoardContent[Player.PlayerPos + 2] != '\n')
	{
		if (IsPosInEmptyFields(Player.PlayerPos + 2))
		{
			Enemy = GetEnemyAtPos(Player.PlayerPos + 2);
			if (Enemy)
			{
				Text += std::string("Na prawo znajduje sie ") + EnemyNames[(int)Enemy->GetType()] + "  Sila: " + std::to_string(Enemy->GetStrength()) + ".\n";
			}
			else
			{
				Text += "Na prawo juz byles, ale w tym momencie nic tam nie widzisz.\n";
			}
		}
		else
		{
			Text += "Na prawo jeszcze nie byles, mozesz spodziewac sie wszystkiego.\n";
		}
	}
	if (Player.PlayerPos + 2 * (BoardWidth + 1) < BoardContent.length())
	{
		if (IsPosInEmptyFields(Player.PlayerPos + 2 * (BoardWidth + 1)))
		{
			Enemy = GetEnemyAtPos(Player.PlayerPos + 2 * (BoardWidth + 1));
			if (Enemy)
			{
				Text += std::string("Na dole znajduje sie ") + EnemyNames[(int)Enemy->GetType()] + "  Sila: " + std::to_string(Enemy->GetStrength()) + ".\n";
			}
			else
			{
				Text += "Na dole juz byles, ale w tym momencie nic tam nie widzisz.\n";
			}
		}
		else
		{
			Text += "Na dole jeszcze nie byles, mozesz spodziewac sie wszystkiego.\n";
		}
	}
	if (BoardContent[Player.PlayerPos - 2] != '\n')
	{
		if (IsPosInEmptyFields(Player.PlayerPos - 2))
		{
			Enemy = GetEnemyAtPos(Player.PlayerPos - 2);
			if (Enemy)
			{
				Text += std::string("Na lewo znajduje sie ") + EnemyNames[(int)Enemy->GetType()] + "  Sila: " + std::to_string(Enemy->GetStrength()) + ".\n";
			}
			else
			{
				Text += "Na lewo juz byles, ale w tym momencie nic tam nie widzisz.\n";
			}
		}
		else
		{
			Text += "Na lewo jeszcze nie byles, mozesz spodziewac sie wszystkiego.\n";
		}
	}
	ShowTextWindow(Text);
}

void TLevelGame::GetTopCardFromDeck()
{
	float x = static_cast<float>(ParentApplication->GetScreenWidth());
	float y = static_cast<float>(0.0f);
	float DestX = 0.0f, DestY = static_cast<float>(AdventureWindow.GetY() + 5);
	AnimationDuration = 750;
	AnimationSteps = 18;
	AnimationFrameDuration = AnimationDuration / AnimationSteps;
	AnimationScaleDelta = 1.0f / AnimationSteps;

	if (CurrentPlayerTurn == TURN_PLAYER)
	{
		DestX = static_cast<float>(ParentApplication->GetScreenWidth() / 2 - 22);
		

		PlayerCard = Deck.Deal();
		PlayerCard.SetPos(x, y);
		PlayerCard.SetDestPos(DestX, DestY);
		PlayerCard.SetVisibility(true);
		PlayerCard.SetScale(0.0f);
	}
	else if (CurrentPlayerTurn == TURN_ENEMY)
	{
		DestX = static_cast<float>(ParentApplication->GetScreenWidth() / 2 - 7);
		
		OponentCard = Deck.Deal();
		OponentCard.SetPos(x, y);
		OponentCard.SetDestPos(DestX, DestY);
		OponentCard.SetVisibility(true);
		OponentCard.SetScale(0.0f);
	}
	// Jesli koniec talii, to losujemy nastepna
	if (Deck.GetSize() < 1)
	{
		Deck.Initialize();
		Deck.Shuffle();
	}
	AnimationDx = (DestX - x) / AnimationSteps;
	AnimationDy = (DestY - y) / AnimationSteps;
}

void TLevelGame::AnimateCard()
{
	if (CurrentPlayerTurn == TURN_PLAYER)
	{
		PlayerCard.MoveBy(AnimationDx, AnimationDy);
		PlayerCard.AddScale(AnimationScaleDelta);
	}
	else if (CurrentPlayerTurn == TURN_ENEMY)
	{
		OponentCard.MoveBy(AnimationDx, AnimationDy);
		OponentCard.AddScale(AnimationScaleDelta);
	}
}

void TLevelGame::DoAdventure()
{
	Player.IncMovements();
	TEnemy* Enemy = GetEnemyAtPos(Player.PlayerPos);
	// Ponizszy warunek jest prawdziwy gdy nie znaleziono wroga na tej pozycji
	if (!Enemy)
	{
		TAdventure::GetRandom(&Player, Adventure);
		if (Adventure.GetType() == EAdventureType::EAT_ENEMY)
		{
			Enemy = Adventure.GrabEnemy();
			AddEnemyForCurrentPlayerPosition(Enemy);
		}
	}
	else
	{
		TAdventure::GetAdventureForEnemy(Enemy, Adventure);
	}
	ShowTextWindow(Adventure.Text);
	// jesli to wrog, to uruchamiamy mechanizm walki z uzyciem kart
	if (Enemy)
	{
		// Ustawiamy typ przygody na bitwe, zeby wyswietlic poprawne opcje w Action Boksie. W tym momencie mozemyt to zrobic, poniewaz informacje z przygody odpowiadajacej
		// wrogowi zostaly juz wyswietlone w funkcji ShowTextWindow powyzej.
		TAdventure::GetMessageAdventure(&Player, Adventure, "Bitwa");
		CurrentPlayerTurn = TURN_PLAYER;
		GetTopCardFromDeck();
	}
}

int TLevelGame::FinishBattleTurns()
{
	// Jesli jestesmy w stanie prezentacji wynikow, to przy zmknieciu okinka zerujemy tryb walki na IDLE
	if (CurrentPlayerTurn == TURN_WINNER)
	{
		int EnduranceIncrease = 0;

		CurrentPlayerTurn = TURN_IDLE;
		PlayerCard.SetVisibility(false);
		OponentCard.SetVisibility(false);
		if (!GodMode)
		{
			EnduranceIncrease = Player.AddToExperience(Adventure.GetExperienceAmount());
			Player.AddToPoints(Adventure.GetPointsAmount());
			Player.AddToEndurance(Adventure.GetEnduranceAmount());
		}

		// jesli wygral gracz, to usuwamy wroga
		if (Adventure.GetExperienceAmount() > 0)
		{
			int Found = GetIndexOfEnemy(Player.PlayerPos);
			auto it = Enemies.begin() + Found;
			// jesli pokonano smoka, to wygrana i koniec gry
			bool GameWon = ((*it)->GetType() == EEnemyType::EET_DRAGON);
			Enemies.erase(it);

			// Zerujemy staty w przygodzie, zeby wytrzymalosc nie dodala/odjela sie dwa razy
			Adventure.ResetStatsModifiers();

			if (GameWon)
			{
				SetVictory();
				return 0;
			}

			if (EnduranceIncrease > 0)
			{
				(*ParentApplication->GetDesktop()) -= &AdventureWindow;
				ShowTextWindow("Dzieki zdobytemu doswiadczeniu Twoja wytrzymalosc oraz sila wzrastaja o " + std::to_string(EnduranceIncrease) + ".\n");
				TAdventure::GetMessageAdventure(&Player, Adventure, "Informacja");
			}
		}
		else if (Adventure.GetEnduranceAmount() < 0)
		{
			// Jesli gracz przegral, to GameOver
			if (Player.GetEndurance() < 1)
			{
				SetGameOver();
				return 0;
			}

			// Zerujemy staty w przygodzie, zeby wytrzymalosc nie dodala/odjela sie dwa razy
			Adventure.ResetStatsModifiers();
		}
		return EnduranceIncrease;
	}
	return 0;
}

void TLevelGame::ZeroAdventureAndCloseWindow()
{
	if (Adventure.Text != "Finish")
	{
		// Aktualizujemy zalegle staty z poprzednio wyswietlonej przygody. Sa one po dodaniu do statow gracza zerowane, wiec nie dodadza sie powtornie.
		UpdateStatsAfterClosingAdventureWindow();

		// Dodajemy opcjonalne zalegle punkty wytrzymalosci. Tutaj, bo tu okienko informujace o tym jestzamkniete. W przyszlosci zeby uniknac takich hakow, trzeba dorobic
		// okinka modalne, wstrzymujace gre do czasu ich zamkniecia.
		Player.UpgradeEndurance();

		(*ParentApplication->GetDesktop()) -= &AdventureWindow;
		// Jesli to koniec gry: GameOver lub Victory
		if (Player.GetEndurance() < 1 || Player.GetEndurance() == MAXINT32)
		{
			// to wracamy do ekranu startowego
			TLevelStartScreen* Level = new TLevelStartScreen(ParentApplication, Lp != -1);
			ParentApplication->OpenLevel(Level);
		}
	}
	if (Lp < 0)
	{
		Adventure.Zero();
	}
}

void TLevelGame::SetGameOver()
{
	Player.SetEndurance(0);
	TScore score;
	Lp = score.GetInsertionPosition(Player.GetPoints());
	if (Lp != -1)
	{
		InputPrompt = "Zdobyles " + std::to_string(Player.GetPoints()) + " punktow i zajales " + std::to_string(Lp + 1) + " miejsce. Podaj swoje imie: ";
	}
	else
	{
		InputPrompt = "Niestety nie dostales sie na liste zwyciezcow. Nacisnij [Enter], zeby kontynuowac: ";
	}
	(*ParentApplication->GetDesktop()) -= &AdventureWindow;
	TAdventure::GetMessageAdventure(&Player, Adventure, "Finish");
}

void TLevelGame::SetVictory()
{
	Player.SetEndurance(MAXINT32);
	TScore score;
	Lp = score.GetInsertionPosition(Player.GetPoints());
	if (Lp != -1)
	{
		InputPrompt = "Zdobyles " + std::to_string(Player.GetPoints()) + " punktow i zajales " + std::to_string(Lp+1) + " miejsce. Podaj swoje imie: ";
	}
	else
	{
		InputPrompt = "Niestety nie dostales sie na liste zwyciezcow. Nacisnij [Enter], zeby kontynuowac: ";
	}
	(*ParentApplication->GetDesktop()) -= &AdventureWindow;
	TAdventure::GetMessageAdventure(&Player, Adventure, "Finish");
}

void TLevelGame::UpdateStatsAfterClosingAdventureWindow()
{
	Player.AddToStrength(Adventure.GetStrengthAmount());
	Player.AddToEndurance(Adventure.GetEnduranceAmount());
	Player.AddToGold(Adventure.GetGoldAmount());
	if (Player.GetWeapon().Type != EWeapon::EW_NONE)
	{
		Player.GetWeapon().Modifier += Adventure.GetEquipmentModifierAmount();
	}
	Adventure.ResetStatsModifiers();
}
