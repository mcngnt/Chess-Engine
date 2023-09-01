#include "bot.h"



Bot::Bot()
{
}


int Bot::getDurationFromStart()
{
	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}


int Bot::evaluate(BoardManager* board)
{
	int score = 0;

	for (int i = 0 ; i < 64 ; ++i)
	{
		int piece = board->get(i);
		int pType = pieceType(piece);
		
		if (pType > None)
		{
			if (isPieceWhite(piece))
			{
				score += pieceValues[pType - 1];
			}
			else
			{
				score -= pieceValues[pType - 1];
			}

		}
		

	}


	return score * (board->whiteToMove ? 1 : -1);
}


int Bot::search(BoardManager* board,int alpha, int beta, int depth, int plyFromRoot)
{
	nodes++;

	int nodeType = AlphaNode;
	int bestEval = -999999;
	int bestMove = 0;
	bool inCheck = board->isChecked();
	bool inQuiescence = depth <= 0;

	Transposition t = transpositionTable.get(board->zobristKey, depth, alpha, beta);
	if (t.isValid)
	{
		return t.value;
	}

	if (getDurationFromStart() > maxTime)
	{
		return 999999;
	}

	if (inQuiescence)
	{
		bestEval = evaluate(board);
        if (bestEval > alpha)
        {
            alpha = bestEval;
        }
        if (alpha >= beta)
        {
            return bestEval;
        }
	}


	std::vector<int> moves = board->generateMoves(inQuiescence && !inCheck);

	std::vector<std::pair<int, int>> moveScorePair(moves.size());
	for (int i = 0; i < (int)moves.size(); ++i){
		int move = moves[i];
	    moveScorePair[i] = std::make_pair(move, 

	    	-(   move == t.bestMove ? 9000000 : tag(move) == Capture ? 1000000 * pieceType(board->get(endPos(move))) - pieceType(board->get(startPos(move))) : 0 )

	    	);
	}

	sort(moveScorePair.begin(), moveScorePair.end(), moveOrdering());



	if (moves.size() == 0)
	{
		if (inCheck)
		{
			return plyFromRoot - 999999;
		}
		return 0;
	}


	for (int moveIndex = 0; moveIndex < (int)moves.size(); ++moveIndex)
	{

		int move = moveScorePair[moveIndex].first;

		board->makeMove(move);
		int eval = -search(board,-beta, -alpha, depth - 1, plyFromRoot + 1);
		board->unmakeMove(move);


		if (eval > bestEval)
		{
			bestEval = eval;


			if (eval > alpha)
			{
				nodeType = ExactNode;
				bestMove = move;
				alpha = eval;

				if (plyFromRoot == 0)
				{
					rootMove = bestMove;
				}

			}

			if (alpha >= beta)
			{
				nodeType = BetaNode;
				break;
			}
		}

	}

	transpositionTable.set(board->zobristKey, depth, bestEval, nodeType, bestMove);


	return bestEval;

}



int Bot::play(BoardManager* board)
{

	// std::cout << "Playing ! " << std::endl;

	startTime = std::chrono::high_resolution_clock::now();

	int alpha = -999999;
	int beta = 999999;

	for (int d = 1; d < 90; d++)
	{
	    nodes = 0;
	    int eval = search(board, alpha, beta, d, 0);
	            
	    if (getDurationFromStart() > maxTime)
	    {
	    	break;
	    }

	  	std::cout << "info Depth : " << d << "  ||  Eval : " << eval << "  ||  Nodes : " << nodes << " || Best Move : " << standardNotation(rootMove) << std::endl;


	}

	std::cout << "info --------------------------------------" << std::endl;

	return rootMove;

}
