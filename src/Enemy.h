#pragma once
#include <string>
#include "TextEngine/Desktop.h"


enum class EEnemyType
{
	EET_RAT,
	EET_SPIDER,
	EET_WILD_DOG,
	EET_WOLF,
	EET_GHOUL,
	EET_SKELETON,
	EET_UTOPIEC,
	EET_IMP,
	EET_NEKKER,
	EET_ZGNILEC,
	EET_POZERACZ_ZWLOK,
	EET_GOBLIN,
	EET_DWARF,
	EET_ORK,
	EET_TROLL,
	EET_OGR,
	EET_BABA_WODNA,
	EET_GHOST,
	EET_UPIOR,
	EET_POLUDNICA,
	EET_POLNOCNICA,
	EET_NIGHTMARE,
	EET_WAMPIR,
	EET_ENDRIAGA,
	EET_GRYF,
	EET_KUROLISZEK,
	EET_BAZYLISZEK,
	EET_IFRYT,
	EET_GOLEM,
	EET_CYCLOPE,
	EET_BEHEMOTH,
	EET_WIWERNA,
	EET_WIDLOGON,
	EET_DRAGON,
	Number
};

extern const char* EnemyNames[];

class TEnemy
{
public:
	TEnemy(EEnemyType EnemyType, int pos, int strength = -1);
	int GetPos() const { return Pos; }
	void SetPos(int Position) { Pos = Position; }
	void Draw(std::string& Destination) const;
	EEnemyType GetType() const { return Type; }
	void SetStrength(int strength) { Strength = strength; }
	int GetStrength() const { return Strength; }
	const char* GetName() const { return EnemyNames[(int)Type]; }
	char GetAvatar() const { return Avatar; }
	const char* FemaleSuffix() const
	{
		return (Type == EEnemyType::EET_BABA_WODNA || Type == EEnemyType::EET_POLUDNICA || Type == EEnemyType::EET_POLNOCNICA || Type == EEnemyType::EET_ENDRIAGA ||
			Type == EEnemyType::EET_WIWERNA) ? "a" : "";
	}
private:
	std::string Name;
	int Strength;
	char Avatar;
	int Pos;
	EEnemyType Type;
};
