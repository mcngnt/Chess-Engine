#include "move.h"




#define northMask 7
#define southMask 56
#define eastMask 448
#define westMask 3584


// #define captureMask 31
// #define canCastleMask 480

// struct BitBoard
// {
// 	uint64_t whitePawn = 0;
// 	uint64_t whiteRook = 0;
// 	uint64_t whiteKnight = 0;
// 	uint64_t whiteBishop = 0;
// 	uint64_t whiteQueen = 0;
// 	uint64_t whiteKing = 0;
// 	uint64_t blackPawn = 0;
// 	uint64_t blackRook = 0;
// 	uint64_t blackKnight = 0;
// 	uint64_t blackBishop = 0;
// 	uint64_t blackQueen = 0;
// 	uint64_t blackKing = 0;
// };

// struct BoardStruct
// {
// 	int board[8][8];
// 	int whiteKingPos;
// 	int blackKingPos;
// };


struct GameState
{
	int capturedPiece;
	bool canWhiteKingCastle;
	bool canWhiteQueenCastle;
	bool canBlackKingCastle;
	bool canBlackQueenCastle;
	int doublePushFile;
	int moveCount;
	int whiteKingPos;
	int blackKingPos;
	uint64_t zobristKey;
};

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

enum Directions
{
	South = 8,
	North = -8,
	East = 1,
	West = -1,

	SouthWest = 7,
	NorthWest = -9,
	SouthEast = 9,
	NorthEast = -7
};

enum DirectionsID
{
	NorthID = 0,
	SouthID = 1,
	EastID = 2,
	WestID = 3,

	NorthEastID = 4,
	NorthWestID = 5,
	SouthEastID = 6,
	SouthWestID = 7
};



bool isPieceWhite(int piece);
int pieceType(int piece);


int pid(int i, int j);



class BoardManager
{
public:
	BoardManager();
	void makeMove(int move);
	// void unmakeMove(int move);
	int get(int pos);
	// void set(int x, int y, int piece);
	// void set(int pos, int piece);
	int get(int x, int y);
	std::vector<int> generatePseudoMoves();
	std::vector<int> generateMoves(bool onlyCaptures);
	bool isSquareEmpty(int i, int j);
	bool isSquareEmpty(int pid);
	bool isSquareFree(int i, int j);
	bool isSquareFree(int pid);
	int isLegal(std::vector<int> moves, int move);
	bool isSquareEnemy(int i, int j);
	bool isSquareEnemy(int pid);
	bool isSquareFriendly(int pid);
	void unmakeMove(int move);
	std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	// std::string startingFen = "4r1k1/ppp1bp2/2br1n2/P2pQ3/4pN1R/1P2PqP1/1BPP1P2/RN2K3 w Q - 15 41";

	// std::string startingFen = "7k/8/8/2p1p2p/2P1p2P/4P3/7K/8 b";
	// std::string startingFen = "6kr/8/8/8/8/3K4/8/8 w";
	// std::string startingFen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
	// std::string startingFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
	// std::string startingFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q1p/PPPBBPPP/RN2K2R w KQkq - ";
	// std::string startingFen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";
	// std::string startingFen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

	// std::string startingFen = "k7/3Qppp1/1R4p1/8/p7/P1p1P1N1/5PP1/2K5 b 0 1";
	void loadFen(std::string fen);
	std::string convertFen();
	// std::string convertFen();

	// BitBoard boardToBitBoard();
	// void loadBitBoard(BitBoard bitboard);

	uint64_t piecesZobrist[8][2][64];
	// Avec 0 : pas de prise en passant
	uint64_t doublePushFileZobrist[9];
	uint64_t whiteToMoveZobrist;
	uint64_t castlingRightZobrist[4];


	void initZobrist();
	uint64_t computeZobrist();

	uint64_t zobristKey;


	void controlledSquares();

	bool isChecked();

	void assign(int i, int j);

	void resetControl();


	bool whiteToMove = true;


	int board[8][8];
	std::stack<GameState> gameStateHistory;

	// std::stack<BitBoard> bitboardHistory;
	// std::stack<BoardStruct> boardHistory;

	// Bits :
	// 1 -> 5 : Capture
	// 6 -> 9 : Can Castle (0 : KingWhiteLeft, 1 : KingWhiteRight , 2 : KingBlackLeft , 3 : KingBlackRight)
	// 10 -> 12 : Double Push Rank ( 0 : No Double Push , 1,2... : Rank + 1)
	// 13-> 17 : 50 moves rule
	GameState currentGameState;

	// BitBoard currentBitboard;

	bool controlled[8][8];

};