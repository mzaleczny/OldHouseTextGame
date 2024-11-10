#pragma once
#include <string>

class TBoard
{
public:
	TBoard(int Length) : LineLen(Length) {};
	void Draw(std::string& Destination);
	int GetLineLength() const { return LineLen; }
	void SetLineLength(int Length) { LineLen = Length + (1-Length%2); }
private:
	int LineLen;
};
