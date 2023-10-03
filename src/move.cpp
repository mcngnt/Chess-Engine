#include <move.h>

#if !USE_UCI
int pos2DToInt(sf::Vector2i pos)
{
	return pos.x + 8 * pos.y;
}
sf::Vector2i posIntTo2D(int pos)
{
	return sf::Vector2i(pos % 8, pos / 8);
}
#endif

int pos2DToInt(int x, int y)
{
	return x + 8 * y;
}


int genMove(int startPos, int endPos, int tag)
{
	if (startPos >= 0 && startPos < 64 && endPos >= 0 && endPos < 64)
	{
		return startPos | endPos << 6 | tag << 12;
	}
	else
	{
		return 0;
	}
}


int genMove(int startPosx, int startPosy, int endPosx, int endPosy, int tag)
{
	return (startPosx + 8 * startPosy) | (endPosx + 8 * endPosy) << 6 | tag << 12;
}

int discardTag(int move)
{
	return move & positionsMoveMask;
}

int endPos(int move)
{
	return (move >> 6) & 63;
}

int startPos(int move)
{
	return move & 63;
}

int getTag(int move)
{
	return (move & tagMask) >> 12;
}

std::string standardPos(int pos)
{
	std::string res;
	res.push_back('a' + pos%8);
	res.push_back('0'+ 7 - pos/8 + 1);
	return res;
}

std::string standardNotation(int move)
{
	int t = getTag(move);
	char c = ' ';
	switch (t)
	{
		case QueenProm:
			c = 'q';
			break;
		case QueenPromCapture:
			c = 'q';
			break;
		case KnightProm:
			c = 'n';
			break;
		case KnightPromCapture:
			c = 'n';
			break;
		case RookProm:
			c = 'r';
			break;
		case RookPromCapture:
			c = 'r';
			break;
		case BishopProm:
			c = 'b';
			break;
		case BishopPromCapture:
			c = 'b';
			break;
	}
	return standardPos(startPos(move)) + standardPos(endPos(move)) + c;
}

bool isCapturingTag(int tag)
{
	if ( (tag <= 3) || ( (8 <= tag) && (tag <= 11) ))
	{
		return false;
	}
	else
	{
		return true;
	}
}

int standPosToInt(char c1, char c2)
{
	return (c1 - 'a') + 8 * (7 - (c2 - '1'));
}

int standNotToMove(std::string standNot)
{
	return genMove(standPosToInt(standNot[0],standNot[1]), standPosToInt(standNot[2],standNot[3]), 0);
}
