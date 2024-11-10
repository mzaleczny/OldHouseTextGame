#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <Windows.h>

namespace TextEngine
{

    class TPixels;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TTerminalScreen
    {
        friend class TPixels;
    public:
        std::string Content;
        char BgChar;
        int Width;
        int Height;
    public:
        TTerminalScreen(int W = 120, int H = 30) : BgChar(' '), Width(W), Height(H)
        {
            Content.resize(Height * Width, BgChar);
            Clear();
        }

        void Resize(int W, int H)
        {
            Width = W;
            Height = H;
            Content.resize(Height * Width, BgChar);
            Clear();
        }

        //--------------------
        void ClearRect(int Left, int Top, int Right, int Bottom, char BgChar);

        //--------------------
        void PresentScreen()
        {
            COORD coord;
            coord.X = 0;
            coord.Y = 0;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
            std::cout << Content << std::flush;
        }

        //--------------------
        void Clear()
        {
            ClearRect(0, 0, Width - 1, Height - 1, BgChar);
        }

        //-------------------
        void DrawText(int Column, int Row, std::string Text)
        {
            Content.replace(Width * Row + Column, Text.length(), Text);
        }

        //-------------------
        void DrawClippedMultilineText(int Column, int Row, std::string Text);

        //-------------------
        char GetCharAt(int Column, int Row)
        {
            if (Column < 0 || Column >= Width) return -1;
            if (Row < 0 || Row >= Height) return -1;
            int i = Width * Row + Column;
            if (i >= 0 && i < Content.length())
                return Content[i];
            return -1;
        }

        //-------------------
        void PutCharAt(int Column, int Row, char ch)
        {
            if (Column < 0 || Column >= Width - 1) return;
            if (Row < 0 || Row >= Height) return;
            int i = Width * Row + Column;
            if (i >= 0 && i < Content.length())
                Content[i] = ch;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TTerminalDesktop;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TWindow
    {
        int X, Y, Width, Height;
        char Color;
        std::string Caption;
        std::string Text;
        TTerminalScreen* Screen;
    public:
        TWindow() = default;
        TWindow(TTerminalScreen* screen, int x, int y, int w, int h, char color, std::string caption)
            : Screen(screen), X(x), Y(y), Width(w), Height(h), Color(color), Caption(caption)
        { }
        void SetContent(TTerminalScreen* screen, int x, int y, char color, std::string caption, std::string text);
        void SetPos(int x, int y) { X = x; Y = y; }
        void SetSize(int w, int h) { Width = w; Height = h; }
        void SetWidth(int w) { Width = w; }
        void SetHeight(int h) { Height = h; }
        int GetX() const { return X; }
        int GetY() const { return Y; }
        int GetWidth() const { return Width; }
        int GetHeight() const { return Height; }
        TTerminalDesktop operator+(TWindow& wnd);
        void Draw();
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TDesktop
    {
        TTerminalScreen* Screen;
        std::vector<TWindow*> Windows;
    public:
        TDesktop(TTerminalScreen* screen) : Screen(screen)
        { }
        void Refresh(bool ForceRedraw = false);
        void Clear() { Screen->Clear(); };
        int GetWindowsCount() const { return static_cast<int>(Windows.size()); }
        // ------------ przeladowane operatory
        void operator+=(TWindow* wnd); // dodawanie okienka
        void operator-=(TWindow* wnd); // usuwanie okienka
        void MoveToTop(TWindow* wnd); // wyjmowanie na sam wierzch
        bool Contain(TWindow* wnd); // czy dane okno jest na liscie wyswietlanych
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TVertex2d
    {
        friend class TPixels;
        int x, y;
        char ch;
    public:
        TVertex2d(int _x = 0, int _y = 0, char _ch = ' ') : x(_x), y(_y), ch(_ch)
        { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TPixels
    {
        TTerminalScreen* Screen;
        std::vector<TVertex2d> OriginalData;
        std::vector<TVertex2d> ActualData;
        float TranslateX;
        float TranslateY;
        bool DoSinusoidalBehavior;
        void InitPixelsFromScreen();
        void InitPixelsFromString(const std::string& Text, int XOffset = 0);
    public:
        TPixels(TTerminalScreen* screen) : Screen(screen), TranslateX(0.0f), TranslateY(0.0f), DoSinusoidalBehavior(false)
        {
            InitPixelsFromScreen();
        }
        TPixels(TTerminalScreen* screen, std::string& Text, int XOffset) : Screen(screen), TranslateX(0.0f), TranslateY(0.0f), DoSinusoidalBehavior(false)
        {
            InitPixelsFromString(Text, XOffset);
        }
        void Update(TTerminalScreen* screen) { InitPixelsFromScreen(); }
        void Draw(bool ClearBeforeRedraw = true);
        void Translate(float dx, float dy) { TranslateX += dx; TranslateY += dy; }
        float GetTranslateX() const { return TranslateX; }
        float GetTranslateY() const { return TranslateY; }
        void SetTranslateX(float dx) { TranslateX = dx; }
        void SetTranslateY(float dy) { TranslateY = dy; }
        bool GetDoSinusoidalBehavior() const { return DoSinusoidalBehavior; }
        void SetSinusoidalBehavior(bool Value) { DoSinusoidalBehavior = Value; }
        void Rotate(float alpha);
        void Scale(float factor);
    };

}
