#include "move.h"
#include "piece.h"


#define northMask 7
#define southMask 56
#define eastMask 448
#define westMask 3584


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
	bool hasWhiteCastled;
	bool hasBlackCastled;
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


class BoardManager
{
public:
	BoardManager();
	void makeMove(int move);
	int get(int pos);
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
	// std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	std::string startingFen = "2qnR3/8/2P2P2/K1B1p3/b4N2/1k2p1P1/2R5/8 w - - 0 1";

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


	uint64_t piecesZobrist[8][2][64];
	// Avec 0 : pas de prise en passant
	uint64_t doublePushFileZobrist[9];
	uint64_t whiteToMoveZobrist;
	uint64_t castlingRightZobrist[4];


	void initZobrist();
	uint64_t computeZobrist();

	uint64_t zobristKey;


	void controlledSquares();
	void resetControl();

	bool isChecked();

	void assign(int i, int j);



	bool whiteToMove = true;


	int board[8][8];

	
	std::stack<GameState> gameStateHistory;
	std::vector<uint64_t> zobristHistory;

	GameState currentGameState;

	bool controlled[8][8];

};