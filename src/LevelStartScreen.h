#pragma once
#include <string>
#include "TextEngine/Level.h"

class TextEngine::TEvent;
class TextEngine::TWindow;

class TLevelStartScreen : public TextEngine::TLevel
{
public:
	TLevelStartScreen(TextEngine::TApplication* Application, bool showTopTenOnStart = false);
	virtual ~TLevelStartScreen();

	//virtual int OnSizeChanged(TEvent* Event) override { return 0; }
	//virtual int OnKeyDown(TEvent* Event) override { return 0; }
	//virtual int OnUpdate(uint64_t LastFrameDuration) override { return 0; }
	virtual int OnDraw() override;
	virtual int OnInput(TextEngine::TEvent* Event) override;
private:
	TextEngine::TWindow Wnd;
	int StartSeed;
	bool GodMode;
	void ShowCredits();
	void ShowTopTen();
};
