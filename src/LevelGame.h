#pragma once
#include <string>
#include <vector>
#include "TextEngine/Level.h"
#include "Board.h"
#include "Player.h"
#include "Cards.h"
#include "Adventure.h"

class TextEngine::TEvent;
class TEnemy;

#define TURN_PLAYER 1
#define TURN_ENEMY 2
#define TURN_WINNER 3
#define TURN_IDLE -1

class TLevelGame : public TextEngine::TLevel
{
public:
	TLevelGame(TextEngine::TApplication* Application, int startSeed = -1, bool godMode = false);
	virtual ~TLevelGame();

	virtual int OnSizeChanged(TextEngine::TEvent* Event) override;
	//virtual int OnKeyDown(TEvent* Event) override { return 0; }
	virtual int OnUpdate(uint64_t LastFrameDuration) override;
	virtual int OnDraw() override;
	virtual int OnInput(TextEngine::TEvent* Event) override;
	virtual bool IsPlaying() const { return (CurrentPlayerTurn != TURN_IDLE && CurrentPlayerTurn != TURN_WINNER) || (AnimationSteps > 0); }
	virtual bool GetDoInput() const { return !IsPlaying(); }
protected:
	TBoard Board;
	TPlayer Player;
	TDeck Deck;
	TCard PlayerCard;
	TCard OponentCard;
	TAdventure Adventure;

	uint64_t AnimationDuration; // czas calej animacji w milisekundach
	uint64_t AnimationFrameDuration; // czas jednej klatki animacji w milisekundach
	int AnimationSteps;

	std::string BoardContent;
	std::vector<int> EmptyFields;
	std::vector<TEnemy*> Enemies;
	TextEngine::TWindow AdventureWindow;
	TextEngine::TWindow MsgWindow;
	// 1 - Tura gracza, 2 - Tura oponenta, -1 - Brak walki
	int CurrentPlayerTurn;
	float AnimationDx;
	float AnimationDy;
	float AnimationScaleDelta;

	int StartSeed;
	bool GodMode;

	int Lp; // przechowuje miejsce w top ten, ktore zajal zwycieski gracz, o ile dostal sie na to liste. Jesli nie to ma wartosc -1
	void DrawActionBox(int Left, int Top);
	void DrawMovementInformationInActionBox(int Left, int Top);
	void DrawCloseWindowInformationInActionBox(int Left, int Top);
	void DrawPickInformationInActionBox(int Left, int Top);
	void DrawTraderOptionsInActionBox(int Left, int Top);
	void DrawWinningInformation();
	void AddEmptyFieldForCurrentPlayerPosition();
	// Ponizsza funkcja zwraca indeks pierwszego pustego pola ktore znajduje sie na planszy na pozycji Pos lub zwraca indeks na ktorym powinno byc dodane puste pole.
	int GetIndexOfEmptyField(int Pos);
	inline bool IsPosInEmptyFields(int Pos);
	void AddEnemy(TEnemy* Enemy, int Position = -1);
	void AddEnemyForCurrentPlayerPosition(TEnemy* Enemy)
	{
		AddEnemy(Enemy, Player.PlayerPos);
	}
	// Ponizsza funkcja zwraca indeks pierwszego wroga ktory znajduje sie na planszy na pozycji Pos lub zwraca indeks na ktorym powinien on byc dodany.
	int GetIndexOfEnemy(int Pos);
	bool IsPosInEnemies(int Pos);
	TEnemy* GetEnemyAtPos(int Pos);
	void ShowTextWindow(const std::string& Text, TextEngine::TWindow* Window = nullptr);

	void OnLookAround();
	void GetTopCardFromDeck();
	void AnimateCard();
	void DoAdventure();
	void OnInputForMovementAction(TextEngine::TEvent* Event);
	void OnInputForEquipmentPickingAdventure(TextEngine::TEvent* Event);
	void OnInputForTrader(TextEngine::TEvent* Event);
	// Zwraca ilosc punktow wytrzymalosci dodanych po wygranej walce. Umozliwia to wyswietlenie stoswnego komunikatu.
	int FinishBattleTurns();
	inline void ZeroAdventureAndCloseWindow();
	void SetGameOver();
	void SetVictory();
	void UpdateStatsAfterClosingAdventureWindow();
};
