#include "Weapon.h"

std::string GetWeaponName(EWeapon Type)
{
	switch (Type)
	{
	case EWeapon::EW_FORK:
		return "Widelec";
	case EWeapon::EW_CLUB:
		return "Palka";
	case EWeapon::EW_DAGGER:
		return "Sztylet";
	case EWeapon::EW_SLING:
		return "Proca";
	case EWeapon::EW_SHORT_SWORD:
		return "Krotki miecz";
	case EWeapon::EW_SWORD:
		return "Miecz";
	case EWeapon::EW_BOW:
		return "Luk";
	case EWeapon::EW_CROSSBOW:
		return "Kusza";
	case EWeapon::EW_AXE:
		return "Topor";
	case EWeapon::EW_LONG_SWORD:
		return "Dlugi miecz";
	case EWeapon::EW_NONE:
	default:
		return "Gole rece";
	};
};
