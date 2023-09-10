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
	int isLegal(std::vector<int> moves, int move);
	void unmakeMove(int move);

	bool isSquareEnemy(int sq);
	bool isSquareFriendly(int sq);
	bool isSquareEmpty(int sq);
	bool isSquareEmptyFriendly(int sq);
	bool isSquareEmptyEnemy(int sq);
	bool isSquareEnemy(int i, int j);
	bool isSquareFriendly(int i, int j);
	bool isSquareEmpty(int i, int j);
	bool isSquareEmptyFriendly(int i, int j);
	bool isSquareEmptyEnemy(int i, int j);



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

	bool foundBug = false;


	void initZobrist();
	uint64_t computeZobrist();

	uint64_t zobristKey;

	void makeNullMove();
	void unmakeNullMove();


	// void controlledSquares();
	// void resetControl();

	void fillBitboardData();

	bool isChecked();

	bool isRepetitionDraw();

	// void assign(int i, int j);



	bool whiteToMove = true;


	int board[8][8];

	uint64_t friendlyPiecesBitboard;
	uint64_t opponentPiecesBitboard;
	uint64_t allPiecesBitboard;

	uint64_t attackMap;

	
	std::stack<GameState> gameStateHistory;
	std::stack<int> movesHistory;
	int historySize = 0;

	uint64_t repetitionTable[1024];

	GameState currentGameState;

	// bool controlled[8][8];

};