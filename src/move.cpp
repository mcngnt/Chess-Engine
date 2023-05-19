#include <move.h>

int pos2DToInt(sf::Vector2i pos)
{
	return pos.x + 8 * pos.y;
}

int pos2DToInt(int x, int y)
{
	return x + 8 * y;
}

sf::Vector2i posIntTo2D(int pos)
{
	return sf::Vector2i(pos % 8, pos / 8);
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

int tag(int move)
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
	return standardPos(startPos(move)) + standardPos(endPos(move));
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