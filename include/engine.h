#include "bot.h"




class Engine
{
public:
	Engine(int whiteBotType_, int blackBotType_);
	int get(int x, int y);
	int get(sf::Vector2i pos);
	int tryMove(sf::Vector2i p1, sf::Vector2i p2);

	void unmakeMove();

	void update();

	int isLegal(int move);


	void test(int depth);
	int aux(int depth);


	int whiteBotType;
	int blackBotType;
	BoardManager board;



	std::stack<int> movesHistory;

	std::vector<int> currentMoves;

	Bot whiteBot;
	Bot blackBot;

	bool checkmate = false;



};