#include "bot.h"



Bot::Bot()
{
}

int pieceValue(int ptype)
{
	switch (ptype)
	{
		case None:
			return 0;
			break;
		case Pawn:
			return pawnValue;
			break;
		case Knight:
			return knightValue;
			break;
		case Rook:
			return rookValue;
			break;
		case Bishop:
			return bishopValue;
			break;
		case Queen:
			return queenValue;
			break;
		default:
			return knightValue;
	}
}

int Bot::scoreMove(BoardManager* board, int move)
{
	int score = 0;

	int movingPieceType = pieceType(board->get(startPos(move)));
	int capturedPieceType = pieceType(board->get(endPos(move)));
	int moveTag = tag(move);

	if (capturedPieceType > 0)
	{
		score += 10 * pieceValue(capturedPieceType) - pieceValue(movingPieceType);
	}

	if (moveTag >= 8) // Promotion
	{
		int promPieceType = Queen;
		switch (moveTag)
		{
			case KnightProm : case KnightPromCapture:
				promPieceType = Knight;
				break;
			case BishopProm : case BishopPromCapture:
				promPieceType = Bishop;
				break;
			case RookProm: case RookPromCapture:
				promPieceType = Rook;
				break;
		}
		score += pieceValue(promPieceType);
	}

	board->whiteToMove = !board->whiteToMove;

	board->controlledSquares();

	if (board->controlled[endPos(move) / 8][endPos(move) % 8])
	{
		score -= pieceValue(movingPieceType);
	}

	board->whiteToMove = !board->whiteToMove;


	return score;
}

bool compMove(MoveScore mscore1, MoveScore mscore2)
{
	return mscore1.score > mscore2.score;
}


std::vector<int> Bot::orderMoves(BoardManager* board, std::vector<int> moves)
{
	std::vector<MoveScore> movesScore;
	for (int move : moves)
	{
		MoveScore mscore;
		mscore.move = move;
		mscore.score = scoreMove(board, move);
		movesScore.push_back(mscore);
	}

	std::sort(movesScore.begin(), movesScore.end(), compMove);

	std::vector<int> sortedMoves;

	for(MoveScore mscore : movesScore)
	{
		sortedMoves.push_back(mscore.move);
	}


	return sortedMoves;
}

int Bot::play(BoardManager* board)
{
	if (botType == Random)
	{
		return playRandom(board);
	}

	if (botType == Good)
	{
		return playWell(board);
	}



	return 0;
}

int heatMapJ(int j, bool whitePlaying)
{
	if(whitePlaying)
	{
		return j;
	}
	else
	{
		return 7 - j;
	}
}

int Bot::accessHeatMap(int pType,int i, int j, bool whitePlaying)
{
	switch(pType)
	{
		case Pawn:
			return heatMapPawn[heatMapJ(j, whitePlaying)][i];
			break;
		case Knight:
			return heatMapKnight[heatMapJ(j, whitePlaying)][i];
			break;
		case Bishop:
			return heatMapBishop[heatMapJ(j, whitePlaying)][i];
			break;
		case Rook:
			return heatMapRook[heatMapJ(j, whitePlaying)][i];
			break;
		case Queen:
			return heatMapQueen[heatMapJ(j, whitePlaying)][i];
			break;
		case King:
			return heatMapKingMiddle[heatMapJ(j, whitePlaying)][i];
			break;
	}
	return 0;
}



int Bot::evaluate(BoardManager* board)
{
	int score = 0;

	int whiteScoreValue = 0;
	int blackScoreValue = 0;
	for (int i = 0 ; i < 64 ; ++i)
	{
		int piece = board->get(i);
		int pType = pieceType(piece);
		if (isPieceWhite(piece))
		{
			whiteScoreValue += pieceValue(pType);
		}
		else
		{
			blackScoreValue += pieceValue(pType);
		}

		if (board->whiteToMove == isPieceWhite(piece))
		{
			score += accessHeatMap(pType, i%8, i/8,board->whiteToMove);
		}

	}


	score /= 30;

	score += 2 * (whiteScoreValue - blackScoreValue) * (board->whiteToMove ? 1 : -1);

	// printf("%d\n", score);
	// printf("---\n");



	return score;
}

int Bot::search(BoardManager* board, int depth, int alpha, int beta)
{
	if (depth == 0)
	{
		nbMoves += 1;
		return evaluate(board);
	}

	std::vector<int> moves = board->generateMoves();
	std::vector<int> sortedMoves = orderMoves(board, moves);

	if (sortedMoves.size() == 0)
	{
		moveSeq[depth - 1] = -1;
		if (board->isChecked())
		{
			return -3*kingValue;
		}
		return 0;
	}

	for (int move : sortedMoves)
	{
		board->makeMove(move);
		int eval = -search(board, depth - 1, -beta, -alpha);
		board->unmakeMove(move);
		if (eval >= beta)
		{
			moveSeq[depth - 1] = move;
			return beta;
		}
		if (eval > alpha)
		{
			moveSeq[depth - 1] = move;
			alpha = eval;
		}

	}

	return alpha;

}

int Bot::playWell(BoardManager* board)
{
	nbMoves = 0;
	int eval = search(board, botDepth, -infinity, infinity);
	int move = moveSeq[botDepth - 1];

	printf("NBMove : %d\n", nbMoves);

	if (move != -1)
	{
		board->makeMove(move);
	}

	printf("--> %s   :  %d\n", standardNotation(move).c_str(), eval);;

	return move;
}

int Bot::playRandom(BoardManager* board)
{
	std::vector<int> moves = board->generateMoves();

	if (moves.size() == 0)
	{
		return -1;
	}

	int random = rand() % moves.size();
	int move = moves[random];
	board->makeMove(move);

	return move;
}