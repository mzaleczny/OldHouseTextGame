#pragma once
#include <string>
#include "TextEngine/Desktop.h"
#include "Weapon.h"

enum class EAdventureType
{
	EAT_NONE,
	EAT_PLAIN_MESSAGE,
	EAT_ENEMY,
	EAT_EQUIPMENT_FOUND,
	EAT_EQUIPMENT_UPGRADE,
	EAT_STRENGTH_GROWTH,
	EAT_ENDURANCE_GROWTH,
	EAT_GOLD_GROWTH,
	EAT_TRADER,
	Number
};


extern EAdventureType Adventures[];


class TPlayer;
class TEnemy;

class TAdventure
{
public:
	std::string Text;
	static int AdventuresCounter;

	TAdventure() : Enemy(nullptr), ExpAmount(0), PointsAmount(0), EnduranceAmount(0), EquipmentModifierAmount(0), StrengthAmount(0), GoldAmount(0) {}
	~TAdventure();
	void Zero();
	EAdventureType GetType() const { return Type; }
	void setStatsOnBattleFinished(int expAmount, int pointsAmount, int enduranceAmount)
	{
		ExpAmount = expAmount;
		PointsAmount = pointsAmount;
		EnduranceAmount = enduranceAmount;
	}
	int GetExperienceAmount() const { return ExpAmount; }
	int GetPointsAmount() const { return PointsAmount; }
	int GetEnduranceAmount() const { return EnduranceAmount; }
	int GetEquipmentModifierAmount() const { return EquipmentModifierAmount; }
	int GetStrengthAmount() const { return StrengthAmount; }
	int GetGoldAmount() const { return GoldAmount; }
	// Ponizsza funkcja sluzy przeniesieniu utworzonego wroga do innego wektopra, ktory pozniej go usunie
	TEnemy* GrabEnemy()
	{
		TEnemy* enemy = Enemy;
		Enemy = nullptr;
		return enemy;
	}
	TWeapon GetWeapon() { return Weapon; }
	TWeapon GetWeaponAtTrader(int i) { return WeaponsAtTrader[i]; }
	void ResetStatsModifiers();
	static void GetMessageAdventure(TPlayer* Player, TAdventure& a, const std::string& Message);
	static void GetRandom(TPlayer* Player, TAdventure& a);
	static void GetAdventureForEnemy(TEnemy* Enemy, TAdventure& a);

private:
	TEnemy* Enemy;
	TWeapon Weapon;
	TWeapon WeaponsAtTrader[(int)EWeapon::Number - 1]; // o jeden mniej bo EWeapon::EW_NONE oznacza gole piesci czyli brak broni
	EAdventureType Type;
	// ExpAmount, PointsAmount i EnduranceAmount zawieraja wartosci o ktore nalezy zmienic staty gracza po zamknieciu okienka z informacja o przeprowadzonej walce
	int ExpAmount;
	int PointsAmount;
	int EnduranceAmount;
	// Ponizej sa wspolczynniki wzrostu statow gracza po napotkaniu odpowiedniej przygody
	int EquipmentModifierAmount;
	int StrengthAmount;
	int GoldAmount;
	static std::string GetTextForEnemy(TEnemy* Enemy);
	static void GetRandomEnemy(TPlayer* Player, TAdventure& a);
	static void GetRandomEquipment(TPlayer* Player, TAdventure& a);
	static void GetRandomEquipmentUpgrade(TPlayer* Player, TAdventure& a);
	static void GetRandomStrengthGrow(TPlayer* Player, TAdventure& a);
	static void GetRandomEnduranceGrow(TPlayer* Player, TAdventure& a);
	static void GetRandomGoldGrow(TPlayer* Player, TAdventure& a);
	static void GetRandomTrader(TPlayer* Player, TAdventure& a);
};
