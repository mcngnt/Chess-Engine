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
	int middleGame = 0;
	int endGame = 0;
	int phase = 0;

	for (int i = 0 ; i < 64 ; ++i)
	{
		int piece = board->get(i);
		int pType = pieceType(piece);
		
		if (pType > None)
		{
			int pscore = pieceValues[pType - 1];
			phase += piecePhaseValue[pType - 1];
			if (isPieceWhite(piece))
			{
				// std::cout << "Piece type : " << pType << " Index : " << (int)(i ^ 56) << std::endl;
				middleGame += pieceTable[pType - 1][i ^ 56] + pscore;
				endGame += pieceTable[pType - 1 + 6][i ^56] + pscore;
			}
			else
			{
				middleGame -= pieceTable[pType - 1][i] + pscore;
				endGame -= pieceTable[pType - 1 + 6][i] + pscore;
			}

		}
		

	}

	// std::cout << (middleGame * phase + endGame * (24 - phase)) / 24 * (board->whiteToMove ? 1 : -1) << std::endl;
	// std::cout << phase << std::endl;


	return (middleGame * phase + endGame * (24 - phase)) / 24 * (board->whiteToMove ? 1 : -1);
}


int Bot::search(BoardManager* board, int alpha, int beta, int depth, int plyFromRoot)
 {
 	nodes++;

 	if(board->isRepetitionDraw() && plyFromRoot > 0)
 	{
 		return 0;
 	}

 	Transposition ttEntry = transpositionTable.get(board->zobristKey, depth, alpha, beta);

    bool inQuiescence = depth <= 0;
    bool inCheck = board->isChecked();
    // bool canPrune = false;
    int bestEval = -999999;
    int currentNodeType = AlphaNode;
    int bestMove = 0;
    int eval = 0;

    if (depth > 2 && getDurationFromStart() > maxTime)
    {
        return 999999;
    }


    if (plyFromRoot > 0 && ttEntry.isValid)
    {
        return ttEntry.value;
    }

    if (inCheck)
    {
        depth++;
    }

    if (inQuiescence)
    {
        bestEval = evaluate(board);
        if (bestEval > alpha)
            alpha = bestEval;
        if (alpha >= beta)
            return bestEval;
    }


/*    else if (!inCheck && beta - alpha == 1)
    {
        if (depth <= 6 && Evaluate() - depth * 165 > beta)
            return beta;
        canPrune = depth <= 6 && Evaluate() + depth * 165 < alpha;
        if (depth >= 2)
        {
            board.ForceSkipTurn();
            LambdaSearch(beta,previousMove, 3 + depth / 3);
            board.UndoSkipTurn();
            if (eval > beta)
                return beta;
        }
    }*/



        // foreach (Move move in moves)
        //     scores[scoreIter++] = -(move == entry.bestMove ? 9000000 :  move.IsCapture ? 1000000 * (int)move.CapturePieceType - (int)move.MovePieceType : killerMoves[plyFromRoot] == move ? 900000 : counterMoves[isWhiteMult, (int)previousMove.MovePieceType, previousMove.TargetSquare.Index] == move ? 800000 : historyHeuristicTable[isWhiteMult,(int)move.MovePieceType, move.TargetSquare.Index]);


    std::vector<int> moves = board->generateMoves(inQuiescence && !inCheck);

    if (moves.size() == 0)
	{
		if (inCheck)
		{
			return plyFromRoot - 999999;
		}
		if (!inQuiescence)
		{
			return 0;
		}
	}

	std::vector<std::pair<int, int>> moveScorePair(moves.size());
	for (int i = 0; i < (int)moves.size(); ++i){
		int move = moves[i];
	    moveScorePair[i] = std::make_pair(move, 

	    	-(

	    		move == ttEntry.bestMove ? 9000000 : tag(move) == Capture ? 1000000 * pieceType(board->get(endPos(move))) - pieceType(board->get(startPos(move))) : killerMoves[plyFromRoot] == move ? 900000 : 0

	    	 )

	   );
	}

	sort(moveScorePair.begin(), moveScorePair.end(), moveOrdering());



    for (int moveCount = 0; moveCount < (int)moves.size(); ++moveCount)
	{

		int move = moveScorePair[moveCount].first;



         // if (canPrune && moveCount > 1 && !move.IsCapture)
         // {
         //      continue;
         // }

            // board.MakeMove(move);


            // if (moveCount++ == 0 || isQuiescence)
            //     LambdaSearch(beta, move);
            // else
            //     if ((moveCount >= 5 && depth >= 2 ? LambdaSearch(alpha + 1, move, 3) : 999999) > alpha && LambdaSearch(alpha + 1, move) > alpha)
            //     LambdaSearch(beta, move);

            // board.UndoMove(move);



		board->makeMove(move);



/*		if (moveCount++ == 0 || isQuiescence)
                LambdaSearch(beta, allowNullMove);
            else
            {
                if (moveCount >= 5 && depth >= 2)
                    LambdaSearch(alpha + 1, allowNullMove, 3);
                else
                    eval = alpha + 1;
                if (eval > alpha)
                {
                    LambdaSearch(alpha + 1, allowNullMove);
                    if (eval > alpha)
                        LambdaSearch(beta, allowNullMove);
                }
            }*/

		// int LambdaSearch(int alphaBis, bool allowNull, int R = 1) => eval = -Search(-alphaBis, -alpha, depth - R, plyFromRoot + 1, allowNull);



		if( moveCount == 0 || inQuiescence )
		{
			eval = -search(board,-beta, -alpha, depth - 1, plyFromRoot + 1);
		}
		else
		{
			if(moveCount >= 4 && depth >= 2)
			{
				eval = -search(board,-alpha - 1, -alpha, depth - 2, plyFromRoot + 1);
			}
			else
			{
				eval = alpha + 1;
			}

			if(eval > alpha)
			{
				eval = -search(board,-alpha - 1, -alpha, depth - 1, plyFromRoot + 1);
				if (eval > alpha)
				{
					eval = -search(board,-beta, -alpha, depth - 1, plyFromRoot + 1);
				}
			}
		}



		board->unmakeMove(move);

		// if(plyFromRoot == 0)
		// {
		// 	// std::cout << standardNotation(move) << std::endl;
		// 	std::cout << "Eval " << standardNotation(move) << " : " << eval << std::endl;
		// }


        if (eval > bestEval)
         {

             bestEval = eval;


             if (eval > alpha)
             {
                 currentNodeType = ExactNode;
                 bestMove = move;
                 alpha = eval;
                 if (plyFromRoot == 0)
                 {
                     rootMove = bestMove;
                 }
             }


             if (alpha >= beta)
             {
                 currentNodeType = BetaNode;
                 if (tag(move) != Capture)
                 {
                     killerMoves[plyFromRoot] = move;
                     //historyHeuristicTable[isWhiteMult, (int)move.MovePieceType, move.TargetSquare.Index] += depth * depth;
                     //counterMoves[isWhiteMult,(int)previousMove.MovePieceType, previousMove.TargetSquare.Index] = move;
                 }
                 break;
             }

         }
     }


        // entry = new(board.ZobristKey, bestEval, depth, bestMove == default ? entry.bestMove : bestMove, currentFlag);

     transpositionTable.set(board->zobristKey, depth, bestEval, currentNodeType, bestMove);

     return bestEval;

}






int Bot::play(BoardManager* board)
{


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

	std::cout << "info bestmove : " << standardNotation(rootMove) << std::endl;

	std::cout << "info --------------------------------------" << std::endl;

	return rootMove;

}
