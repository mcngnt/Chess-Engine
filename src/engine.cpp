#include "engine.h"


Engine::Engine(int whiteBotType_, int blackBotType_)
{
	whiteBotType = whiteBotType_;
	blackBotType = blackBotType_;

	currentMoves = board.generateMoves(false);

	bot.evaluate(&board);
}


void Engine::reset()
{
	board = BoardManager();
	currentMoves = board.generateMoves(false);
	gameFinished = false;
}


int Engine::aux(int depth)
{
	if (depth == 0)
	{
		return 1;
	}

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


int Engine::tryMove(sf::Vector2i p1, sf::Vector2i p2, char c)
{
	int move = genMove(pos2DToInt(p1), pos2DToInt(p2), 0);

	int legalMove = isLegal(move);

	if (legalMove > 0)
	{
		if (  tag(legalMove) >= 8)
		{
			int newTag = 0;
			int newTagPr = QueenProm;
			int newTagPrCap = QueenPromCapture;
			if (c == 'q')
			{
				newTagPr = QueenProm;
				newTagPrCap = QueenPromCapture;
			}
			if (c == 'n')
			{
				newTagPr = KnightProm;
				newTagPrCap = KnightPromCapture;
			}
			if (c == 'b')
			{
				newTagPr = BishopProm;
				newTagPrCap = BishopPromCapture;
			}
			if (c == 'r')
			{
				newTagPr = RookProm;
				newTagPrCap = RookPromCapture;
			}

			switch (tag(legalMove))
			{
				case KnightProm:
					newTag = newTagPr;
					break;
				case BishopProm:
					newTag = newTagPr;
					break;
				case RookProm:
					newTag = newTagPr;
					break;
				case KnightPromCapture:
					newTag = newTagPrCap;
					break;
				case BishopPromCapture:
					newTag = newTagPrCap;
					break;
				case RookPromCapture:
					newTag = newTagPrCap;
					break;
				
			}
			
			legalMove = discardTag(legalMove) | (newTag << 12);
		}
		board.makeMove(legalMove);

		currentMoves = board.generateMoves(false);
		board.movesHistory.push(legalMove);
		return legalMove;
	}
	return 0;
}

int Engine::tryMove(int move, char c)
{
	int legalMove = isLegal(move);

	if (legalMove > 0)
	{
		if (  tag(legalMove) >= 8)
		{
			int newTag = 0;
			int newTagPr = QueenProm;
			int newTagPrCap = QueenPromCapture;
			if (c == 'q')
			{
				newTagPr = QueenProm;
				newTagPrCap = QueenPromCapture;
			}
			if (c == 'n')
			{
				newTagPr = KnightProm;
				newTagPrCap = KnightPromCapture;
			}
			if (c == 'b')
			{
				newTagPr = BishopProm;
				newTagPrCap = BishopPromCapture;
			}
			if (c == 'r')
			{
				newTagPr = RookProm;
				newTagPrCap = RookPromCapture;
			}

			switch (tag(legalMove))
			{
				case KnightProm:
					newTag = newTagPr;
					break;
				case BishopProm:
					newTag = newTagPr;
					break;
				case RookProm:
					newTag = newTagPr;
					break;
				case KnightPromCapture:
					newTag = newTagPrCap;
					break;
				case BishopPromCapture:
					newTag = newTagPrCap;
					break;
				case RookPromCapture:
					newTag = newTagPrCap;
					break;
				
			}
			
			legalMove = discardTag(legalMove) | (newTag << 12);
		}
		board.makeMove(legalMove);

		currentMoves = board.generateMoves(false);
		board.movesHistory.push(legalMove);
		return legalMove;
	}
	return 0;
}

int Engine::tryMove(int move)
{
	board.makeMove(move);
	currentMoves = board.generateMoves(false);
	board.movesHistory.push(move);
	return move;
}

int Engine::getBestMove()
{
	return bot.play(&board);
}

void Engine::update()
{
	if (currentMoves.size() == 0)
	{
		gameFinished = true;
	}
	if (gameFinished)
	{
		return;
	}
	if ((board.whiteToMove && whiteBotType == Human) || (!board.whiteToMove && blackBotType == Human))
	{
	}
	else
	{
		int move;
		if (board.whiteToMove)
		{
			move = bot.play(&board);
			board.makeMove(move);
		}
		else
		{
			move = bot.play(&board);
			board.makeMove(move);
		}
		board.movesHistory.push(move);
		currentMoves = board.generateMoves(false);
	}
}

void Engine::unmakeMove()
{
	if (!board.movesHistory.empty())
	{
		board.unmakeMove(board.movesHistory.top());
		board.movesHistory.pop();
		currentMoves = board.generateMoves(false);
		// update();
	}
	gameFinished = false;
}