#include "engine.h"


Engine::Engine(int whiteBotType_, int blackBotType_)
{
	whiteBotType = whiteBotType_;
	blackBotType = blackBotType_;

	if (whiteBotType != NotBot)
	{
		whiteBot.botType = whiteBotType;
	}
	if (blackBotType != NotBot)
	{
		blackBot.botType = blackBotType;
	}


	currentMoves = board.generateMoves(false);

	printf("True  : %llu\n", board.computeZobrist());
	printf("Board : %llu\n", board.zobristKey);
	printf("---\n");
}

int Engine::aux(int depth)
{
	if (depth == 0)
	{
		return 1;
	}

	// printf("%d\n", board.get(3,1));

	std::vector<int> moves = board.generateMoves(false);
	int nb = 0;
	for(int move : moves)
	{
		board.makeMove(move);
		nb += aux(depth-1);
		board.unmakeMove(move);
	}
	return nb;
}


void Engine::test(int depth)
{
	std::vector<int> moves = board.generateMoves(false);

	int nb = 0;
	
	for(int move : moves)
	{
		board.makeMove(move);
		int i = aux(depth-1);
		printf("%s: %d\n", standardNotation(move).c_str(), i);
		board.unmakeMove(move);
		nb += i;
	}
	printf("%d\n", nb);
}


int Engine::get(int x, int y)
{
	return board.get(x,y);
}

int Engine::get(sf::Vector2i pos)
{
	return board.get(pos.x, pos.y);
}

int Engine::isLegal(int move)
{
	return board.isLegal(currentMoves, move);
}


int Engine::tryMove(sf::Vector2i p1, sf::Vector2i p2)
{
	int move = genMove(pos2DToInt(p1), pos2DToInt(p2), 0);

	int legalMove = isLegal(move);

	if (legalMove > 0)
	{
		if (  tag(legalMove) >= 8)
		{
			int newTag = 0;
			switch (tag(legalMove))
			{
				case KnightProm:
					newTag = QueenProm;
					break;
				case BishopProm:
					newTag = QueenProm;
					break;
				case RookProm:
					newTag = QueenProm;
					break;
				case KnightPromCapture:
					newTag = QueenPromCapture;
					break;
				case BishopPromCapture:
					newTag = QueenPromCapture;
					break;
				case RookPromCapture:
					newTag = QueenPromCapture;
					break;
				
			}
			
			legalMove = discardTag(legalMove) | (newTag << 12);
		}
		board.makeMove(legalMove);

		printf("True  : %llu\n", board.computeZobrist());
		printf("Board : %llu\n", board.zobristKey);
		printf("%d %d %d %d\n", board.currentGameState.canWhiteKingCastle, board.currentGameState.canWhiteQueenCastle, board.currentGameState.canBlackKingCastle, board.currentGameState.canBlackQueenCastle);
		printf("---\n");

		currentMoves = board.generateMoves(false);
		movesHistory.push(legalMove);
		// update();
		return legalMove;
	}
	return 0;
}

void Engine::update()
{
	if ((board.whiteToMove && whiteBotType == NotBot) || (!board.whiteToMove && blackBotType == NotBot))
	{
		if (currentMoves.size() == 0)
		{
			checkmate = true;
		}

	}
	else
	{
		int move;
		if (board.whiteToMove)
		{
			move = whiteBot.play(&board);
			printf("True  : %llu\n", board.computeZobrist());
			printf("Board : %llu\n", board.zobristKey);
			printf("%d %d %d %d\n", board.currentGameState.canWhiteKingCastle, board.currentGameState.canWhiteQueenCastle, board.currentGameState.canBlackKingCastle, board.currentGameState.canBlackQueenCastle);
			printf("---\n");
		}
		else
		{
			move = blackBot.play(&board);
			printf("True  : %llu\n", board.computeZobrist());
			printf("Board : %llu\n", board.zobristKey);
			printf("%d %d %d %d\n", board.currentGameState.canWhiteKingCastle, board.currentGameState.canWhiteQueenCastle, board.currentGameState.canBlackKingCastle, board.currentGameState.canBlackQueenCastle);
			printf("---\n");
		}
		if (move == -1)
		{
			checkmate = true;
		}
		currentMoves = board.generateMoves(false);
		movesHistory.push(move);
	}
}

void Engine::unmakeMove()
{
	if (!movesHistory.empty())
	{
		board.unmakeMove(movesHistory.top());
		movesHistory.pop();
		currentMoves = board.generateMoves(false);
		// update();
	}
	checkmate = false;
}