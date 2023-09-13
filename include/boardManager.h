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
	bool isSquareNotFriendly(int i, int j);
	bool isSquareNotFriendly(int pid);
	bool isSquareNotEnemy(int i, int j);
	bool isSquareNotEnemy(int pid);
	int isLegal(std::vector<int> moves, int move);
	bool isSquareEnemy(int i, int j);
	bool isSquareEnemy(int pid);
	bool isSquareFriendly(int pid);
	void unmakeMove(int move);

	std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	// std::string startingFen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/P7/1PP1NnPP/RNBQK2R b KQ - 1 8  ";
	// std::string startingFen = "rnbq1k1r/pp1P1ppp/2p2b2/8/2B5/P7/1PP1NnPP/RNBQK2R w KQ - 1 8  ";
	void loadFen(std::string fen);
	std::string convertFen();


	uint64_t piecesZobrist[8][2][64];
	// Avec 0 : pas de prise en passant
	uint64_t doublePushFileZobrist[9];
	uint64_t whiteToMoveZobrist;
	uint64_t castlingRightZobrist[4];

	uint64_t piecesBitboard[12];

	int getPieceBitboardIndex(int pieceType, bool isPieceWhite);
	uint64_t getPieceBitboard(int pieceType, bool isPieceWhite);
	void togglePieceBitboard(int pieceType, bool isPieceWhite, int squareIndex);
	bool isPieceHereBitboard(int pieceType, bool isPieceWhite, int squareIndex);

	bool foundBug = false;


	void initZobrist();
	uint64_t computeZobrist();

	uint64_t zobristKey;

	void makeNullMove();
	void unmakeNullMove();


	// void controlledSquares();
	// void resetControl();

	bool isChecked();

	bool isRepetitionDraw();

	// void assign(int i, int j);

	uint64_t attackMap;
	uint64_t friendlyPiecesBitboard;
	uint64_t enemyPiecesBitboard;
	uint64_t allPiecesBitboard;

	void fillBitboardData();



	bool whiteToMove = true;


	int board[8][8];

	
	std::stack<GameState> gameStateHistory;
	std::stack<int> movesHistory;
	int historySize = 0;

	uint64_t repetitionTable[1024];

	GameState currentGameState;

	// bool controlled[8][8];

};