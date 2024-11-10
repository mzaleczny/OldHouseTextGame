#pragma once
#include "Common.h"
#include "Adventure.h"
#include "Player.h"
#include "Enemy.h"

float AdventurePercentages[] = {
	0.90f, // EAdventureType::EAT_NONE
	0.35f, // EAdventureType::EAT_ENEMY
	0.29f, // EAdventureType::EAT_EQUIPMENT_FOUND,
	0.23f, // EAdventureType::EAT_EQUIPMENT_UPGRADE,
	0.19f, // EAdventureType::EAT_STRENGTH_GROWTH,
	0.15f, // EAdventureType::EAT_ENDURANCE_GROWTH,
	0.05f, // EAdventureType::EAT_GOLD_GROWTH,
	0.00f  // EAdventureType::EAT_TRADER
};
EAdventureType AdventureForPercent[] = {
	EAdventureType::EAT_NONE,
	EAdventureType::EAT_ENEMY,
	EAdventureType::EAT_EQUIPMENT_FOUND,
	EAdventureType::EAT_EQUIPMENT_UPGRADE,
	EAdventureType::EAT_STRENGTH_GROWTH,
	EAdventureType::EAT_ENDURANCE_GROWTH,
	EAdventureType::EAT_GOLD_GROWTH,
	EAdventureType::EAT_TRADER
};

int TAdventure::AdventuresCounter = 0;

TAdventure::~TAdventure()
{
	if (Enemy)
	{
		delete Enemy;
		Enemy = nullptr;
	}
}

void TAdventure::Zero()
{
	Text = "";
	Type = EAdventureType::EAT_NONE;
	ResetStatsModifiers();
	if (Enemy)
	{
		delete Enemy;
		Enemy = nullptr;
	}
}

void TAdventure::ResetStatsModifiers()
{
	ExpAmount = 0;
	PointsAmount = 0;
	EnduranceAmount = 0;
	EquipmentModifierAmount = 0;
	StrengthAmount = 0;
	GoldAmount = 0;
}

void TAdventure::GetMessageAdventure(TPlayer* Player, TAdventure& a, const std::string& Message)
{
	a.Type = EAdventureType::EAT_PLAIN_MESSAGE;
	a.Text = Message;
}

void TAdventure::GetRandom(TPlayer* Player, TAdventure& a)
{
	static EAdventureType PreviousAdventureType = EAdventureType::EAT_NONE;
	EAdventureType AdventureType;
	a.Zero();

	TAdventure::AdventuresCounter++;
	if (TAdventure::AdventuresCounter == 1)
	{
		return TAdventure::GetRandomEquipment(Player, a);
	}

	float Percent;
	
	do
	{
		Percent = (rand() % 101) / 100.0f;
		for (int i = 0; i < 8; ++i)
		{
			if (Percent - 0.001 > AdventurePercentages[i])
			{
				AdventureType = AdventureForPercent[i];
				break;
			}
		}
	} while (AdventureType == PreviousAdventureType);

	PreviousAdventureType = AdventureType;
	switch (AdventureType)
	{
	case EAdventureType::EAT_NONE:
		a.Type = EAdventureType::EAT_NONE;
		a.Text = "Ten pokoj jest pusty.\n";
		break;
	case EAdventureType::EAT_EQUIPMENT_FOUND:
		TAdventure::GetRandomEquipment(Player, a);
		break;
	case EAdventureType::EAT_EQUIPMENT_UPGRADE:
		TAdventure::GetRandomEquipmentUpgrade(Player, a);
		break;
	case EAdventureType::EAT_STRENGTH_GROWTH:
		TAdventure::GetRandomStrengthGrow(Player, a);
		break;
	case EAdventureType::EAT_ENDURANCE_GROWTH:
		TAdventure::GetRandomEnduranceGrow(Player, a);
		break;
	case EAdventureType::EAT_GOLD_GROWTH:
		TAdventure::GetRandomGoldGrow(Player, a);
		break;
	case EAdventureType::EAT_TRADER:
		TAdventure::GetRandomTrader(Player, a);
		break;
	case EAdventureType::EAT_ENEMY:
	default:
		// Dla wrogow jako poprzednio przygode ustawiamy brak przygody, po to by zezwolic na wielokrotne losowanie wrogow pod rzad/ Pozostale przygody nie trafia sie pod rzad.
		PreviousAdventureType = EAdventureType::EAT_NONE;
		a.Type = EAdventureType::EAT_ENEMY;
		TAdventure::GetRandomEnemy(Player, a);
		break;
	}
}

std::string TAdventure::GetTextForEnemy(TEnemy* Enemy)
{
	std::string Text;
	EEnemyType EnemyType = Enemy->GetType();
	Text = std::string("Zaatakowal") + Enemy->FemaleSuffix() + " Cie ";
	Text += std::string(EnemyNames[(int)EnemyType]) + " '" + Enemy->GetAvatar() + "',  Sila: ";
	Text += std::to_string(Enemy->GetStrength()) + "\n";
	return Text;
}

void TAdventure::GetAdventureForEnemy(TEnemy* Enemy, TAdventure& a)
{
	a.Type = EAdventureType::EAT_ENEMY;
	a.Text = TAdventure::GetTextForEnemy(Enemy);
}

void TAdventure::GetRandomEnemy(TPlayer* Player, TAdventure& a)
{
	int PlayerPos = Player->PlayerPos;
	int x, y;
	Player->GetPlayerCoords(&x, &y);

	int EnemyType = ((min(x, y) / 2) + rand() % ((int)EEnemyType::EET_DRAGON % ((x+1)*(y+1)) + 1)) % ((int)EEnemyType::EET_DRAGON);
	int Strength;
	Strength = rand() % (static_cast<int>(x * y * 0.75) + 2) + (x+y)/2 + Player->GetStrength() / 5;
	if (Strength < 1) Strength = 1;

	a.Type = EAdventureType::EAT_ENEMY;
	a.Enemy = new (std::nothrow) TEnemy((EEnemyType)EnemyType, PlayerPos, Strength);
	a.Text = TAdventure::GetTextForEnemy(a.Enemy);
}

void TAdventure::GetRandomEquipment(TPlayer* Player, TAdventure& a)
{
	int PlayerPos = Player->PlayerPos;
	int x, y;
	Player->GetPlayerCoords(&x, &y);
	int WeaponType = static_cast<int>((float)(rand() % (static_cast<int>((x + 1) * (y + 1) * 0.5f) + 2) + (x + y) / 2 * Player->GetStrength() / 2) / 5.0f);
	WeaponType %= (int)EWeapon::Number;
	// pomijamy bron typu "Gole rece"
	if (WeaponType == 0) WeaponType = (rand() % 2) + 1;

	a.Weapon.BaseAttack = static_cast<int>((float)(rand() % (static_cast<int>((x+1) * (y+1) * 0.75f) + 2) + (x + y) / 2 * Player->GetStrength() / 2) / 2.0f) - Player->GetMovements();
	if (a.Weapon.BaseAttack <= 0) a.Weapon.BaseAttack = 1;
	a.Weapon.Modifier = 0;
	a.Weapon.Type = (EWeapon)WeaponType;
	a.Weapon.Cost = 1;

	a.Type = EAdventureType::EAT_EQUIPMENT_FOUND;
	a.Text = std::string("Znalazles bron ") + a.Weapon.GetName() + " (+" + std::to_string(a.Weapon.GetTotalAttack()) + ")\nCo robisz?\n";
}

void TAdventure::GetRandomEquipmentUpgrade(TPlayer* Player, TAdventure& a)
{
	const char* Upgrades[] = {
		"oselke", "male narzedzia rzemieslnicze", "narzedzia rzemieslnicze", "mistrzowskie narzedzia rzemieslnicze", "rune ziemi", "rune ksiezyca", "rune slonca",
		"rune powietrza", "rune wody", "rune ognia"
	};
	int size = sizeof(Upgrades) / sizeof(Upgrades[0]);
	int Co = rand() % size;
	if (Co == 0) Co = 1;
	int Modifier = rand() % Co + rand() % 3 + 1;
	
	a.Type = EAdventureType::EAT_EQUIPMENT_UPGRADE;
	a.Text = std::string("Znalazles ") + Upgrades[Co] + ".\n";
	if (Player->GetWeapon().Type != EWeapon::EW_NONE)
	{
		a.Text += "Twoja bron zadaje teraz +" + std::to_string(Modifier) + " wiecej obrazen.\n";
		a.EquipmentModifierAmount = Modifier;
	}
	else
	{
		a.Text += "Niestety nie masz broni, ktora moglbys ulepszyc.\n";
		a.EquipmentModifierAmount = 0;
	}
}

void TAdventure::GetRandomStrengthGrow(TPlayer* Player, TAdventure& a)
{
	const char* Places[] = {
		"Znalazles hantle i pocwiczyles troche.",
		"Znalazles Fontanne Sily i napiles sie z niej bez wahania.",
		"Natrafiles na ciezki teren, ktorego pokonanie\nkosztowalo Cie wiele wysilku.",
		"Kapliczke Mocy i pomodliles sie do starozytnego bostwa."
	};
	int size = sizeof(Places) / sizeof(Places[0]);
	int Co = rand() % size;
	if (Co == 0) Co = 1;
	int Modifier = rand() % Co + 1;

	a.Type = EAdventureType::EAT_STRENGTH_GROWTH;
	a.Text = std::string(Places[Co]) + "\nW efekcie Twoja sila wzrasta o +" + std::to_string(Modifier) + ".\n";
	a.StrengthAmount = Modifier;
}

void TAdventure::GetRandomEnduranceGrow(TPlayer* Player, TAdventure& a)
{
	const char* Places[] = {
		"Znalazles hantle i pocwiczyles troche.",
		"Znalazles Fontanne Wytrzymalosci i napiles sie z niej bez wahania.",
		"Natrafiles na ciezki teren, ktorego pokonanie\nkosztowalo Cie wiele wysilku.",
		"Kapliczke Wytrzymalosci i pomodliles sie do starozytnego bostwa."
	};
	int size = sizeof(Places) / sizeof(Places[0]);
	int Co = rand() % size;
	if (Co == 0) Co = 1;
	int Modifier = rand() % Co + 1;

	a.Type = EAdventureType::EAT_ENDURANCE_GROWTH;
	a.Text = std::string(Places[Co]) + "\nW efekcie Twoja wytrzymalosc wzrasta o +" + std::to_string(Modifier) + ".\n";
	a.EnduranceAmount = Modifier;
}

void TAdventure::GetRandomGoldGrow(TPlayer* Player, TAdventure& a)
{
	int PlayerPos = Player->PlayerPos;
	int x, y;
	Player->GetPlayerCoords(&x, &y);

	int Modifier = rand() % 5 + 1 + (x+1)*(y+1)/2;
	a.Type = EAdventureType::EAT_ENDURANCE_GROWTH;
	a.Text = std::string("Znalazles pewna ilosc mieszkow zlota: ") + std::to_string(Modifier) + "\n";
	a.GoldAmount = Modifier;
}

void TAdventure::GetRandomTrader(TPlayer* Player, TAdventure& a)
{
	int PlayerPos = Player->PlayerPos;
	int x, y;
	int ItemsCount = 0;
	Player->GetPlayerCoords(&x, &y);

	a.Text = "Spotkales kupca. Ma on w swojej ofercie\nnastepujace przedmioty:\n\n";
	for (int i = 1; i < (int)EWeapon::Number; i++)
	{
		int WeaponIndex = i - 1;
		EWeapon WeaponType = (EWeapon)i;

		a.WeaponsAtTrader[WeaponIndex].Type = WeaponType;
		a.WeaponsAtTrader[WeaponIndex].Modifier = 0;
		a.WeaponsAtTrader[WeaponIndex].BaseAttack = static_cast<int>((float)(rand() % (static_cast<int>((x + 1) * (y + 1) * 0.9f) + 2) + (x + y) / 2 * Player->GetStrength() / 2) / 2.0f) - Player->GetMovements();
		int Percent = rand() % 101;
		float PositionPercent = 100 * (PlayerPos / (float)((BoardWidth + 1) * (BoardHeight * 2)));
		if (Percent > ((int)EWeapon::Number - (int)WeaponType) * PositionPercent)
		{
			a.WeaponsAtTrader[WeaponIndex].BaseAttack = 0;
			a.WeaponsAtTrader[WeaponIndex].Cost = 0;
		}
		else
		{
			if (a.WeaponsAtTrader[WeaponIndex].BaseAttack < 5)
			{
				a.WeaponsAtTrader[WeaponIndex].BaseAttack = rand() % 5 + 1;
			}
		}
		if (a.WeaponsAtTrader[WeaponIndex].BaseAttack > 0)
		{
			if (rand() % 100 > 49)
			{
				a.WeaponsAtTrader[WeaponIndex].BaseAttack += rand() % (a.WeaponsAtTrader[WeaponIndex].BaseAttack / 2 + 1);
				
			}
			else
			{
				a.WeaponsAtTrader[WeaponIndex].BaseAttack -= rand() % (a.WeaponsAtTrader[WeaponIndex].BaseAttack / 2 + 1);
			}
			a.WeaponsAtTrader[WeaponIndex].Cost = a.WeaponsAtTrader[WeaponIndex].BaseAttack / 3 + 1;
			if (a.WeaponsAtTrader[WeaponIndex].BaseAttack > 0)
			{
				ItemsCount++;
				if (i < 10)
				{
					a.Text += "    ";
				}
				else
				{
					a.Text += "   ";
				}
				a.Text += std::to_string(i) + ". ";
				std::string Name = a.WeaponsAtTrader[WeaponIndex].GetName() + " (+" + std::to_string(a.WeaponsAtTrader[WeaponIndex].GetTotalAttack()) + ")";
				while (Name.length() < 25) Name += ' ';
				a.Text += Name + "Cena: " + std::to_string(a.WeaponsAtTrader[WeaponIndex].Cost) + "\n";
			}
			else
			{
				a.WeaponsAtTrader[WeaponIndex].BaseAttack = 0;
				a.WeaponsAtTrader[WeaponIndex].Cost = 0;
			}
		}
	}

	a.Type = EAdventureType::EAT_TRADER;
	if (ItemsCount == 0)
	{
		TAdventure::GetRandomTrader(Player, a);
	}
}
