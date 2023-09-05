#include "bot.h"




class Engine
{
public:
	Engine(int whiteBotType_, int blackBotType_);
	int get(int x, int y);
	int get(sf::Vector2i pos);
	int tryMove(sf::Vector2i p1, sf::Vector2i p2, char c);
	int tryMove(int move, char c);
	int tryMove(int move);
	void reset();

	int getBestMove();

	void unmakeMove();

	void update();

	int isLegal(int move);


	void perft(int depth);
	int perft_no_log(int depth);


	int whiteBotType;
	int blackBotType;

	BoardManager board;


	std::vector<int> currentMoves;

	Bot bot;

	bool gameFinished = false;



};