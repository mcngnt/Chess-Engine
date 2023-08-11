#include "piece.h"



bool isPieceWhite(int piece)
{
	return (piece >> 3) == 1;
}
int pieceType(int piece)
{
	return piece & 7;
}

int pid(int i, int j)
{
	return i + 8 * j;
}