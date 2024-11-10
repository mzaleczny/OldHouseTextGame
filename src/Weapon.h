#pragma once

#include <string>

enum class EWeapon
{
	EW_NONE,
	EW_FORK,
	EW_CLUB,
	EW_DAGGER,
	EW_SLING,
	EW_SHORT_SWORD,
	EW_SWORD,
	EW_BOW,
	EW_CROSSBOW,
	EW_AXE,
	EW_LONG_SWORD,
	Number
};

std::string GetWeaponName(EWeapon Type);

struct TWeapon
{
	int BaseAttack;
	int Modifier;
	int Cost;
	EWeapon Type;
	TWeapon() : BaseAttack(0), Modifier(0), Cost(0), Type(EWeapon::EW_NONE) {}
	std::string GetName() const
	{
		return GetWeaponName(Type);
	}
	int GetTotalAttack() const { return BaseAttack + Modifier; }
};
