#include "Common.h"
#include "Player.h"
#include "Weapon.h"

void TPlayer::Draw(std::string& Destination) const
{
	Destination[PlayerPos] = PLAYER_CHAR;
}

void TPlayer::DrawStats(TextEngine::TTerminalScreen* Destination) const
{
	Destination->DrawText(0, 0, "Sila: " + std::to_string(Strength) + "   Wytrz: " + std::to_string(Endurance) + "   Zloto: " + std::to_string(Gold) +
		"   Dosw: " + std::to_string(Experience) + "   Punkty: " + std::to_string(Points) + "   Bron: " + Weapon.GetName() + " (+" +
		std::to_string(Weapon.GetTotalAttack()) + ")  Up: " + std::to_string(PrevRestAmount) + "/" + std::to_string(MaxExperience));
	Destination->DrawText(0, 1, std::string(Destination->Width, F_H));
}

void TPlayer::GetPlayerCoords(int* x, int* y)
{
	// bo dodajemy znak nowej linii do kazdej linii planszy
	*x = (PlayerPos % (BoardWidth + 1)) / 2;
	// bo co drugie pole to ramka
	*y = (PlayerPos / (BoardWidth + 1)) / 2;
}

void TPlayer::UpgradeEndurance()
{
	// Przy okazji wzrostu wytrzymalosci wzrasta takze sila o tyle samo
	Strength += EnduranceUpgradeCumulationAmount;
	Endurance += EnduranceUpgradeCumulationAmount;
	EnduranceUpgradesCounter += EnduranceUpgradeCumulationAmount;
	EnduranceUpgradeCumulationAmount = 0;
	while (EnduranceUpgradesCounter - 10 > 10)
	{
		MaxExperience += 1;
		EnduranceUpgradesCounter -= 10;
	}
}

int TPlayer::AddToExperience(int amount)
{
	int EnduranceIncrease = 0;

	if (amount == 0) return 0;

	Experience += amount;
	PrevRestAmount += amount;
	while (PrevRestAmount >= MaxExperience)
	{
		EnduranceUpgradeCumulationAmount++;
		EnduranceIncrease++;
		PrevRestAmount -= MaxExperience;
	}

	return EnduranceIncrease;
}
