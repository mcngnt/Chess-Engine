#include "bot.h"



Bot::Bot()
{
}

int pieceValue(int ptype)
{
	switch (ptype)
	{
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
		case King:
			return kingValue;
			break;
	}
	return 0;
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


std::vector<int> Bot::orderMoves(BoardManager* board, std::vector<int> moves, char depth)
{
	std::vector<MoveScore> movesScore;
	for (int move : moves)
	{
		MoveScore mscore;
		mscore.move = move;

		if (move == PVmoves[currentDepth - depth])
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
	int move = 0;
	if (botType == Random)
	{
		move =  playRandom(board);
	}

	if (botType == Good)
	{
		move =  playWell(board);
	}

	if (move != -1)
	{
		board->makeMove(move);
	}

	return move;
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

int rotate(int i, bool whitePlaying)
{
	if (whitePlaying)
	{
		return i;
	}
	else
	{
		return ( (7 - (i%8)) + (i/8) * 7);
	}
}

int Bot::accessHeatMapMG(int pType,int i, bool whitePlaying)
{
	switch(pType)
	{
		case Pawn:
			return mg_pawn_table[rotate(i, whitePlaying)];
			break;
		case Knight:
			return mg_knight_table[rotate(i, whitePlaying)];
			break;
		case Bishop:
			return mg_bishop_table[rotate(i, whitePlaying)];
			break;
		case Rook:
			return mg_rook_table[rotate(i, whitePlaying)];
			break;
		case Queen:
			return mg_queen_table[rotate(i, whitePlaying)];
			break;
		case King:
			return mg_king_table[rotate(i, whitePlaying)];
			break;
	}
	return 0;
}

int Bot::accessHeatMapEG(int pType,int i, bool whitePlaying)
{
	switch(pType)
	{
		case Pawn:
			return eg_pawn_table[rotate(i, whitePlaying)];
			break;
		case Knight:
			return eg_knight_table[rotate(i, whitePlaying)];
			break;
		case Bishop:
			return eg_bishop_table[rotate(i, whitePlaying)];
			break;
		case Rook:
			return eg_rook_table[rotate(i, whitePlaying)];
			break;
		case Queen:
			return eg_queen_table[rotate(i, whitePlaying)];
			break;
		case King:
			return eg_king_table[rotate(i, whitePlaying)];
			break;
	}
	return 0;
}


int restrainKingEndGame(BoardManager* board, int myKingPos, int opponentKingPos)
{
	// On favorise un roi enemi proche du bord
	// On tente de rapprocher notre roi du roi adverse afin de limiter ses possibilités de mouvement

	int newScore = 0;

	int distCenterOpp = abs((opponentKingPos % 8) - 3) + abs((opponentKingPos / 8) - 3);
	int distCenterFriend = abs((myKingPos % 8) - 3) + abs((myKingPos / 8) - 3);

	newScore += distCenterOpp - distCenterFriend;

	int distKings = abs((myKingPos % 8) - (opponentKingPos % 8)) + abs((myKingPos / 8) - (opponentKingPos / 8));

	newScore += 2 * (14 - distKings);


	return newScore;

}



int Bot::evaluate(BoardManager* board)
{
	int score = 0;

	int whiteScoreValue = 0;
	int blackScoreValue = 0;

	int heatMapScoreMGWhite = 0;
	int heatMapScoreMGBlack = 0;
	int heatMapScoreEGWhite = 0;
	int heatMapScoreEGBlack = 0;

	// int whitePawnPerFile[8];
	// int blackPawnPerFile[8];

	int pieceNumber = 0;

	for (int i = 0 ; i < 64 ; ++i)
	{
		int piece = board->get(i);
		int pType = pieceType(piece);
		if (pType != None)
		{
			pieceNumber += 1;
		}
		/*if (pType == Pawn)
		{
			if(isPieceWhite(piece))
			{
				whitePawnPerFile[i % 8] += 1;
			}
			else
			{
				blackPawnPerFile[i % 8] += 1;
			}
		}*/
		if (isPieceWhite(piece))
		{
			whiteScoreValue += pieceValue(pType);
			heatMapScoreMGWhite += accessHeatMapMG(pType,i, board->whiteToMove);
			heatMapScoreEGWhite += accessHeatMapEG(pType,i, board->whiteToMove);
		}
		else
		{
			blackScoreValue += pieceValue(pType);
			heatMapScoreMGBlack += accessHeatMapMG(pType,i, board->whiteToMove);
			heatMapScoreEGBlack += accessHeatMapEG(pType,i, board->whiteToMove);
		}

	}

	float endGameWeight = 1.0 - (float(pieceNumber) / 32.0);

	// for (int k = 0 ; k < 8 ; ++k)
	// {
	// 	score -= int( ( (whitePawnPerFile[k] > 1) - (blackPawnPerFile[k] > 1)) * (board->whiteToMove ? 1 : -1) * 10);
	// }

	score += int((1.0 - endGameWeight) * ( (board->currentGameState.hasWhiteCastled) - (board->currentGameState.hasBlackCastled)) * (board->whiteToMove ? 1 : -1) * 50);

	score += int( 0.5 * ( (1.0 - endGameWeight) * (heatMapScoreMGWhite - heatMapScoreMGBlack) + endGameWeight * (heatMapScoreEGWhite - heatMapScoreEGBlack) )* (board->whiteToMove ? 1 : -1));

	score -= board->isChecked() * 50;

	if (board->whiteToMove)
	{
		score += int(restrainKingEndGame(board, board->currentGameState.whiteKingPos, board->currentGameState.blackKingPos) * 10 * endGameWeight);
	}
	else
	{
		score += int(restrainKingEndGame(board, board->currentGameState.blackKingPos, board->currentGameState.whiteKingPos) * 10 * endGameWeight);
	}


	score += (whiteScoreValue - blackScoreValue) * (board->whiteToMove ? 1 : -1);


	return score;
}

int Bot::quietSearch(BoardManager* board, int alpha, int beta)
{
	int eval = evaluate(board);
	if (eval >= beta)
	{
		return beta;
	}
	if (eval > alpha)
	{
		alpha = eval;
	}

	std::vector<int> moves = board->generateMoves(true);
	std::vector<int> sortedMoves = orderMoves(board, moves, 0);


	for (int move : sortedMoves)
	{
		board->makeMove(move);
		int eval = -quietSearch(board, -beta, -alpha);
		board->unmakeMove(move);

		nbQMoves += 1;

		if (eval >= beta)
		{
			return beta;
		}

		if (eval > alpha)
		{
			alpha = eval;
		}


	}

	return alpha;

}

int Bot::search(BoardManager* board, char depth, int alpha, int beta)
{

	char nodeType = AlphaNode;

	Transposition t = transpositionTable.get(board->zobristKey, depth, alpha, beta);
	if (t.isValid)
	{
		nbTranspo += 1;
		return t.value;
	}

	// int count = 0;
	// for (uint64_t z : board->zobristHistory)
	// {
	// 	if (z == board->zobristKey)
	// 	{
	// 		count += 1;
	// 	}
	// }

	// if (count >= 2)
	// {
	// 	return -1000;
	// }



	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	// std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

	if (depth == 0)
	{
		nbMoves += 1;
		// return evaluate(board);
		return quietSearch(board, alpha, beta);
	}

	if (duration.count() > maxTime)
	{
		reachedTime = true;
		return 0;
	}


	std::vector<int> moves = board->generateMoves(false);
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


		if (eval >= beta)
		{
			transpositionTable.set(board->zobristKey, depth, beta, BetaNode, move);
			return beta;
		}
		if (eval > alpha)
		{
			nodeType = ExactNode;
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
	nbTranspo = 0;
	reachedTime = false;
	nbQMoves = 0;



	startTime = std::chrono::high_resolution_clock::now();


	int eval = 0;
	int finalEval = 0;

	for (char i = 1; i <= maxBotDepth; ++i)
	{
		currentDepth = i;
		eval = search(board, i, -infinity, infinity);
		if (!reachedTime)
		{
			finalEval = eval;
			std::stack<int> moves;
			for (int j = 0; j < 1; ++j)
			{
				Transposition t = transpositionTable.get(board->zobristKey, 0, 0, 0);
				// printf("Depth : %d, PVNB : %d Node Type : %d Best move : %s\n",currentDepth, j, t.nodeType, standardNotation(t.bestMove).c_str() );
				if (t.isValid)
				{
					PVmoves[j] = t.bestMove;
					board->makeMove(t.bestMove);
					moves.push(t.bestMove);
				}
				else
				{
					break;
				}
			}
			while (!moves.empty())
			{
				board->unmakeMove(moves.top());
				moves.pop();
			}
		}
		else
		{
			break;
		}

	}


	printf("info Profondeur : %d\n", currentDepth - 1);
	printf("info Nombre positions evaluees : %d\n", nbMoves);
	printf("info Nombre transpositions rencontrees : %d\n", nbTranspo);
	printf("info Nombre d'entrees dans la table de transposition : %d\n", transpositionTable.count);
	printf("info Nombre de positions silencieuses evaluees : %d\n", nbQMoves);


	printf("info Mouvement --> %s  : eval =  %d\n", standardNotation(PVmoves[0]).c_str(), finalEval);                                                                                                                                                       

	printf("info ----------\n");

	transpositionTable.clear();

	return PVmoves[0];
}

int Bot::playRandom(BoardManager* board)
{
	std::vector<int> moves = board->generateMoves(false);

	if (moves.size() == 0)
	{
		return -1;
	}

	// srand(6);
	int random = rand() % moves.size();
	int move = moves[random];
	// board->makeMove(move);

	return move;
}