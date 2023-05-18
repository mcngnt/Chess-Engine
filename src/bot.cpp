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

	// for (int i = 0; i < maxBotDepth ; ++i)
	// {
	// 	if (move == moveSeq[i])
	// 	{
	// 		score += 10 * kingValue;
	// 		printf("Found %d %d \n", i, move);
	// 	}
	// }

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


std::vector<int> Bot::orderMoves(BoardManager* board, std::vector<int> moves, char depth)
{
	std::vector<MoveScore> movesScore;
	for (int move : moves)
	{
		MoveScore mscore;
		mscore.move = move;
		
		if (depth == currentDepth && move == bestMove)
		{
			mscore.score = infinity;
		}
		else
		{
			mscore.score = scoreMove(board, move);
		}

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


	return score;
}

int Bot::search(BoardManager* board, char depth, int alpha, int beta)
{

	char nodeType = AlphaNode;

	// board->computeZobrist();
	board->zobristKey =  board->computeZobrist();
	Transposition t = transpositionTable.get(board->zobristKey, depth, alpha, beta);
	if (t.isValid)
	{
		if (depth == currentDepth)
		{
			currentBestMove = t.bestMove;
		}
		return t.value;
	}


	if (depth == 0)
	{
		nbMoves += 1;
		return evaluate(board);
	}

	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	// std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;


	if (duration.count() > maxTime)
	{
		reachedTime = true;
		return 0;
	}


	std::vector<int> moves = board->generateMoves();
	std::vector<int> sortedMoves = orderMoves(board, moves, depth);



	if (sortedMoves.size() == 0)
	{
		if (board->isChecked())
		{
			return -3*kingValue;
		}
		return 0;
	}

	int bestPositionMove = 0;


	for (int move : sortedMoves)
	{

		board->makeMove(move);
		int eval = -search(board, depth - 1, -beta, -alpha);
		board->unmakeMove(move);

		// printf("%llu\n",board->zobristKey );
		board->zobristKey =  board->computeZobrist();
		// printf("%llu\n", board->zobristKey);
		// printf("%s\n", "---");



		if (eval >= beta)
		{
			transpositionTable.set(board->zobristKey, depth, beta, BetaNode, move);
			return beta;
		}
		if (eval > alpha)
		{
			nodeType = ExactNode;
			if (depth == currentDepth)
			{
				currentBestMove = move;
			}
			bestPositionMove = move;
			alpha = eval;
		}
	}

	transpositionTable.set(board->zobristKey, depth, alpha, nodeType, bestPositionMove);

	return alpha;

}

int Bot::playWell(BoardManager* board)
{
	nbMoves = 0;
	reachedTime = false;
	bestMove = 0;



	startTime = std::chrono::high_resolution_clock::now();


	// int move = 0;

	int eval = 0;

	for (char i = 1; i <= maxBotDepth; ++i)
	{
		currentDepth = i;
		eval = search(board, i, -infinity, infinity);
		if (!reachedTime)
		{
			// move = currentBestMove;
			bestMove = currentBestMove;

		}
		else
		{
			printf(" Last iter : %d\n", i);
			break;
		}

	}

	printf("NBMove : %d\n", nbMoves);

	if (bestMove != -1)
	{
		board->makeMove(bestMove);
	}

	printf("--> %s   :  %d\n", standardNotation(bestMove).c_str(), eval);;

	return bestMove;
}

int Bot::playRandom(BoardManager* board)
{
	std::vector<int> moves = board->generateMoves();

	if (moves.size() == 0)
	{
		return -1;
	}

	srand(2);
	int random = rand() % moves.size();
	int move = moves[random];
	board->makeMove(move);

	return move;
}