#include "boardManager.h"
#include "transposition.h"

#define maxBotDepth 50

#define pawnValue 100
#define knightValue 320
#define bishopValue 330
#define rookValue 500
#define queenValue 900
#define kingValue 20000

enum BotType
{
	NotBot = 0,
	Random = 1,
	Good = 2

};

struct MoveScore
{
    int move;
    int score;
};


class Bot
{
public:
	Bot();
	
	int play(BoardManager* board);
	int playRandom(BoardManager* board);
	int playWell(BoardManager* board);

	int quietSearch(BoardManager* board, int alpha, int beta);

	int search(BoardManager* board, char depth, int alpha, int beta);
	int evaluate(BoardManager* board);

	int scoreMove(BoardManager* board, int move);
	std::vector<int> orderMoves(BoardManager* board, std::vector<int> moves, char depth);

	int accessHeatMap(int pType,int i, int j, bool whitePlaying);



	int botType = Random;

	int nbMoves = 0;
	int nbTranspo = 0;

	char currentDepth;

	float maxTime = 3000.0;

	int bestMove;
	int currentBestMove;

	std::chrono::high_resolution_clock::time_point startTime;

	bool reachedTime;

	int nbQMoves;

	TranspositionTable transpositionTable;


	int heatMapPawn[8][8] = { 
		{ 0,  0,  0,  0,  0,  0,  0,  0},
		{50, 50, 50, 50, 50, 50, 50, 50},
		{10, 10, 20, 30, 30, 20, 10, 10},
		{ 5,  5, 10, 25, 25, 10,  5,  5},
		{ 0,  0,  0, 20, 20,  0,  0,  0},
		{ 5, -5,-10,  0,  0,-10, -5,  5},
		{ 5, 10, 10,-20,-20, 10, 10,  5},
		{ 0,  0,  0,  0,  0,  0,  0,  0}
	};

	int heatMapKnight[8][8] = {
		{-50,-40,-30,-30,-30,-30,-40,-50},
		{-40,-20,  0,  0,  0,  0,-20,-40},
		{-30,  0, 10, 15, 15, 10,  0,-30},
		{-30,  5, 15, 20, 20, 15,  5,-30},
		{-30,  0, 15, 20, 20, 15,  0,-30},
		{-30,  5, 10, 15, 15, 10,  5,-30},
		{-40,-20,  0,  5,  5,  0,-20,-40},
		{-50,-40,-30,-30,-30,-30,-40,-50},
	};

	int heatMapBishop[8][8] = {
		{-20,-10,-10,-10,-10,-10,-10,-20},
		{-10,  0,  0,  0,  0,  0,  0,-10},
		{-10,  0,  5, 10, 10,  5,  0,-10},
		{-10,  5,  5, 10, 10,  5,  5,-10},
		{-10,  0, 10, 10, 10, 10,  0,-10},
		{-10, 10, 10, 10, 10, 10, 10,-10},
		{-10,  5,  0,  0,  0,  0,  5,-10},
		{-20,-10,-10,-10,-10,-10,-10,-20},
	};

	int heatMapRook[8][8] = 
	{
		{0,  0,  0,  0,  0,  0,  0,  0},
		{ 5, 10, 10, 10, 10, 10, 10,  5},
		{-5,  0,  0,  0,  0,  0,  0, -5},
		{-5,  0,  0,  0,  0,  0,  0, -5},
		{-5,  0,  0,  0,  0,  0,  0, -5},
		{-5,  0,  0,  0,  0,  0,  0, -5},
		{-5,  0,  0,  0,  0,  0,  0, -5},
		{ 0,  0,  0,  5,  5,  0,  0,  0}
	};

	int heatMapQueen[8][8] = {
		{-20,-10,-10, -5, -5,-10,-10,-20},
		{-10,  0,  0,  0,  0,  0,  0,-10},
		{-10,  0,  5,  5,  5,  5,  0,-10},
		{ -5,  0,  5,  5,  5,  5,  0, -5},
		{  0,  0,  5,  5,  5,  5,  0, -5},
		{-10,  5,  5,  5,  5,  5,  0,-10},
		{-10,  0,  5,  0,  0,  0,  0,-10},
		{-20,-10,-10, -5, -5,-10,-10,-20}
	};

	int heatMapKingMiddle[8][8] = {
		{-30,-40,-40,-50,-50,-40,-40,-30},
		{-30,-40,-40,-50,-50,-40,-40,-30},
		{-30,-40,-40,-50,-50,-40,-40,-30},
		{-30,-40,-40,-50,-50,-40,-40,-30},
		{-20,-30,-30,-40,-40,-30,-30,-20},
		{-10,-20,-20,-20,-20,-20,-20,-10},
		{ 20, 20,  0,  0,  0,  0, 20, 20},
		{ 20, 30, 10,  0,  0, 10, 30, 20}
	};

};