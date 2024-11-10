#include "TextEngine/Desktop.h"
#include "TextEngine/Application.h"
#include "LevelStartScreen.h"
using namespace std;

int main()
{
	/*
	Deck deck;
	deck.Shuffle();

	deck.Deal().PrintCard();
	deck.Deal().PrintCard();
	deck.Deal().PrintCard();
	deck.Deal().PrintCard();
	deck.Deal().PrintCard();

	cout << endl << deck.GetSize() << endl;

	deck.Print();
	*/
	TextEngine::TApplication* Application = new (nothrow) TextEngine::TApplication();
	if (!Application)
	{
		cerr << "Error! Out of memory, cannot create application." << endl;
		return -1;
	}

	TLevelStartScreen* Level = new (nothrow) TLevelStartScreen(Application);
	if (!Level)
	{
		cerr << "Error! Out of memory, cannot create main screen level." << endl;
		delete Application;
		return -2;
	}

	Application->OpenLevel(Level);
	Application->Run();

	//delete GameState;
	delete Application;

	return 0;
}
