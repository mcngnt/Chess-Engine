#include "bot.h"

// int convertPos(char x, char y)
// {
// 	return (x - 'a') + 8 * (7 - (y - '1'));
// }

// int pieceTypeFromLetter(char c)
// {
// 	switch (c)
// 	{
// 		case 'N':
// 			return Knight;
// 			break;
// 		case 'Q':
// 			return Queen;
// 			break;
// 		case 'B':
// 			return Bishop;
// 			break;
// 		case 'K':
// 			return King;
// 			break;
// 		case 'R':
// 			return Rook;
// 			break;
// 	}
// 	return 0;
// }


// void Bot::addMove(std::string moveString, int openingNB, int moveNB)
// {
// 	// printf("%d\n",moveNB );
// 	int moveSize = moveString.size();

// 	if (moveSize == 2)
// 	{
// 		openingMoves[openingNB][moveNB].first = convertPos(moveString[0], moveString[1]);
// 		openingMoves[openingNB][moveNB].second = Pawn;
// 	}
// 	if (moveSize == 3)
// 	{
// 		if (moveString[0] == 'O')
// 		{
// 			openingMoves[openingNB][moveNB].second = King + 1;
// 		}
// 		else
// 		{
// 			openingMoves[openingNB][moveNB].first = convertPos(moveString[1], moveString[2]);
// 			openingMoves[openingNB][moveNB].second = pieceTypeFromLetter(moveString[0]);
// 		}
// 	}
// 	if (moveSize == 4)
// 	{
// 		openingMoves[openingNB][moveNB].second = King + 2;
// 	}
// }

Bot::Bot()
{
	// std::ifstream file("res/openings.txt");

    // if (file.is_open())
    // {
    //     std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //     file.close();

    //     int openingNB = 0;
    //     int moveNB = 0;

    //     std::string moveString;

    //     for (char c : content)
    //     {
    //     	if (c == '\n')
    //     	{
    //     		openingNB += 1;
    //     		moveNB = 0;
    //     		// std::cout << moveString << std::endl;
    //     		addMove(moveString, openingNB, moveNB);
    //     		moveString = "";
    //     		continue;
    //     	}
    //     	if (c == ' ')
    //     	{
    //     		moveNB += 1;
    //     		// std::cout << moveString << std::endl;
    //     		addMove(moveString, openingNB, moveNB);
    //     		moveString = "";
    //     		continue;
    //     	}
    //     	moveString += c;
    //     }
    // } 
    // else
    // {
    //     std::cout << "Unable to open the file." << std::endl;
    // }
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
		case None:
			return 0;
			break;
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

int restrainKingEndGame(BoardManager* board, int myKingPos, int opponentKingPos)
{
	// On favorise un roi enemi proche du bord
	// On tente de rapprocher notre roi du roi adverse afin de limiter ses possibilités de mouvement

	int newScore = 0;

	int distCenterOpp = abs((opponentKingPos % 8) - 3) + abs((opponentKingPos / 8) - 3);
	int distCenterFriend = abs((myKingPos % 8) - 3) + abs((myKingPos / 8) - 3);

	newScore += distCenterOpp - distCenterFriend;

	// int distKings = abs((myKingPos % 8) - (opponentKingPos % 8)) + abs((myKingPos / 8) - (opponentKingPos / 8));

	// newScore += 14 - distKings;


	return newScore;

}


int Bot::evaluate(BoardManager* board)
{
	int score = 0;

	int whiteScoreValue = 0;
	int blackScoreValue = 0;

	int heatMapScoreWhite = 0;
	int heatMapScoreBlack = 0;

	int pieceNumber = 0;

	for (int i = 0 ; i < 64 ; ++i)
	{
		int piece = board->get(i);
		int pType = pieceType(piece);
		if (pType != None)
		{
			pieceNumber += 1;
		}
		if (isPieceWhite(piece))
		{
			whiteScoreValue += pieceValue(pType);
			heatMapScoreWhite += accessHeatMap(pType, i%8, i/8,board->whiteToMove);
		}
		else
		{
			blackScoreValue += pieceValue(pType);
			heatMapScoreBlack += accessHeatMap(pType, i%8, i/8,board->whiteToMove);
		}

	}

	float endGameWeight = 1.0 - (float(pieceNumber) / 32.0);



	score += int( 0.5 * (1.0 - endGameWeight) * (heatMapScoreWhite - heatMapScoreBlack) * (board->whiteToMove ? 1 : -1));

	score += 20 * ((board->currentGameState.canWhiteKingCastle + board->currentGameState.canWhiteQueenCastle) - (board->currentGameState.canBlackKingCastle + board->currentGameState.canBlackQueenCastle)) * (board->whiteToMove ? 1 : -1);


	if (board->whiteToMove)
	{
		score += int(restrainKingEndGame(board, board->currentGameState.whiteKingPos, board->currentGameState.blackKingPos) * 10 * endGameWeight);
	}
	else
	{
		score += int(restrainKingEndGame(board, board->currentGameState.blackKingPos, board->currentGameState.whiteKingPos) * 10 * endGameWeight);
	}


	score += (whiteScoreValue - blackScoreValue) * (board->whiteToMove ? 1 : -1);

	// printf("%d\n", score);

	// printf("%d\n", score);
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
	// printf("QSEARCH\n");

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
		if (depth == currentDepth)
		{
			currentBestMove = t.bestMove;
		}
		return t.value;
	}



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
	nbTranspo = 0;
	reachedTime = false;
	bestMove = 0;
	nbQMoves = 0;



	startTime = std::chrono::high_resolution_clock::now();


	int eval = 0;

	for (char i = 1; i <= maxBotDepth; ++i)
	{
		currentDepth = i;
		eval = search(board, i, -infinity, infinity);
		if (!reachedTime)
		{
			bestMove = currentBestMove;

		}
		else
		{
			printf(" Last iter : %d\n", i);
			break;
		}

	}

	printf("NBMove : %d\n", nbMoves);
	printf("NBTranspo : %d\n", nbTranspo);
	printf("Count : %d\n", transpositionTable.count);
	printf("NBQMoves : %d\n", nbQMoves);

	if (bestMove != -1)
	{
		board->makeMove(bestMove);
	}

	printf("--> %s   :  %d\n", standardNotation(bestMove).c_str(), eval);;

	transpositionTable.clear();

	return bestMove;
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
	board->makeMove(move);

	return move;
}