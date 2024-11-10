#include <iostream>
#include <vector>
#include <string>
#include <ctime> // deklaracja funkcji time
#include <iomanip>
#include "Desktop.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextEngine::TTerminalScreen::ClearRect(int Left, int Top, int Right, int Bottom, char BgChar)
{
    int Length = Right - Left + 1;
    for (int y = Top; y <= Bottom; y++)
    {
        Content.replace(y * Width + Left, Length, Length, BgChar);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextEngine::TTerminalScreen::DrawClippedMultilineText(int Column, int Row, std::string Text)
{
    if (Column >= Width) return;
    int WidthRemained = Width - Column;
    size_t StartPos = 0;
    size_t EndLinePos;
    size_t Len = Text.length();
    size_t LineLen;
    int Line = 0;
    while (StartPos < Len)
    {
        EndLinePos = Text.find('\n', StartPos);
        if (EndLinePos == std::string::npos)
        {
            LineLen = Len - StartPos;
        }
        else
        {
            LineLen = EndLinePos - StartPos;
        }
        DrawText(Column, Row + Line, Text.substr(StartPos, (LineLen <= WidthRemained) ? LineLen : WidthRemained));
        StartPos += LineLen + 1;
        Line++;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextEngine::TWindow::SetContent(TTerminalScreen* screen, int x, int y, char color, std::string caption, std::string text)
{
    Screen = screen;
    x = X;
    y = Y;
    Caption = caption;
    Text = text;
    Color = color;

    size_t StartPos = 0;
    size_t EndLinePos;
    size_t Len = Text.length();
    size_t LineLen, MaxLineLen = 0;;
    while (StartPos < Len)
    {
        EndLinePos = Text.find('\n', StartPos);
        if (EndLinePos == std::string::npos)
        {
            LineLen = Len - StartPos;
        }
        else
        {
            LineLen = EndLinePos - StartPos;
        }
        if (MaxLineLen < LineLen)
        {
            MaxLineLen = LineLen;
        }
        StartPos += LineLen + 1;
    }

    Width = static_cast<int>(MaxLineLen) + 6;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextEngine::TWindow::Draw()
{
    // zamaluj kolorem tla
    Screen->ClearRect(X, Y, X + Width - 1, Y + Height - 1, Color);
    // narysowanie ramki – dwie linie poziome -------------
    std::string Line(Width, '-');
    Screen->DrawText(X, Y, Line); // górny brzeg
    Screen->DrawText(X, Y + Height - 1, Line); // dolny brzeg
    // cd. rysowania ramki – dwie linie pionowe ------------
    for (int i = 0; i < Height; i++)
    {
        Screen->DrawText(X, Y + i, "|"); // lewy brzeg
        Screen->DrawText(X + Width, Y + i, "|"); // prawy brzeg
    }
    // napisanie tytulu okna na srodku pierwszej linijki
    Screen->DrawText(X + (Width - static_cast<int>(Caption.length())) / 2, Y, Caption);
    Screen->DrawClippedMultilineText(X + 3, Y + 2, Text);
    //Screen->PresentScreen();
}


//*****************************************************************************************************************
// definicje funkcji skladowych klasy Tpulpit
//*****************************************************************************************************************
void TextEngine::TDesktop::Refresh(bool ForceRedraw)
{
    for (TextEngine::TWindow* Wnd : Windows)
    {
        Wnd->Draw();
    }
    if (ForceRedraw)
    {
        Screen->PresentScreen();
    }
}

//*****************************************************************************************************************
// dodawanie okna
void TextEngine::TDesktop::operator+=(TWindow* wnd)
{
    Windows.push_back(wnd);
    Refresh();
}

//*****************************************************************************************************************
// usuwanie okna z pulpitu (czyli z wektora wskazników do okien)
void TextEngine::TDesktop::operator-=(TWindow* wnd)
{
    // odszukanie w tablicy wskaznika do tego okna
    for (unsigned int Index = 0 ; Index < Windows.size(); Index++)
    {
        if (Windows[Index] == wnd)
        {
            Windows.erase(Windows.begin() + Index);
            break;
        }
    }
    Clear(); // najpierw musimy zmazac caly pulpit
    Refresh();
}

//*****************************************************************************************************************
/* wydobycie na sam wierzch zadanego okienka */
void TextEngine::TDesktop::MoveToTop(TWindow* wnd)
{
    // polega na postawieniu go na samym koncu wektora
    // w tym celu najprosciej usunac je z wektora i natychmiast dodac
    *this -= wnd; // czyli pulpit –= okno
    *this += wnd; // czyli pulpit += okno
    Refresh();
}

//*****************************************************************************************************************
bool TextEngine::TDesktop::Contain(TWindow* wnd)
{
    for (TWindow* w : Windows)
    {
        if (wnd == w) return true;
    }
    return false;
}


//*****************************************************************************************************************
void TextEngine::TPixels::InitPixelsFromScreen()
{
    OriginalData.clear();
    ActualData.clear();
    int x = 0;
    int y = 0;
    for (int i = 0; i < Screen->Content.length(); i++)
    {
        char ch = Screen->GetCharAt(x, y);
        TVertex2d v(x, y, ch);
        OriginalData.push_back(v);
        ActualData.push_back(v);
        if (ch == '\n')
        {
            x = 0;
            y++;

        }
        else
        {
            x++;
        }
    }
}

//*****************************************************************************************************************
void TextEngine::TPixels::InitPixelsFromString(const std::string& Text, int XOffset)
{
    OriginalData.clear();
    ActualData.clear();
    int x = XOffset;
    int y = 0;
    for (int i = 0; i < Text.length(); i++)
    {
        char ch = Text[i];
        TVertex2d v(x, y, ch);
        OriginalData.push_back(v);
        ActualData.push_back(v);
        if (ch == '\n')
        {
            x = XOffset;
            y++;
        }
        else
        {
            x++;
        }
    }
}

//*****************************************************************************************************************
void TextEngine::TPixels::Draw(bool ClearBeforeRedraw)
{
    if (ClearBeforeRedraw)
    {
        Screen->Clear();
    }
    for (int i = 0; i < ActualData.size(); i++)
    {
        if (ActualData[i].ch != '\n')
        {
            if (!DoSinusoidalBehavior)
            {
                Screen->PutCharAt(static_cast<int>(ActualData[i].x + TranslateX), static_cast<int>(ActualData[i].y + TranslateY), ActualData[i].ch);
            }
            else
            {
                float X = ActualData[i].x + TranslateX;
                float Y = ActualData[i].y + TranslateY + 1.1f * sin(X * 0.17f);
                Screen->PutCharAt(static_cast<int>(X), static_cast<int>(Y), ActualData[i].ch);
            }
        }
    }
}

//*****************************************************************************************************************
void TextEngine::TPixels::Rotate(float alpha)
{
    int PivotX = Screen->Width / 2;
    int PivotY = Screen->Height / 2;
    float s = sin(alpha);
    float c = cos(alpha);
    ActualData.clear();
    for (int i = 0; i < OriginalData.size(); i++)
    {
        float X = static_cast<float>(OriginalData[i].x - PivotX);
        float Y = static_cast<float>(OriginalData[i].y - PivotY);
        float XRot = X * c - Y * s;
        float YRot = X * s + Y * c;
        XRot = round(XRot + PivotX);
        YRot = round(YRot + PivotY);
        TVertex2d v(static_cast<int>(XRot), static_cast<int>(YRot), OriginalData[i].ch);
        ActualData.push_back(v);
    }
}

//*****************************************************************************************************************
void TextEngine::TPixels::Scale(float factor)
{
    int PivotX = Screen->Width / 2;
    int PivotY = Screen->Height / 2;
    for (int i = 0; i < ActualData.size(); i++)
    {
        float X = static_cast<float>(ActualData[i].x - PivotX);
        float Y = static_cast<float>(ActualData[i].y - PivotY);
        X *= factor;
        Y *= factor;
        ActualData[i].x = static_cast<int>(round(X + PivotX));
        ActualData[i].y = static_cast<int>(round(Y + PivotY));
    }
}
