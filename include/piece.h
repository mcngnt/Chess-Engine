enum Pieces
{
	None = 0,
	Pawn = 1,
	Rook = 2,
	Knight = 3,
	Bishop = 4,
	Queen = 5,
	King = 6,
	
	White = 8,
	Black = 16
};



bool isPieceWhite(int piece);
int pieceType(int piece);
int pid(int i, int j);
