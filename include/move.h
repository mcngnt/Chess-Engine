
#include <utils.h>


#define positionsMoveMask 4095

#define startPosMask 63
#define endPosMask 4032


#define tagMask 61440

enum Tag
{
	QuietMove = 0,
	DoublePawnPush = 1,
	KingCastle = 2,
	QueenCastle = 3,
	Capture = 4,
	EPCapture = 5,
	KnightProm = 8,
	BishopProm = 9,
	RookProm = 10,
	QueenProm = 11,
	KnightPromCapture = 12,
	BishopPromCapture = 13,
	RookPromCapture = 14,
	QueenPromCapture = 15
};


int pos2DToInt(sf::Vector2i pos);
int pos2DToInt(int x, int y);
sf::Vector2i posIntTo2D(int pos);
int genMove(int startPos, int endPos, int tag);
int genMove(int startPosx, int startPosy, int endPosx, int endPosy, int tag);
int discardTag(int move);
int endPos(int move);
int startPos(int move);
int tag(int move);
std::string standardNotation(int move);
bool isCapturingTag(int tag);