#include "Common.h"
#include "Enemy.h"

const char* EnemyNames[] = { "Szczur", "Pajak", "Dziki Pies", "Wilk", "Ghoul", "Szkielet", "Utopiec", "Imp", "Nekker", "Zgnilec", "Pozeracz Zwlok", "Goblin", "Krasnolud",
							 "Ork", "Troll", "Ogr", "Baba Wodna", "Duch", "Upior", "Poludnica", "Polnocnica", "Koszmar", "Wampir", "Endriaga", "Gryf", "Kuroliszek", "Bazyliszek",
							 "Ifryt", "Golem", "Cyklop", "Behemot", "Wiwerna", "Widlogon", "Smok"
};

TEnemy::TEnemy(EEnemyType EnemyType, int pos, int strength) : Type(EnemyType)
{
	Pos = pos;
	Strength = strength;
	if (strength == -1)
	{
		Strength = (int)EnemyType + rand() % Pos - Pos / 2;
	}
	Name = EnemyNames[(int)EnemyType];
	switch (EnemyType)
	{
	case EEnemyType::EET_RAT:
		Avatar = 'R';
		break;
	case EEnemyType::EET_SPIDER:
		Avatar = 'S';
		break;
	case EEnemyType::EET_WILD_DOG:
		Avatar = 'd';
		break;
	case EEnemyType::EET_WOLF:
		Avatar = 'W';
		break;
	case EEnemyType::EET_GHOUL:
		Avatar = 'g';
		break;
	case EEnemyType::EET_SKELETON:
		Avatar = 'K';
		break;
	case EEnemyType::EET_UTOPIEC:
		Avatar = 'U';
		break;
	case EEnemyType::EET_IMP:
		Avatar = 'I';
		break;
	case EEnemyType::EET_NEKKER:
		Avatar = 'N';
		break;
	case EEnemyType::EET_ZGNILEC:
		Avatar = 'Z';
		break;
	case EEnemyType::EET_POZERACZ_ZWLOK:
		Avatar = 'C';
		break;
	case EEnemyType::EET_GOBLIN:
		Avatar = 'G';
		break;
	case EEnemyType::EET_DWARF:
		Avatar = 'D';
		break;
	case EEnemyType::EET_ORK:
		Avatar = 'o';
		break;
	case EEnemyType::EET_TROLL:
		Avatar = 'T';
		break;
	case EEnemyType::EET_OGR:
		Avatar = 'O';
		break;
	case EEnemyType::EET_BABA_WODNA:
		Avatar = 'B';
		break;
	case EEnemyType::EET_GHOST:
		Avatar = 'H';
		break;
	case EEnemyType::EET_UPIOR:
		Avatar = 'U';
		break;
	case EEnemyType::EET_POLUDNICA:
		Avatar = '@';
		break;
	case EEnemyType::EET_POLNOCNICA:
		Avatar = '#';
		break;
	case EEnemyType::EET_NIGHTMARE:
		Avatar = '$';
		break;
	case EEnemyType::EET_WAMPIR:
		Avatar = '%';
		break;
	case EEnemyType::EET_ENDRIAGA:
		Avatar = 'E';
		break;
	case EEnemyType::EET_GRYF:
		Avatar = '(';
		break;
	case EEnemyType::EET_KUROLISZEK:
		Avatar = ')';
		break;
	case EEnemyType::EET_BAZYLISZEK:
		Avatar = '=';
		break;
	case EEnemyType::EET_IFRYT:
		Avatar = 'I';
		break;
	case EEnemyType::EET_GOLEM:
		Avatar = '^';
		break;
	case EEnemyType::EET_CYCLOPE:
		Avatar = '&';
		break;
	case EEnemyType::EET_BEHEMOTH:
		Avatar = '*';
		break;
	case EEnemyType::EET_WIWERNA:
		Avatar = '{';
		break;
	case EEnemyType::EET_WIDLOGON:
		Avatar = '}';
		break;
	case EEnemyType::EET_DRAGON:
		Avatar = '!';
		break;
	}
}

void TEnemy::Draw(std::string& Destination) const
{
	Destination[Pos] = Avatar;
}
