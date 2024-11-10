#pragma once
#include <string>
#include "TextEngine/Desktop.h"
#include "Adventure.h"
#include "Weapon.h"

class TPlayer
{
public:
	int PlayerPos;
	TPlayer() : PlayerPos(0), Strength(4), Endurance(8), EnduranceUpgradesCounter(0), Gold(2), Experience(0), MaxExperience(7), Points(0), Movements(0), PrevRestAmount(0),
		EnduranceUpgradeCumulationAmount(0) {};
	void Draw(std::string& Destination) const;
	void DrawStats(TextEngine::TTerminalScreen* Destination) const;
	void GetPlayerCoords(int* x, int* y);

	int GetStrength() const { return Strength; }
	void SetStrength(int amount) { Strength = amount; }
	void AddToStrength(int amount) { Strength += amount; }

	int GetEndurance() const { return Endurance; }
	void SetEndurance(int amount) { Endurance = amount; }
	void AddToEndurance(int amount) { Endurance += amount; }
	void UpgradeEndurance();

	int GetGold() const { return Gold; }
	void SetGold(int amount) { Gold = amount; }
	void AddToGold(int amount) { Gold += amount; }

	int GetExperience() const { return Experience; }
	void SetExperience(int amount) { Experience = amount; }
	// zwraca ilosc punktow wytrzymalosci dodanych za doswiadczenie
	int AddToExperience(int amount);

	int GetPoints() const { return Points; }
	void SetPoints(int amount) { Points = amount; }
	void AddToPoints(int amount) { Points += amount; }

	TWeapon& GetWeapon() { return Weapon; }
	void SetWeapon(const TWeapon& weapon) {
		Weapon = weapon;
	}
	void IncMovements() { Movements++; }
	int GetMovements() const { return Movements; }

	void IncMaxExperience() { MaxExperience += 7; }
private:
	TWeapon Weapon;
	int Strength;
	int Endurance;
	int EnduranceUpgradesCounter;
	int Gold;
	int Experience;
	int MaxExperience;
	int Points;
	int Movements;
	int PrevRestAmount;
	// Przechowuje ilosc punktow wytrzymalosci, ktore nalezy dodac z odroczeniem - po zamknieciu okienka z informacja o zwiekszeniu wytrzymalosci
	int EnduranceUpgradeCumulationAmount;
};