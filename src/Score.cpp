#include "score.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

void Explode(std::string ExplodeString, std::string String, std::vector<std::string>& Items)
{
    std::string CurrentString = "";
    Items.clear();
    size_t Pos = 0;
    size_t CurrentPos = 0;
    while ((Pos = String.find(ExplodeString.c_str(), CurrentPos)) != std::string::npos)
    {
        CurrentString = String.substr(CurrentPos, Pos - CurrentPos);
        Items.push_back(CurrentString);
        CurrentPos = Pos + 1;
    }
    if (CurrentPos < String.length())
    {
        Items.push_back(String.substr(CurrentPos));
    }
}

TScore::TScore()
{
    std::string DataFile = ".\\score.dat";
    char* DataBuffer = nullptr;
    std::string Data;
    std::fstream File(DataFile, std::ios::in | std::ios::beg | std::ios::binary);
    size_t DataBufferSize;

    if (File)
    {
        File.seekg(0, std::ios_base::end);
        DataBufferSize = File.tellg();
        File.seekg(0, std::ios_base::beg);
        DataBuffer = new (std::nothrow) char[DataBufferSize];
        if (DataBuffer)
        {
            File.read(DataBuffer, DataBufferSize);
            Data.assign(DataBuffer);
            delete[] DataBuffer;
            DataBuffer = nullptr;
        }
        File.close();
    }

    if (Data.length() == 0)
    {
        const char* MaleNames[] = { "Marcin", "Pawel", "Piotr", "Kacper", "Tomasz", "Michal", "Lukasz", "Jan", "Wladyslaw", "Stanislaw", "Bogdan", "Mateusz", "Arkadiusz", "Wojciech", "Grzegorz", "Janusz", "Jezy", "Jarek", "Rafal", "Jaroslaw", "Mariusz", "Emil", "Zenon", "Kazimierz"};
        const char* FemaleNames[] = { "Agnieszka", "Alicja", "Agata", "Patrycja", "Aleksandra", "Joanna", "Malgorzata", "Alina", "Ewa", "Marzena", "Lucja", "Dominika", "Marta", "Maria", "Edyta", "Anna", "Hanna", "Natalia", "Krystyna", "Kazimiera", "Wladyslawa", "Weronika", "Ula", "Aleksandra"};
        const char** Names[] = { MaleNames, FemaleNames };

        for (int i = 1; i <= 10; i++)
        {
            int Male = rand() % 2;
            Data += std::to_string(i) + "|1|";
            Data += std::string(Names[Male][rand() % 24]) + "\r\n";
        }
        File.open(DataFile, std::ios::out | std::ios::trunc | std::ios::binary);
        File.write(Data.data(), Data.size());
        File.close();
    }

    std::vector<std::string> Lines;
    std::vector<std::string> Fields;
    ScoreItem Item;
    Explode("\r\n", Data, Lines);
    for (int i = 0; i < Lines.size() && i < 10; i++)
    {
        Explode("|", Lines[i], Fields);
        Item.Lp = std::stoi(Fields[0]);
        Item.Points = std::stoi(Fields[1]);
        Item.Name = Fields[2];
        Items.push_back(Item);
    }
}

TScore::~TScore()
{
}

std::string TScore::GetScoreTableDataAsString(int PositionHilited)
{
    std::string Data, Name;
    std::stringstream oss;

    oss << std::setw(4) << std::left << "Lp"
        << std::setw(26) << std::left << "Kto"
        << std::setw(10) << std::left << "Punkty"
        << std::endl;
    Data = oss.str();
    oss.str("");
    oss.clear();

    for (int i = 0; i < Items.size() && i < 10; i++)
    {
        if (PositionHilited == i)
        {
            Name = "** " + Items[i].Name + " **";
        }
        else
        {
            Name = Items[i].Name;
        }
        oss << std::setw(4) << std::left << Items[i].Lp
            << std::setw(26) << std::left << Name
            << std::setw(10) << std::left << Items[i].Points
            << std::endl;
        Data += oss.str();
        oss.str("");
        oss.clear();
    }

    return Data;
}

void TScore::Save()
{
    std::string DataFile = ".\\score.dat";
    std::string Data = GetDataForItems();
    std::ofstream File;
    File.open(DataFile, std::ios::beg | std::ios::trunc | std::ios::binary);
    File.write(Data.data(), Data.size());
    File.close();
}

void TScore::Insert(int Points, std::string Name)
{
    int Index = GetInsertionPosition(Points);
    if (Index > -1)
    {
        ScoreItem Item{ Index+1, Points, Name.substr(0, 25) };
        // przesuwamy wszystkich o jedno pozycje w dol
        for (int i = 9; i > Index; i--)
        {
            Items[i] = Items[static_cast<size_t>(i - 1)];
            Items[i].Lp = (i+1);
        }
        std::vector<ScoreItem>::iterator it = Items.begin() + Index;
        Items[Index] = Item;
        Save();
    }
}

int TScore::GetInsertionPosition(int Points)
{
    for (int i = 0; i < Items.size(); i++)
    {
        if (Points >= Items[i].Points && i < 10)
        {
            return i;
        }
    }
    return -1;
}

std::string TScore::GetDataForItems()
{
    std::string Data;
    for (int i = 0; i < 10; i++)
    {
        Data += std::to_string(Items[i].Lp) + "|" + std::to_string(Items[i].Points) + "|";
        Data += Items[i].Name + "\r\n";
    }
    return Data;
}
