#include "boardManager.h"



int directions[8] = {North, South, East, West, NorthEast, NorthWest, SouthEast, SouthWest};
int numSquares[64][8];

int BoardManager::getPieceBitboardIndex(int pieceType, bool isPieceWhite)
{
	return isPieceWhite ? pieceType - 1 : pieceType + 5;
}

uint64_t BoardManager::getPieceBitboard(int pieceType, bool isPieceWhite)
{
	return piecesBitboard[getPieceBitboardIndex(pieceType, isPieceWhite)];
}

void BoardManager::togglePieceBitboard(int pieceType, bool isPieceWhite, int squareIndex)
{
	piecesBitboard[getPieceBitboardIndex(pieceType, isPieceWhite)] = toggleSquare(piecesBitboard[getPieceBitboardIndex(pieceType, isPieceWhite)], squareIndex);
}


bool BoardManager::isChecked()
{
	// whiteToMove = !whiteToMove;
    // controlledSquares();
    // whiteToMove = !whiteToMove;
    // if (whiteToMove)
    // {
    // 	return controlled[currentGameState.whiteKingPos / 8][currentGameState.whiteKingPos % 8];
    // }
    // else
    // {
    // 	return controlled[currentGameState.blackKingPos / 8][currentGameState.blackKingPos % 8];
    // }
    fillBitboardData();
    return isBitToggled(attackMap, whiteToMove ? currentGameState.whiteKingPos : currentGameState.blackKingPos);

}

bool BoardManager::isRepetitionDraw()
{

	if (historySize >= 1024)
	{
		return false;
	}
	for (int i = historySize - 2; i > 0; --i)
	{
		if(repetitionTable[i] == zobristKey)
		{
			return true;
		}
	}
	return false;

}


void BoardManager::makeNullMove()
{

	whiteToMove = !whiteToMove;

    uint64_t newZobristKey = zobristKey;
    newZobristKey ^= whiteToMoveZobrist;
    newZobristKey ^= doublePushFileZobrist[currentGameState.doublePushFile];
    newZobristKey ^= doublePushFileZobrist[0];

    GameState newGameState;
	newGameState.canBlackQueenCastle = currentGameState.canBlackQueenCastle;
	newGameState.canWhiteQueenCastle = currentGameState.canWhiteQueenCastle;
	newGameState.canWhiteKingCastle = currentGameState.canWhiteKingCastle;
	newGameState.canBlackKingCastle = currentGameState.canBlackKingCastle;
	newGameState.doublePushFile = 0;
	newGameState.whiteKingPos = currentGameState.whiteKingPos;
	newGameState.blackKingPos = currentGameState.blackKingPos;
	newGameState.hasWhiteCastled = currentGameState.hasWhiteCastled;
	newGameState.hasBlackCastled = currentGameState.hasBlackCastled;
	newGameState.zobristKey = newZobristKey;

    gameStateHistory.push(currentGameState);

    currentGameState = newGameState;
    zobristKey = newZobristKey;


    historySize++;

}


void BoardManager::unmakeNullMove()
{
	 whiteToMove = !whiteToMove;
     currentGameState = gameStateHistory.top();
     zobristKey = currentGameState.zobristKey;
     gameStateHistory.pop();
     historySize--;

}


BoardManager::BoardManager()
{
	for (int i = 0 ; i < 8 ; ++i)
	{
		for (int j = 0 ; j < 8 ; ++j)
		{
			int numNorth = j;
			int numSouth = 7 - j;
			int numEast = 7 - i;
			int numWest = i;
			numSquares[i + j*8][0] = numNorth; 
			numSquares[i + j*8][1] = numSouth; 
			numSquares[i + j*8][2] = numEast; 
			numSquares[i + j*8][3] = numWest;
			numSquares[i + j*8][4] = min(numNorth, numEast);
			numSquares[i + j*8][5] = min(numNorth, numWest);
			numSquares[i + j*8][6] = min(numSouth, numEast);
			numSquares[i + j*8][7] = min(numSouth, numWest);
		}
	}
	currentGameState.capturedPiece = 0;
	currentGameState.canWhiteKingCastle = true;
	currentGameState.canWhiteQueenCastle = true;
	currentGameState.canBlackKingCastle = true;
	currentGameState.canBlackQueenCastle = true;
	currentGameState.hasWhiteCastled = false;
	currentGameState.hasBlackCastled = false;
	currentGameState.doublePushFile = 0;
	currentGameState.moveCount = 0;

	loadFen(startingFen);
	initZobrist();
	computeZobrist();

	currentGameState.zobristKey = zobristKey;
}

bool BoardManager::isSquareEnemy(int sq)
{
	return isBitToggled(opponentPiecesBitboard,sq);
}
bool BoardManager::isSquareFriendly(int sq)
{
	return isBitToggled(friendlyPiecesBitboard,sq);
}
bool BoardManager::isSquareEmpty(int sq)
{
	return !isBitToggled(allPiecesBitboard,sq);
}
bool BoardManager::isSquareEmptyFriendly(int sq)
{
	return isSquareFriendly(sq) || isSquareEmpty(sq);
}
bool BoardManager::isSquareEmptyEnemy(int sq)
{
	return isSquareFriendly(sq) || isSquareEmpty(sq);
}

bool BoardManager::isSquareEnemy(int i, int j)
{
	return isBitToggled(opponentPiecesBitboard,i + 8 *j);
}
bool BoardManager::isSquareFriendly(int i, int j)
{
	return isBitToggled(friendlyPiecesBitboard,i + 8 *j);
}
bool BoardManager::isSquareEmpty(int i, int j)
{
	return !isBitToggled(allPiecesBitboard,i + 8 *j);
}
bool BoardManager::isSquareEmptyFriendly(int i, int j)
{
	return isSquareFriendly(i,j) || isSquareEmpty(i,j);
}
bool BoardManager::isSquareEmptyEnemy(int i, int j)
{
	return isSquareFriendly(i,j) || isSquareEmpty(i,j);
}


// void BoardManager::assign(int i, int j)
// {
// 	if (i >= 0 && i < 8 && j >= 0 && j < 8)
// 	{
// 		controlled[i][j] = true;
// 	}
// }

void BoardManager::fillBitboardData()
{
	attackMap = 0;
	friendlyPiecesBitboard = piecesBitboard[0] | piecesBitboard[1] | piecesBitboard[2] | piecesBitboard[3] | piecesBitboard[4] | piecesBitboard[5];
	opponentPiecesBitboard = piecesBitboard[6] | piecesBitboard[7] | piecesBitboard[8] | piecesBitboard[9] | piecesBitboard[10] | piecesBitboard[11];
	if(!whiteToMove)
	{
		uint64_t temp = friendlyPiecesBitboard;
		friendlyPiecesBitboard = opponentPiecesBitboard;
		opponentPiecesBitboard = temp;
	}
	allPiecesBitboard = friendlyPiecesBitboard | opponentPiecesBitboard;

	for (int pType = 1; pType <= 6; ++pType)
	{
	    uint64_t pMask = getPieceBitboard(pType, !whiteToMove);
	    while (pMask != 0)
	    {
			int sq = getAndClearLSB(&pMask);

			int i = sq % 8;
			int j = sq / 8;

			if (pType == Pawn)
			{
				if (!whiteToMove)
				{
					if (numSquares[sq][NorthEastID] >= 1 && isSquareEmptyFriendly(i+1,j-1))
					{
						// assign(j-1,i+1);
						attackMap |= (1 << ((i+1) + 8 * (j-1)));
					}
					if (numSquares[sq][NorthWestID] >= 1 && isSquareEmptyFriendly(i-1,j-1))
					{
						// assign(j-1,i-1);
						attackMap |= (1 << ((i-1) + 8 * (j-1)));
					}
				}
				else
				{
					if (numSquares[sq][SouthEastID] >= 1 && isSquareEmptyFriendly(i+1,j+1))
					{
						// assign(j+1, i+1);
						attackMap |= (1 << ((i+1) + 8 * (j+1)));
					}
					if (numSquares[sq][SouthWestID] >= 1 && isSquareEmptyFriendly(i-1,j+1))
					{
						// assign(j+1,i-1);
						attackMap |= (1 << ((i-1) + 8 * (j+1)));
					}
				}		
			}
			if (pType == King)
			{
				for (int dirID = 0 ; dirID <= 7 ; ++dirID)
				{
					int targetPos = sq + directions[dirID];
					if (numSquares[sq][dirID] >= 1 && isSquareEmptyFriendly(targetPos))
					{
						// assign(targetPos/8, targetPos % 8);
						attackMap |= (1 << targetPos);
					}
				}
			}
			if (pType == Knight)
			{
				if (numSquares[sq][NorthID] >= 2 && numSquares[sq][EastID] >= 1 && isSquareEmptyFriendly(i+1,j-2))
				{
			 		// assign(j-2,i+1);
			 		attackMap |= (1 << ((i+1) + 8 * (j-2)));
			 	}
				if (numSquares[sq][NorthID] >= 2 && numSquares[sq][WestID] >= 1 && isSquareEmptyFriendly(i-1,j-2))
				{
			 		// assign(j-2,i-1);
			 		attackMap |= (1 << ((i-1) + 8 * (j-2)));
			 	}
				if (numSquares[sq][SouthID] >= 2 && numSquares[sq][EastID] >= 1 && isSquareEmptyFriendly(i+1,j+2))
				{
			 		// assign(j+2,i+1);
			 		attackMap |= (1 << ((i+1) + 8 * (j+2)));
			 	}
				if (numSquares[sq][SouthID] >= 2 && numSquares[sq][WestID] >= 1 && isSquareEmptyFriendly(i-1,j+2))
				{
			 		// assign(j+2,i-1);
			 		attackMap |= (1 << ((i-1) + 8 * (j+2)));
			 	}
				if (numSquares[sq][EastID] >= 2 && numSquares[sq][NorthID] >= 1 && isSquareEmptyFriendly(i+2,j-1))
				{
			 		// assign(j-1,i+2);
			 		attackMap |= (1 << ((i+2) + 8 * (j-1)));
			 	}
				if (numSquares[sq][EastID] >= 2 && numSquares[sq][SouthID] >= 1 && isSquareEmptyFriendly(i+2,j+1))
				{
			 		// assign(j+1,i+2);
			 		attackMap |= (1 << ((i+2) + 8 * (j+1)));
			 	}
				if (numSquares[sq][WestID] >= 2 && numSquares[sq][NorthID] >= 1 && isSquareEmptyFriendly(i-2,j-1))
				{
			 		// assign(j-1,i-2);
			 		attackMap |= (1 << ((i+1) + 8 * (j-1)));
			 	}
				if (numSquares[sq][WestID] >= 2 && numSquares[sq][SouthID] >= 1 && isSquareEmptyFriendly(i-2,j+1))
				{
			 		// assign(j+1,i - 2);
			 		attackMap |= (1 << ((i-2) + 8 * (j+1)));
			 	}
			}
			if (pType == Rook || pType == Bishop || pType == Queen)
			{
				int startDir = (pType == Bishop) ? 4 : 0;
				int endDir = (pType == Rook) ? 3 : 7;
				for (int dirID = startDir ; dirID <= endDir ; ++dirID)
				{
					for (int i = 0 ; i < numSquares[sq][dirID]; ++i)
					{
						int targetPos = sq + directions[dirID] * (i+1);
						if (isSquareFriendly(targetPos))
						{
							break;
						}
						// assign(targetPos / 8, targetPos % 8);
						attackMap |= (1 << targetPos);
						if (isSquareEnemy(targetPos))
						{
							break;
						}
					}
				}
			}

		}
	}
}


// void BoardManager::controlledSquares()
// {

// 	resetControl();

// 	for (int i = 0 ; i < 8 ; ++i)
// 	{
// 		for (int j = 0 ; j < 8 ; ++j)
// 		{
// 			int piece = get(i,j);
// 			int sq = pid(i,j);

// 			if(piece > 0 && isPieceWhite(piece) == whiteToMove)
// 			{

// 				if (pieceType(piece) == Pawn)
// 				{

// 					if (isPieceWhite(piece))
// 					{
// 						if (numSquares[sq][NorthEastID] >= 1 && isSquareFree(i+1,j-1))
// 						{
// 							assign(j-1,i+1);
// 						}
// 						if (numSquares[sq][NorthWestID] >= 1 && isSquareFree(i-1,j-1))
// 						{
// 							assign(j-1,i-1);
// 						}

// 					}
// 					else
// 					{
// 						if (numSquares[sq][SouthEastID] >= 1 && isSquareFree(i+1,j+1))
// 						{
// 							assign(j+1, i+1);
// 						}
// 						if (numSquares[sq][SouthWestID] >= 1 && isSquareFree(i-1,j+1))
// 						{
// 							assign(j+1,i-1);
// 						}
// 					}		

// 				}

// 				if (pieceType(piece) == King)
// 				{

// 					for (int dirID = 0 ; dirID <= 7 ; ++dirID)
// 					{
// 						int targetPos = sq + directions[dirID];

// 						if (numSquares[sq][dirID] >= 1 && isSquareFree(targetPos))
// 						{
// 							assign(targetPos/8, targetPos % 8);
// 						}
// 					}

// 				}

// 				if (pieceType(piece) == Knight)
// 				{

// 					if (numSquares[sq][NorthID] >= 2 && numSquares[sq][EastID] >= 1 && isSquareFree(i+1,j-2))
// 					{
//  						assign(j-2,i+1);
//  					}
// 					if (numSquares[sq][NorthID] >= 2 && numSquares[sq][WestID] >= 1 && isSquareFree(i-1,j-2))
// 					{
//  						assign(j-2,i-1);
//  					}
// 					if (numSquares[sq][SouthID] >= 2 && numSquares[sq][EastID] >= 1 && isSquareFree(i+1,j+2))
// 					{
//  						assign(j+2,i+1);
//  					}
// 					if (numSquares[sq][SouthID] >= 2 && numSquares[sq][WestID] >= 1 && isSquareFree(i-1,j+2))
// 					{
//  						assign(j+2,i-1);
//  					}

// 					if (numSquares[sq][EastID] >= 2 && numSquares[sq][NorthID] >= 1 && isSquareFree(i+2,j-1))
// 					{
//  						assign(j-1,i+2);
//  					}
// 					if (numSquares[sq][EastID] >= 2 && numSquares[sq][SouthID] >= 1 && isSquareFree(i+2,j+1))
// 					{
//  						assign(j+1,i+2);
//  					}
// 					if (numSquares[sq][WestID] >= 2 && numSquares[sq][NorthID] >= 1 && isSquareFree(i-2,j-1))
// 					{
//  						assign(j-1,i-2);
//  					}
// 					if (numSquares[sq][WestID] >= 2 && numSquares[sq][SouthID] >= 1 && isSquareFree(i-2,j+1))
// 					{
//  						assign(j+1,i - 2);
//  					}

// 				}

// 				if (pieceType(piece) == Rook || pieceType(piece) == Bishop || pieceType(piece) == Queen)
// 				{

// 					int startDir = (pieceType(piece) == Bishop) ? 4 : 0;
// 					int endDir = (pieceType(piece) == Rook) ? 3 : 7;

// 					for (int dirID = startDir ; dirID <= endDir ; ++dirID)
// 					{
// 						for (int i = 0 ; i < numSquares[sq][dirID]; ++i)
// 						{
// 							int targetPos = sq + directions[dirID] * (i+1);

// 							if (isSquareFriendly(targetPos))
// 							{
// 								break;
// 							}

// 							assign(targetPos / 8, targetPos % 8);

// 							if (isSquareEnemy(targetPos))
// 							{
// 								break;
// 							}

// 						}
// 					}

// 				}

// 			}
// 		}
// 	}

// }

// void BoardManager::resetControl()
// {
// 	for (int i = 0 ; i <= 7; ++i)
// 	{
// 		for (int j = 0; j <= 7; ++j)
// 		{
// 			controlled[i][j] = false;
// 		}
// 	}
// }



std::vector<int> BoardManager::generateMoves(bool onlyCaptures)
{
	std::vector<int> pseudoMoves = generatePseudoMoves();

	std::vector<int> legalMoves;

	for (int pseudoMove : pseudoMoves)
	{
		if ((onlyCaptures && !isCapturingTag(getTag(pseudoMove))))
		{
			continue;
		}
		makeMove(pseudoMove);

		whiteToMove = !whiteToMove;

		if (!isChecked())
		{
			legalMoves.push_back(pseudoMove);
		}

		whiteToMove = !whiteToMove;

		unmakeMove(pseudoMove);
	}

	return legalMoves;
}


std::vector<int> BoardManager::generatePseudoMoves()
{
	std::vector<int> moves;

	fillBitboardData();


	for (int pType = 1; pType <= 6; ++pType)
	{
	    uint64_t pMask = getPieceBitboard(pType, whiteToMove);
        while (pMask != 0)
        {

			int piece = (whiteToMove ? White : Black) | pType;
			int sq = getAndClearLSB(&pMask);
			int i = sq % 8;
			int j = sq / 8;


				///// King ////

				if (pieceType(piece) == King)
				{

					for (int dirID = 0 ; dirID <= 7 ; ++dirID)
					{
						int targetPos = sq + directions[dirID];

						if (numSquares[sq][dirID] >= 1 && isSquareEmptyEnemy(targetPos))
						{
							moves.push_back(genMove(sq, targetPos, isSquareEnemy(targetPos) * Capture));
						}
					}

					if (!isBitToggled(attackMap, sq))
					{
						if (    (currentGameState.canWhiteQueenCastle && whiteToMove) ||  (currentGameState.canBlackQueenCastle && !whiteToMove))
						{
							bool isOK = true;
							if (!isSquareEmpty(1,j))
							{
								isOK = false;
							}
							for (int p = 2; p <= 3; ++p )
							{
								if (!isSquareEmpty(p, j) || isBitToggled(attackMap, p + 8 * j))
								{
									isOK = false;
								}
							}
							if (isOK)
							{
								moves.push_back(genMove(sq, sq - 2, QueenCastle));
							}
						}
						if (   (currentGameState.canWhiteKingCastle && whiteToMove) ||  (currentGameState.canBlackKingCastle && !whiteToMove) )
						{
							bool isOK = true;
							for (int p = 5; p <= 6; ++p )
							{
								if (!isSquareEmpty(p, j) || isBitToggled(attackMap, p + 8 * j))
								{
									isOK = false;
								}
							}
							if (isOK)
							{
								moves.push_back(genMove(sq, sq + 2, KingCastle));
							}
						}
					}

				}

				///// King ////

				///// Sliding pieces ////

				if (pieceType(piece) == Rook || pieceType(piece) == Bishop || pieceType(piece) == Queen)
				{

					int startDir = (pieceType(piece) == Bishop) ? 4 : 0;
					int endDir = (pieceType(piece) == Rook) ? 3 : 7;

					for (int dirID = startDir ; dirID <= endDir ; ++dirID)
					{
						for (int i = 0 ; i < numSquares[sq][dirID]; ++i)
						{
							int targetPos = sq + directions[dirID] * (i+1);

							if (isSquareFriendly(targetPos))
							{
								break;
							}


							moves.push_back(genMove(sq, targetPos, isSquareEnemy(targetPos) * Capture));

							if (isSquareEnemy(targetPos))
							{
								break;
							}

						}
					}

				}

				///// Sliding pieces ////

				///// Knight ////

				if (pieceType(piece) == Knight)
				{

					if (numSquares[sq][NorthID] >= 2 && numSquares[sq][EastID] >= 1 && isSquareEmptyEnemy(i+1,j-2))
					{
						moves.push_back(genMove(i,j,i+1,j-2, isSquareEnemy(i+1,j-2) * Capture));
					}
					if (numSquares[sq][NorthID] >= 2 && numSquares[sq][WestID] >= 1 && isSquareEmptyEnemy(i-1,j-2))
					{
						moves.push_back(genMove(i,j,i-1,j-2, isSquareEnemy(i-1,j-2) * Capture));
					}
					if (numSquares[sq][SouthID] >= 2 && numSquares[sq][EastID] >= 1 && isSquareEmptyEnemy(i+1,j+2))
					{
						moves.push_back(genMove(i,j,i+1,j+2, isSquareEnemy(i+1,j+2) * Capture));
					}
					if (numSquares[sq][SouthID] >= 2 && numSquares[sq][WestID] >= 1 && isSquareEmptyEnemy(i-1,j+2))
					{
						moves.push_back(genMove(i,j,i-1,j+2, isSquareEnemy(i-1,j+2) * Capture));
					}

					if (numSquares[sq][EastID] >= 2 && numSquares[sq][NorthID] >= 1 && isSquareEmptyEnemy(i+2,j-1))
					{
						moves.push_back(genMove(i,j,i+2,j-1, isSquareEnemy(i+2,j-1) * Capture));
					}
					if (numSquares[sq][EastID] >= 2 && numSquares[sq][SouthID] >= 1 && isSquareEmptyEnemy(i+2,j+1))
					{
						moves.push_back(genMove(i,j,i+2,j+1, isSquareEnemy(i+2,j+1) * Capture));
					}
					if (numSquares[sq][WestID] >= 2 && numSquares[sq][NorthID] >= 1 && isSquareEmptyEnemy(i-2,j-1))
					{
						moves.push_back(genMove(i,j,i-2,j-1, isSquareEnemy(i-2,j-1) * Capture));
					}
					if (numSquares[sq][WestID] >= 2 && numSquares[sq][SouthID] >= 1 && isSquareEmptyEnemy(i-2,j+1))
					{
						moves.push_back(genMove(i,j,i-2,j+1, isSquareEnemy(i-2,j+1) * Capture));
					}

				}

				///// Knight ////

				///// Pawns ////

				if (pieceType(piece) == Pawn)
				{

					if (isPieceWhite(piece))
					{
						if (numSquares[sq][NorthID] >= 1 && isSquareEmpty(i,j-1))
						{
							if (j > 1)
							{
								moves.push_back(genMove(i,j,i,j-1, QuietMove));
							}
							else
							{
								moves.push_back(genMove(i,j,i,j-1, KnightProm));
								moves.push_back(genMove(i,j,i,j-1, BishopProm));
								moves.push_back(genMove(i,j,i,j-1, RookProm));
								moves.push_back(genMove(i,j,i,j-1, QueenProm));
							}
						}
						if (j == 6 && isSquareEmpty(i,j-2) && isSquareEmpty(i, j-1))
						{
							moves.push_back(genMove(i,j,i,j-2, DoublePawnPush));
						}
						if (numSquares[sq][NorthEastID] >= 1 && isSquareEnemy(i+1,j-1))
						{
							if (j - 1 == 0)
							{
								moves.push_back(genMove(i,j,i+1,j-1, KnightPromCapture));
								moves.push_back(genMove(i,j,i+1,j-1, BishopPromCapture));
								moves.push_back(genMove(i,j,i+1,j-1, RookPromCapture));
								moves.push_back(genMove(i,j,i+1,j-1, QueenPromCapture));
							}
							else
							{
								moves.push_back(genMove(i,j,i+1,j-1, Capture));
							}
						}
						if (numSquares[sq][NorthWestID] >= 1 && isSquareEnemy(i-1,j-1))
						{
							if (j - 1 == 0)
							{
								moves.push_back(genMove(i,j,i-1,j-1, KnightPromCapture));
								moves.push_back(genMove(i,j,i-1,j-1, BishopPromCapture));
								moves.push_back(genMove(i,j,i-1,j-1, RookPromCapture));
								moves.push_back(genMove(i,j,i-1,j-1, QueenPromCapture));
							}
							else
							{
								moves.push_back(genMove(i,j,i-1,j-1, Capture));
							}
						}

						if (numSquares[sq][NorthEastID] >= 1 && j == 3 && isSquareEnemy(i+1,j) && pieceType(get(i+1,j)) == Pawn && isSquareEmpty(i+1, j-1) && currentGameState.doublePushFile - 1 == i+1)
						{
							moves.push_back(genMove(i,j,i+1,j-1, EPCapture));
						}
						if (numSquares[sq][NorthWestID] >= 1 && j == 3 && isSquareEnemy(i-1,j) && pieceType(get(i-1,j)) == Pawn && isSquareEmpty(i-1, j-1) && currentGameState.doublePushFile - 1 == i-1)
						{
							moves.push_back(genMove(i,j,i-1,j-1, EPCapture));
						}

					}
					else
					{
						if (numSquares[sq][NorthID] >= 1 && isSquareEmpty(i,j+1))
						{
							if (j < 6)
							{
								moves.push_back(genMove(i,j,i,j+1, QuietMove));
							}
							else
							{
								moves.push_back(genMove(i,j,i,j+1, KnightProm));
								moves.push_back(genMove(i,j,i,j+1, BishopProm));
								moves.push_back(genMove(i,j,i,j+1, RookProm));
								moves.push_back(genMove(i,j,i,j+1, QueenProm));
							}
						}
						if (j == 1 && isSquareEmpty(i,j+2) && isSquareEmpty(i, j+1))
						{
							moves.push_back(genMove(i,j,i,j+2, DoublePawnPush));
						}
						if (numSquares[sq][SouthEastID] >= 1 && isSquareEnemy(i+1,j+1))
						{
							if (j + 1 == 7)
							{
								moves.push_back(genMove(i,j,i+1,j+1, KnightPromCapture));
								moves.push_back(genMove(i,j,i+1,j+1, BishopPromCapture));
								moves.push_back(genMove(i,j,i+1,j+1, RookPromCapture));
								moves.push_back(genMove(i,j,i+1,j+1, QueenPromCapture));
							}
							else
							{
								moves.push_back(genMove(i,j,i+1,j+1, Capture));
							}
						}
						if (numSquares[sq][SouthWestID] >= 1 && isSquareEnemy(i-1,j+1))
						{
							if (j + 1 == 7)
							{
								moves.push_back(genMove(i,j,i-1,j+1, KnightPromCapture));
								moves.push_back(genMove(i,j,i-1,j+1, BishopPromCapture));
								moves.push_back(genMove(i,j,i-1,j+1, RookPromCapture));
								moves.push_back(genMove(i,j,i-1,j+1, QueenPromCapture));
							}
							else
							{
								moves.push_back(genMove(i,j,i-1,j+1, Capture));
							}
						}

						if (numSquares[sq][SouthEastID] >= 1 && j == 4 && isSquareEnemy(i+1,j)&& pieceType(get(i+1,j)) == Pawn && isSquareEmpty(i+1, j+1) && (currentGameState.doublePushFile - 1 == i+1))
						{
							moves.push_back(genMove(i,j,i+1,j+1, EPCapture));
						}
						if (numSquares[sq][SouthWestID] >= 1 && j == 4 && isSquareEnemy(i-1,j) && pieceType(get(i-1,j)) == Pawn && isSquareEmpty(i-1, j+1) && (currentGameState.doublePushFile - 1 == i-1))
						{
							moves.push_back(genMove(i,j,i-1,j+1, EPCapture));
						}

					}		

				}

				///// Pawn ////


		}
	}

	return moves;
}

int BoardManager::isLegal(std::vector<int> moves, int move)
{
	for (int movei : moves)
	{
		if (discardTag(movei) == move)
		{
			return movei;
		}
	}
	return 0;
}

void BoardManager::loadFen(std::string fen)
{
	gameStateHistory = std::stack<GameState>();
	historySize = 0;

	currentGameState.canBlackQueenCastle = false;
	currentGameState.canWhiteQueenCastle = false;
	currentGameState.canWhiteKingCastle = false;
	currentGameState.canBlackKingCastle = false;

	for (int ii = 0 ; ii <= 7; ++ii)
	{
		for (int jj = 0; jj <= 7; ++jj)
		{
			board[jj][ii] = None;
		}
	}

	int state = 0;
	int i = 0;
	int j = 0;

	bool hasReadNb = false;
	int nb = 0;

	for(char c : fen)
	{
		int currentID = i + 8 * j;
		if (c == ' ')
		{
			state +=1;
			continue;
		}
		if (state == 0)
		{
			switch (c)
			{
				case '/':
					 j +=1;
					 i = 0;
					 break;
				case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' :
					i += c - '0';
					break;
				case 'r':
					board[j][i] = Black | Rook;
					togglePieceBitboard(Rook, false, currentID);
					i += 1; 
					break;
				case 'n':
					board[j][i] = Black | Knight;
					togglePieceBitboard(Knight, false, currentID);
					i += 1; 
					break;
				case 'b':
					board[j][i] = Black | Bishop;
					togglePieceBitboard(Bishop, false, currentID);
					i += 1;
					break; 
				case 'q':
					board[j][i] = Black | Queen;
					togglePieceBitboard(Queen, false, currentID);
					i += 1; 
					break;
				case 'k':
					board[j][i] = Black | King;
					togglePieceBitboard(King, false, currentID);
					currentGameState.blackKingPos = i + j*8;
					i += 1;
					break; 
				case 'p':
					board[j][i] = Black | Pawn;
					togglePieceBitboard(Pawn, false, currentID);
					i += 1;
					break; 
				case 'R':
					board[j][i] = White | Rook;
					togglePieceBitboard(Rook, true, currentID);
					i += 1;
					break; 
				case 'N':
					board[j][i] = White | Knight;
					togglePieceBitboard(Knight, true, currentID);
					i += 1;
					break; 
				case 'B':
					board[j][i] = White | Bishop;
					togglePieceBitboard(Bishop, true, currentID);
					i += 1; 
					break;
				case 'Q':
					board[j][i] = White | Queen;
					togglePieceBitboard(Queen, true, currentID);
					i += 1; 
					break;
				case 'K':
					board[j][i] = White | King;
					togglePieceBitboard(King, true, currentID);
					currentGameState.whiteKingPos = i + j*8;
					i += 1; 
					break;
				case 'P':
					board[j][i] = White | Pawn;
					togglePieceBitboard(Pawn, true, currentID);
					i += 1; 
					break;

			}
		}
		if (state == 1)
		{
			whiteToMove = (c == 'w');
		}
		if (state == 2)
		{
			switch (c)
			{
				case 'Q':
					currentGameState.canWhiteQueenCastle = true;
					break;
				case 'K':
					currentGameState.canWhiteKingCastle = true;
					break;
				case 'q':
					currentGameState.canBlackQueenCastle = true;
					break;
				case 'k':
					currentGameState.canBlackKingCastle = true;
					break;
			}
		}
		if (state == 3)
		{
			switch (c)
			{
				case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' :
					currentGameState.doublePushFile = (c - '0');
					break;
			}
		}
		if (state == 4)
		{
			if (!hasReadNb)
			{
				nb = c - '0';
				hasReadNb = true;
			}
			else
			{
				nb *= 10;
				nb += c - '0';
			}
		}

	}
	currentGameState.moveCount = nb;
}


std::string BoardManager::convertFen()
{
	std::string fen = "";

	int emptyNB = 0;

	for (int j = 0 ; j < 8; ++j)
	{
		for (int i = 0 ; i < 8; ++i)
		{
			int piece = get(i,j);
			if (piece == None)
			{
				emptyNB += 1;
			}
			else
			{

				if (emptyNB > 0)
				{
					fen += '0' + emptyNB;
				}
				emptyNB = 0;

				switch (piece)
				{
					case Black | Rook:
						fen += 'r';
						break;
					case Black | Knight:
						fen += 'n';
						break;
					case Black | Bishop:
						fen += 'b';
						break; 
						break;
					case Black | Queen:
						fen += 'q';
						break; 
					case Black | King:
						fen += 'k';
						break;
					case Black | Pawn:
						fen += 'p';
						break;
					case White | Knight:
						fen += 'N';
						break;
					case White | Bishop:
						fen += 'B';
						break; 
					case White | Rook:
						fen += 'R';
						break;
					case White | Queen:
						fen += 'Q';
						break; 
					case White | King:
						fen += 'K';
						break;
					case White | Pawn:
						fen += 'P';
						break;

				}
			}
		}


		if (emptyNB > 0)
		{
			fen += '0' + emptyNB;
		}
		emptyNB = 0;
		if (j < 7)
		{
			fen += '/';
		}
	}

	fen += ' ';
	if (whiteToMove)
	{
		fen += 'w';
	}
	else
	{
		fen += 'b';
	}

	fen += ' ';

	fen += '0' + currentGameState.doublePushFile;

	fen += ' ';

	fen += '1';

	return fen;

}



void BoardManager::makeMove(int move)
{

	int startPosi = startPos(move);
	int endPosi = endPos(move);
	int tag = getTag(move);

	int startPiece = get(startPosi);
	int endPiece = get(endPosi);

	int startPieceType = pieceType(startPiece);
	int endPieceType = pieceType(endPiece);

	int startPosX = startPosi % 8;
	int startPosY = startPosi / 8;
	int endPosX = endPosi % 8;
	int endPosY = endPosi / 8;

	GameState newGameState;
	newGameState.canBlackQueenCastle = currentGameState.canBlackQueenCastle;
	newGameState.canWhiteQueenCastle = currentGameState.canWhiteQueenCastle;
	newGameState.canWhiteKingCastle = currentGameState.canWhiteKingCastle;
	newGameState.canBlackKingCastle = currentGameState.canBlackKingCastle;
	newGameState.doublePushFile = 0;
	newGameState.whiteKingPos = currentGameState.whiteKingPos;
	newGameState.blackKingPos = currentGameState.blackKingPos;
	newGameState.hasWhiteCastled = currentGameState.hasWhiteCastled;
	newGameState.hasBlackCastled = currentGameState.hasBlackCastled;

	if (startPosi == endPosi)
	{
		return;
	}


	zobristKey ^= piecesZobrist[startPieceType][whiteToMove][startPosi];
	if (tag == Capture || tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		zobristKey ^= piecesZobrist[endPieceType][!whiteToMove][endPosi];
	}
	zobristKey ^= piecesZobrist[startPieceType][whiteToMove][endPosi];



	if (tag == Capture || tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		newGameState.capturedPiece = endPiece;
	}

	if (tag == QuietMove || tag == Capture)
	{
		if ( (startPosX == 0 && startPieceType == Rook) || (endPosX == 0 && endPieceType == Rook)  )
		{
			if (whiteToMove)
			{
				newGameState.canWhiteQueenCastle = false;
			}
			else
			{
				newGameState.canBlackQueenCastle = false;
			}
		}
		if (  (startPosX == 7 && startPieceType == Rook) || (endPosX == 7 && endPieceType == Rook)  )
		{
			if (whiteToMove)
			{
				newGameState.canWhiteKingCastle = false;
			}
			else
			{
				newGameState.canBlackKingCastle = false;
			}
		}
		if (startPieceType == King)
		{
			if (whiteToMove)
			{
				newGameState.canWhiteQueenCastle = false;
				newGameState.canWhiteKingCastle = false;
				newGameState.whiteKingPos = endPosi;
			}
			else
			{
				newGameState.canBlackQueenCastle = false;
				newGameState.canBlackKingCastle = false;
				newGameState.blackKingPos = endPosi;
			}
		}
		board[endPosY][endPosX] = board[startPosY][startPosX];
		board[startPosY][startPosX] = None;
		togglePieceBitboard(startPieceType, whiteToMove, startPosi);
		togglePieceBitboard(startPieceType, whiteToMove, endPosi);
		if (tag == Capture)
		{
			togglePieceBitboard(endPieceType, !whiteToMove, endPosi);
		}
	}

	if (tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		int piecePromoType = Queen;
		switch (tag)
		{
			case KnightPromCapture:
				piecePromoType = Knight;
				break;
			case BishopPromCapture:
				piecePromoType = Bishop;
				break;
			case RookPromCapture:
				piecePromoType = Rook;
				break;
			default:
				piecePromoType = Queen;
				break;
		}


		if (whiteToMove)
		{
			board[endPosY][endPosX] = White | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][1][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][1][endPosi];
		}
		else
		{
			board[endPosY][endPosX] = Black | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][0][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][0][endPosi];
		}
		board[startPosY][startPosX] = None;
		togglePieceBitboard(endPieceType, !whiteToMove, endPosi);
		togglePieceBitboard(Pawn, whiteToMove, startPosi);
		togglePieceBitboard(piecePromoType, whiteToMove, endPosi);
	}


	if (tag == DoublePawnPush)
	{
		board[endPosY][endPosX] = board[startPosY][startPosX];
		board[startPosY][startPosX] = None;
		togglePieceBitboard(Pawn, whiteToMove, startPosi);
		togglePieceBitboard(Pawn, whiteToMove, endPosi);
		newGameState.doublePushFile = startPosX + 1;
	}

	if (tag == EPCapture)
	{
		board[endPosY][endPosX] = board[startPosY][startPosX];
		board[startPosY][startPosX] = None;
		togglePieceBitboard(pieceType(board[startPosY][endPosX]), !whiteToMove, endPosX + 8 * startPosY);
		board[startPosY][endPosX] = None;
		togglePieceBitboard(Pawn, whiteToMove, startPosi);
		togglePieceBitboard(Pawn, whiteToMove, endPosi);
		zobristKey ^= piecesZobrist[Pawn][!whiteToMove][endPosX + 8 * startPosY];
	}

	if (tag == KingCastle || tag == QueenCastle)
	{
		board[endPosY][endPosX] = board[startPosY][startPosX];
		board[startPosY][startPosX] = None;
		togglePieceBitboard(King, whiteToMove, startPosi);
		togglePieceBitboard(King, whiteToMove, endPosi);
		if (whiteToMove)
		{
			newGameState.canWhiteKingCastle = false;
			newGameState.canWhiteQueenCastle = false;
			newGameState.whiteKingPos = endPosi;
			newGameState.hasWhiteCastled = true;
		}
		else
		{
			newGameState.canBlackKingCastle = false;
			newGameState.canBlackQueenCastle = false;
			newGameState.blackKingPos = endPosi;
			newGameState.hasBlackCastled = true;
		}
		if (tag == KingCastle)
		{
			board[startPosY][5] = board[startPosY][7];
			board[startPosY][7] = None;
			togglePieceBitboard(Rook, whiteToMove, 5 + startPosY * 8);
			togglePieceBitboard(Rook, whiteToMove, 7 + startPosY * 8);
			zobristKey ^= piecesZobrist[Rook][whiteToMove][7 + startPosY * 8];
			zobristKey ^= piecesZobrist[Rook][whiteToMove][5 + startPosY * 8];
		}
		else
		{
			board[startPosY][3] = board[startPosY][0];
			board[startPosY][0] = None;
			togglePieceBitboard(Rook, whiteToMove, 3 + startPosY * 8);
			togglePieceBitboard(Rook, whiteToMove, 0 + startPosY * 8);
			zobristKey ^= piecesZobrist[Rook][whiteToMove][startPosY * 8];
			zobristKey ^= piecesZobrist[Rook][whiteToMove][3 + startPosY * 8];
		}
	}

	if (tag == KnightProm || tag == RookProm || tag == BishopProm || tag == QueenProm)
	{
		int piecePromoType = Queen;
		switch (tag)
		{
			case KnightProm:
				piecePromoType = Knight;
				break;
			case BishopProm:
				piecePromoType = Bishop;
				break;
			case RookProm:
				piecePromoType = Rook;
				break;
			default:
				piecePromoType = Queen;
				break;
		}


		if (whiteToMove)
		{
			board[endPosY][endPosX] = White | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][1][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][1][endPosi];
		}
		else
		{
			board[endPosY][endPosX] = Black | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][0][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][0][endPosi];
		}
		board[startPosY][startPosX] = None;
		togglePieceBitboard(piecePromoType, whiteToMove, endPosi);
		togglePieceBitboard(Pawn, whiteToMove, startPosi);
	}

/*	if((piecesBitboard[getPieceBitboardIndex(Queen, true)] & piecesBitboard[getPieceBitboardIndex(Queen, false)]) != 0)
	{
		std::cout << tag << " " << startPieceType << " " << endPieceType << std::endl;
	}*/

	if (newGameState.canWhiteKingCastle != currentGameState.canWhiteKingCastle)
	{
		zobristKey ^= castlingRightZobrist[0];
	}
	if (newGameState.canWhiteQueenCastle != currentGameState.canWhiteQueenCastle)
	{
		zobristKey ^= castlingRightZobrist[1];
	}
	if (newGameState.canBlackKingCastle != currentGameState.canBlackKingCastle)
	{
		zobristKey ^= castlingRightZobrist[2];
	}
	if (newGameState.canBlackQueenCastle != currentGameState.canBlackQueenCastle)
	{
		zobristKey ^= castlingRightZobrist[3];
	}

	zobristKey ^= doublePushFileZobrist[currentGameState.doublePushFile];
	zobristKey ^= doublePushFileZobrist[newGameState.doublePushFile];
	zobristKey ^= whiteToMoveZobrist;

	newGameState.zobristKey = zobristKey;

	whiteToMove = !whiteToMove;


	gameStateHistory.push(currentGameState);

	currentGameState = newGameState;

	if (historySize < 1024)
	{
		repetitionTable[historySize] = zobristKey;
	}


	historySize++;
}

void BoardManager::unmakeMove(int move)
{
	int startPosi = startPos(move);
	int endPosi = endPos(move);
	int tag = getTag(move);

	int startPiece = get(startPosi);
	int endPiece = get(endPosi);

	int startPieceType = pieceType(startPiece);
	int endPieceType = pieceType(endPiece);


	int startPosX = startPosi % 8;
	int startPosY = startPosi / 8;
	int endPosX = endPosi % 8;
	int endPosY = endPosi / 8;

	if (startPosi == endPosi)
	{
		return;
	}

	if (tag == QuietMove || tag == DoublePawnPush)
	{
		board[startPosY][startPosX] = board[endPosY][endPosX];
		board[endPosY][endPosX] = None;
/*		std::cout << pieceType(board[startPosY][startPosX]) << std::endl;
		std::cout << endPieceType << std::endl;
		std::cout << "------------" << std::endl;*/
		togglePieceBitboard(endPieceType, !whiteToMove, startPosi);
		togglePieceBitboard(endPieceType, !whiteToMove, endPosi);
	}


	if (tag == Capture)
	{
		board[startPosY][startPosX] = board[endPosY][endPosX];
		board[endPosY][endPosX] = currentGameState.capturedPiece;
		togglePieceBitboard(endPieceType, !whiteToMove, startPosi);
		togglePieceBitboard(endPieceType, !whiteToMove, endPosi);
		togglePieceBitboard(pieceType(currentGameState.capturedPiece), whiteToMove, endPosi);
		// std::cout << pieceType(currentGameState.capturedPiece) << std::endl;
	}


	if (tag == EPCapture)
	{
		board[startPosY][startPosX] = board[endPosY][endPosX];
		board[endPosY][endPosX] = None;
		togglePieceBitboard(Pawn, !whiteToMove, startPosi);
		togglePieceBitboard(Pawn, !whiteToMove, endPosi);
		if (!whiteToMove)
		{
			board[endPosY + 1][endPosX] = Black | Pawn;
			togglePieceBitboard(Pawn, false, endPosX + 8 * (endPosY + 1));
		}
		else
		{
			board[endPosY - 1][endPosX] = White | Pawn;
			togglePieceBitboard(Pawn, true, endPosX + 8 * (endPosY - 1));
		}
	}

	if (tag == KingCastle || tag == QueenCastle)
	{
		board[startPosY][startPosX] = board[endPosY][endPosX];
		board[endPosY][endPosX] = None;
		togglePieceBitboard(King, !whiteToMove, startPosi);
		togglePieceBitboard(King, !whiteToMove, endPosi);
		if (tag == KingCastle)
		{
			board[startPosY][7] = board[startPosY][5];
			board[startPosY][5] = None;
			togglePieceBitboard(Rook, !whiteToMove, 5 + startPosY * 8);
			togglePieceBitboard(Rook, !whiteToMove, 7 + startPosY * 8);
		}
		else
		{
			board[startPosY][0] = board[startPosY][3];
			board[startPosY][3] = None;
			togglePieceBitboard(Rook, !whiteToMove, 0 + startPosY * 8);
			togglePieceBitboard(Rook, !whiteToMove, 3 + startPosY * 8);
		}
	}

	if (tag == KnightProm || tag == RookProm || tag == BishopProm || tag == QueenProm)
	{
		if (!whiteToMove)
		{
			board[startPosY][startPosX] = White | Pawn;
		}
		else
		{
			board[startPosY][startPosX] = Black | Pawn;
		}
		board[endPosY][endPosX] = None;
		togglePieceBitboard(Pawn, !whiteToMove, startPosi);
		switch(tag)
		{
			case KnightProm:
				togglePieceBitboard(Knight, !whiteToMove, endPosi);
				break;
			case RookProm:
				togglePieceBitboard(Rook, !whiteToMove, endPosi);
				break;
			case BishopProm:
				togglePieceBitboard(Bishop, !whiteToMove, endPosi);
				break;
			case QueenPromCapture:
				togglePieceBitboard(Queen, !whiteToMove, endPosi);
				break;
		}
	}

	if (tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		if (!whiteToMove)
		{
			board[startPosY][startPosX] = White | Pawn;
		}
		else
		{
			board[startPosY][startPosX] = Black | Pawn;
		}
		board[endPosY][endPosX] = currentGameState.capturedPiece;
		togglePieceBitboard(Pawn, !whiteToMove, startPosi);
		switch(tag)
		{
			case KnightPromCapture:
				togglePieceBitboard(Knight, !whiteToMove, endPosi);
				break;
			case RookPromCapture:
				togglePieceBitboard(Rook, !whiteToMove, endPosi);
				break;
			case BishopPromCapture:
				togglePieceBitboard(Bishop, !whiteToMove, endPosi);
				break;
			case QueenPromCapture:
				togglePieceBitboard(Queen, !whiteToMove, endPosi);
				break;
		}
		togglePieceBitboard(pieceType(currentGameState.capturedPiece), whiteToMove, endPosi);
	}

	if((piecesBitboard[getPieceBitboardIndex(Queen, true)] & piecesBitboard[getPieceBitboardIndex(Queen, false)]) != 0 && !foundBug)
	{
		std::cout << tag << " " << startPieceType << " " << endPieceType << " " << whiteToMove << " " << startPosi << " " << endPosi << std::endl;
		// std::cout << "----" << std::endl;
		foundBug = true;
		// 0 0 5 1 3 24
	}

	whiteToMove = !whiteToMove;

	currentGameState = gameStateHistory.top();
	zobristKey = currentGameState.zobristKey;
	gameStateHistory.pop();
	historySize--;

}

int BoardManager::get(int pos)
{
	if (pos >= 0 && pos < 64)
	{
		return board[pos / 8][pos % 8];
	}
	else
	{
		return 0;
	}
}

int BoardManager::get(int x, int y)
{
	if (x >= 0 && x < 8 && y  >= 0 && y < 8)
	{
		return board[y][x];
	}
	else
	{
		return 0;
	}
}

uint64_t BoardManager::computeZobrist()
{
	uint64_t newZobristKey = (uint64_t)0;

	for (int i = 0; i < 64; ++i)
	{
		int piece = get(i);
		if (piece > 0)
		{
			newZobristKey ^= piecesZobrist[pieceType(piece)][isPieceWhite(piece)][i];
		}
	}

	newZobristKey ^= doublePushFileZobrist[currentGameState.doublePushFile];


	if (!whiteToMove)
	{
		newZobristKey ^= whiteToMoveZobrist;
	}


	if (currentGameState.canWhiteKingCastle)
	{
		newZobristKey ^= castlingRightZobrist[0];
	}
	if (currentGameState.canWhiteQueenCastle)
	{
		newZobristKey ^= castlingRightZobrist[1];
	}
	if (currentGameState.canBlackKingCastle)
	{
		newZobristKey ^= castlingRightZobrist[2];
	}
	if (currentGameState.canBlackQueenCastle)
	{
		newZobristKey ^= castlingRightZobrist[3];
	}

	return newZobristKey;
}

void BoardManager::initZobrist()
{

    std::mt19937_64 gen(time(NULL));
    std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());

    for (int pType = 1; pType <= 6; ++pType)
    {
    	for (int color = 0 ; color <= 1; ++color)
    	{
    		for (int squareID = 0; squareID < 64; ++squareID)
    		{
    			piecesZobrist[pType][color][squareID] = dis(gen);
    		}
    	}
    }

    for (int i = 0 ; i < 9; ++i)
    {
    	doublePushFileZobrist[i] = dis(gen);
    }
    whiteToMoveZobrist = dis(gen);
    for (int i = 0; i < 4; ++i)
    {
    	castlingRightZobrist[i] = dis(gen);
    }

    zobristKey = computeZobrist();
}

