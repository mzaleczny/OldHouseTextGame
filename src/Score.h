#pragma once
#include <string>
#include <vector>


struct ScoreItem
{
	int Lp;
	int Points;
	std::string Name;
};

class TScore
{
public:
	TScore();
	~TScore();
	std::vector<ScoreItem> Items;
	std::string GetScoreTableDataAsString(int PositionHilited = -1);
	void Save();
	void Insert(int Points, std::string Name);
	int GetInsertionPosition(int Points);

private:
	std::string GetDataForItems();
};
