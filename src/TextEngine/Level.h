#pragma once
#include <string>

namespace TextEngine
{
	class TApplication;
	class TPixels;
	class TEvent;

	class TLevel
	{
	public:
		TLevel(TextEngine::TApplication* Application);
		virtual ~TLevel();
		
		virtual int OnSizeChanged(TEvent* Event) { return 0; }
		virtual int OnInput(TEvent* Event) { return 0; }
		virtual int OnKeyDown(TEvent* Event) { return 0; }
		virtual int OnUpdate(uint64_t LastFrameDuration) { return 0; }
		virtual int OnDraw() { return 0; }
		virtual bool IsPlaying() { return false; }
		virtual bool GetDoInput() const { return DoInput; }
		int GetXInput() const { return XInput; }
		int GetYInput() const { return YInput; }
		std::string GetInputPrompt() const { return InputPrompt; }
	protected:
		TApplication* ParentApplication = nullptr;
		std::string InputPrompt;
		int XInput, YInput;
		bool DoInput;
	};
}
