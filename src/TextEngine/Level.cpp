#include "Desktop.h"
#include "Application.h"
#include "Level.h"

TextEngine::TLevel::TLevel(TApplication* Application)
	: ParentApplication(Application), InputPrompt("Co robisz? "), DoInput(false)
{
	XInput = 11;
	YInput = Application->GetScreenHeight() - 2;
}

TextEngine::TLevel::~TLevel()
{
}
