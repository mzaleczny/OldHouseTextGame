#pragma once
#include <vector>
#include <string>
#include <condition_variable>
#include <mutex>
#include <deque>

namespace TextEngine
{
	class TTerminalScreen;
	class TDesktop;
	class TEvent;
	class TLevel;

	class TApplication
	{
	public:
		TApplication();
		virtual ~TApplication();
		void Run();
		
		// Ta funkcja nie dzia³a poprawnie jeœli jest wywo³ana w konstruktorze jakiegoœ obiektu TLevel
		void OpenLevel(TLevel* Level);
		int GetScreenWidth() const { return ScreenWidth; }
		int GetScreenHeight() const { return ScreenHeight; }
		void QuitApplication() { Quit = true; }
		bool IsApplicationQuitting() const { return Quit; }
		void ProcessEvent(TEvent* Event, TLevel* Level);
		inline uint64_t TimeSinceEpochInMillisec();
		TTerminalScreen* GetScreenBuffer() const { return ScreenBuffer; }
		TDesktop* GetDesktop() const { return Desktop; }
	protected:
		TTerminalScreen* ScreenBuffer;
		TDesktop* Desktop;
		TLevel* CurrentLevel;
		TLevel* LevelMarkedForDeletion;
		uint64_t LastFrameTime;
		int ScreenWidth;
		int ScreenHeight;
		bool Quit;
		bool UpdateTerminalDimensions();
		bool PollEvent(TEvent* Event);
		void Render();
		static std::string GetLineFromCin();
	};
}
