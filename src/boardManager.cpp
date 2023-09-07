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
	whiteToMove = !whiteToMove;
    controlledSquares();
    whiteToMove = !whiteToMove;
    if (whiteToMove)
    {
    	return controlled[currentGameState.whiteKingPos / 8][currentGameState.whiteKingPos % 8];
    }
    else
    {
    	return controlled[currentGameState.blackKingPos / 8][currentGameState.blackKingPos % 8];
    }
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



bool BoardManager::isSquareEmpty(int i, int j)
{
	return get(i,j) == 0;
}

bool BoardManager::isSquareEmpty(int pid)
{
	return get(pid) == 0;
}

bool BoardManager::isSquareEnemy(int i, int j)
{
	return isPieceWhite(get(i,j)) != whiteToMove && (get(i,j) > 0);
}

bool BoardManager::isSquareEnemy(int pid)
{
	return isPieceWhite(get(pid)) != whiteToMove && (get(pid) > 0);
}

bool BoardManager::isSquareFree(int i, int j)
{
	return isPieceWhite(get(i,j)) != whiteToMove || (get(i,j) == 0);
}

bool BoardManager::isSquareFree(int pid)
{
	return isPieceWhite(get(pid)) != whiteToMove || (get(pid) == 0);
}

bool BoardManager::isSquareFriendly(int pid)
{
	return isPieceWhite(get(pid)) == whiteToMove && (get(pid) > 0);
}

void BoardManager::assign(int i, int j)
{
	if (i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		controlled[i][j] = true;
	}
}


void BoardManager::controlledSquares()
{

	resetControl();

	for (int i = 0 ; i < 8 ; ++i)
	{
		for (int j = 0 ; j < 8 ; ++j)
		{
			int piece = get(i,j);
			int currentPID = pid(i,j);

			if(piece > 0 && isPieceWhite(piece) == whiteToMove)
			{

				if (pieceType(piece) == Pawn)
				{

					if (isPieceWhite(piece))
					{
						if (numSquares[currentPID][NorthEastID] >= 1 && isSquareFree(i+1,j-1))
						{
							assign(j-1,i+1);
						}
						if (numSquares[currentPID][NorthWestID] >= 1 && isSquareFree(i-1,j-1))
						{
							assign(j-1,i-1);
						}

					}
					else
					{
						if (numSquares[currentPID][SouthEastID] >= 1 && isSquareFree(i+1,j+1))
						{
							assign(j+1, i+1);
						}
						if (numSquares[currentPID][SouthWestID] >= 1 && isSquareFree(i-1,j+1))
						{
							assign(j+1,i-1);
						}
					}		

				}

				if (pieceType(piece) == King)
				{

					for (int dirID = 0 ; dirID <= 7 ; ++dirID)
					{
						int targetPos = currentPID + directions[dirID];

						if (numSquares[currentPID][dirID] >= 1 && isSquareFree(targetPos))
						{
							assign(targetPos/8, targetPos % 8);
						}
					}

				}

				if (pieceType(piece) == Knight)
				{

					if (numSquares[currentPID][NorthID] >= 2 && numSquares[currentPID][EastID] >= 1 && isSquareFree(i+1,j-2))
					{
 						assign(j-2,i+1);
 					}
					if (numSquares[currentPID][NorthID] >= 2 && numSquares[currentPID][WestID] >= 1 && isSquareFree(i-1,j-2))
					{
 						assign(j-2,i-1);
 					}
					if (numSquares[currentPID][SouthID] >= 2 && numSquares[currentPID][EastID] >= 1 && isSquareFree(i+1,j+2))
					{
 						assign(j+2,i+1);
 					}
					if (numSquares[currentPID][SouthID] >= 2 && numSquares[currentPID][WestID] >= 1 && isSquareFree(i-1,j+2))
					{
 						assign(j+2,i-1);
 					}

					if (numSquares[currentPID][EastID] >= 2 && numSquares[currentPID][NorthID] >= 1 && isSquareFree(i+2,j-1))
					{
 						assign(j-1,i+2);
 					}
					if (numSquares[currentPID][EastID] >= 2 && numSquares[currentPID][SouthID] >= 1 && isSquareFree(i+2,j+1))
					{
 						assign(j+1,i+2);
 					}
					if (numSquares[currentPID][WestID] >= 2 && numSquares[currentPID][NorthID] >= 1 && isSquareFree(i-2,j-1))
					{
 						assign(j-1,i-2);
 					}
					if (numSquares[currentPID][WestID] >= 2 && numSquares[currentPID][SouthID] >= 1 && isSquareFree(i-2,j+1))
					{
 						assign(j+1,i - 2);
 					}

				}

				if (pieceType(piece) == Rook || pieceType(piece) == Bishop || pieceType(piece) == Queen)
				{

					int startDir = (pieceType(piece) == Bishop) ? 4 : 0;
					int endDir = (pieceType(piece) == Rook) ? 3 : 7;

					for (int dirID = startDir ; dirID <= endDir ; ++dirID)
					{
						for (int i = 0 ; i < numSquares[currentPID][dirID]; ++i)
						{
							int targetPos = currentPID + directions[dirID] * (i+1);

							if (isSquareFriendly(targetPos))
							{
								break;
							}

							assign(targetPos / 8, targetPos % 8);

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

}

void BoardManager::resetControl()
{
	for (int i = 0 ; i <= 7; ++i)
	{
		for (int j = 0; j <= 7; ++j)
		{
			controlled[i][j] = false;
		}
	}
}



std::vector<int> BoardManager::generateMoves(bool onlyCaptures)
{
	std::vector<int> pseudoMoves = generatePseudoMoves();

	std::vector<int> legalMoves;

	for (int pseudoMove : pseudoMoves)
	{
		if (onlyCaptures && !isCapturingTag(tag(pseudoMove)))
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

	whiteToMove = !whiteToMove;
	controlledSquares();
	whiteToMove = !whiteToMove;


	for (int i = 0 ; i < 8 ; ++i)
	{
		for (int j = 0 ; j < 8 ; ++j)
		{
			int piece = get(i,j);
			int currentPID = pid(i,j);

			if(piece > 0 && isPieceWhite(piece) == whiteToMove)
			{

				if (pieceType(piece) == Pawn)
				{

					if (isPieceWhite(piece))
					{
						if (numSquares[currentPID][NorthID] >= 1 && isSquareEmpty(i,j-1))
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
						if (numSquares[currentPID][NorthEastID] >= 1 && isSquareEnemy(i+1,j-1))
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
						if (numSquares[currentPID][NorthWestID] >= 1 && isSquareEnemy(i-1,j-1))
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

						if (numSquares[currentPID][NorthEastID] >= 1 && j == 3 && isSquareEnemy(i+1,j) && pieceType(get(i+1,j)) == Pawn && isSquareEmpty(i+1, j-1) && currentGameState.doublePushFile - 1 == i+1)
						{
							moves.push_back(genMove(i,j,i+1,j-1, EPCapture));
						}
						if (numSquares[currentPID][NorthWestID] >= 1 && j == 3 && isSquareEnemy(i-1,j) && pieceType(get(i-1,j)) == Pawn && isSquareEmpty(i-1, j-1) && currentGameState.doublePushFile - 1 == i-1)
						{
							moves.push_back(genMove(i,j,i-1,j-1, EPCapture));
						}

					}
					else
					{
						if (numSquares[currentPID][NorthID] >= 1 && isSquareEmpty(i,j+1))
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
						if (numSquares[currentPID][SouthEastID] >= 1 && isSquareEnemy(i+1,j+1))
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
						if (numSquares[currentPID][SouthWestID] >= 1 && isSquareEnemy(i-1,j+1))
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

						if (numSquares[currentPID][SouthEastID] >= 1 && j == 4 && isSquareEnemy(i+1,j)&& pieceType(get(i+1,j)) == Pawn && isSquareEmpty(i+1, j+1) && (currentGameState.doublePushFile - 1 == i+1))
						{
							moves.push_back(genMove(i,j,i+1,j+1, EPCapture));
						}
						if (numSquares[currentPID][SouthWestID] >= 1 && j == 4 && isSquareEnemy(i-1,j) && pieceType(get(i-1,j)) == Pawn && isSquareEmpty(i-1, j+1) && (currentGameState.doublePushFile - 1 == i-1))
						{
							moves.push_back(genMove(i,j,i-1,j+1, EPCapture));
						}

					}		

				}

				if (pieceType(piece) == King)
				{

					for (int dirID = 0 ; dirID <= 7 ; ++dirID)
					{
						int targetPos = currentPID + directions[dirID];

						if (numSquares[currentPID][dirID] >= 1 && isSquareFree(targetPos))
						{
							moves.push_back(genMove(currentPID, targetPos, isSquareEnemy(targetPos) * Capture));
						}
					}

					if (!controlled[j][i])
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
								if (!isSquareEmpty(p, j) || controlled[j][p])
								{
									isOK = false;
								}
							}
							if (isOK)
							{
								moves.push_back(genMove(currentPID, currentPID - 2, QueenCastle));
							}
						}
						if (   (currentGameState.canWhiteKingCastle && whiteToMove) ||  (currentGameState.canBlackKingCastle && !whiteToMove) )
						{
							bool isOK = true;
							for (int p = 5; p <= 6; ++p )
							{
								if (!isSquareEmpty(p, j) || controlled[j][p])
								{
									isOK = false;
								}
							}
							if (isOK)
							{
								moves.push_back(genMove(currentPID, currentPID + 2, KingCastle));
							}
						}
					}

				}

				if (pieceType(piece) == Knight)
				{

					if (numSquares[currentPID][NorthID] >= 2 && numSquares[currentPID][EastID] >= 1 && isSquareFree(i+1,j-2))
					{
						moves.push_back(genMove(i,j,i+1,j-2, isSquareEnemy(i+1,j-2) * Capture));
					}
					if (numSquares[currentPID][NorthID] >= 2 && numSquares[currentPID][WestID] >= 1 && isSquareFree(i-1,j-2))
					{
						moves.push_back(genMove(i,j,i-1,j-2, isSquareEnemy(i-1,j-2) * Capture));
					}
					if (numSquares[currentPID][SouthID] >= 2 && numSquares[currentPID][EastID] >= 1 && isSquareFree(i+1,j+2))
					{
						moves.push_back(genMove(i,j,i+1,j+2, isSquareEnemy(i+1,j+2) * Capture));
					}
					if (numSquares[currentPID][SouthID] >= 2 && numSquares[currentPID][WestID] >= 1 && isSquareFree(i-1,j+2))
					{
						moves.push_back(genMove(i,j,i-1,j+2, isSquareEnemy(i-1,j+2) * Capture));
					}

					if (numSquares[currentPID][EastID] >= 2 && numSquares[currentPID][NorthID] >= 1 && isSquareFree(i+2,j-1))
					{
						moves.push_back(genMove(i,j,i+2,j-1, isSquareEnemy(i+2,j-1) * Capture));
					}
					if (numSquares[currentPID][EastID] >= 2 && numSquares[currentPID][SouthID] >= 1 && isSquareFree(i+2,j+1))
					{
						moves.push_back(genMove(i,j,i+2,j+1, isSquareEnemy(i+2,j+1) * Capture));
					}
					if (numSquares[currentPID][WestID] >= 2 && numSquares[currentPID][NorthID] >= 1 && isSquareFree(i-2,j-1))
					{
						moves.push_back(genMove(i,j,i-2,j-1, isSquareEnemy(i-2,j-1) * Capture));
					}
					if (numSquares[currentPID][WestID] >= 2 && numSquares[currentPID][SouthID] >= 1 && isSquareFree(i-2,j+1))
					{
						moves.push_back(genMove(i,j,i-2,j+1, isSquareEnemy(i-2,j+1) * Capture));
					}

				}

				if (pieceType(piece) == Rook || pieceType(piece) == Bishop || pieceType(piece) == Queen)
				{

					int startDir = (pieceType(piece) == Bishop) ? 4 : 0;
					int endDir = (pieceType(piece) == Rook) ? 3 : 7;

					for (int dirID = startDir ; dirID <= endDir ; ++dirID)
					{
						for (int i = 0 ; i < numSquares[currentPID][dirID]; ++i)
						{
							int targetPos = currentPID + directions[dirID] * (i+1);

							if (isSquareFriendly(targetPos))
							{
								break;
							}


							moves.push_back(genMove(currentPID, targetPos, isSquareEnemy(targetPos) * Capture));

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
	int tag = (move & tagMask) >> 12;

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

	sf::Vector2i startPos = posIntTo2D(startPosi);
	sf::Vector2i endPos = posIntTo2D(endPosi);

	if (startPosi == endPosi)
	{
		return;
	}


	zobristKey ^= piecesZobrist[pieceType(get(startPosi))][whiteToMove][startPosi];
	if (tag == Capture || tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		zobristKey ^= piecesZobrist[pieceType(get(endPosi))][!whiteToMove][endPosi];
	}
	zobristKey ^= piecesZobrist[pieceType(get(startPosi))][whiteToMove][endPosi];



	if (tag == Capture || tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		newGameState.capturedPiece = get(endPosi);
	}

	if (tag == QuietMove || tag == Capture)
	{
		if ( (startPos.x == 0 && pieceType(get(startPosi)) == Rook) || (endPos.x == 0 && pieceType(get(endPosi)) == Rook)  )
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
		if (  (startPos.x == 7 && pieceType(get(startPosi)) == Rook) || (endPos.x == 7 && pieceType(get(endPosi)) == Rook)  )
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
		if (pieceType(get(startPosi)) == King)
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
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
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


		if (isPieceWhite(get(startPosi)))
		{
			board[endPos.y][endPos.x] = White | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][1][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][1][endPosi];
		}
		else
		{
			board[endPos.y][endPos.x] = Black | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][0][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][0][endPosi];
		}
		board[startPos.y][startPos.x] = None;
	}


	if (tag == DoublePawnPush)
	{
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
		newGameState.doublePushFile = startPos.x + 1;
	}

	if (tag == EPCapture)
	{
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
		board[startPos.y][endPos.x] = None;
		zobristKey ^= piecesZobrist[Pawn][!whiteToMove][endPos.x + 8 * startPos.y];
	}

	if (tag == KingCastle || tag == QueenCastle)
	{
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
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
			board[startPos.y][5] = board[startPos.y][7];
			board[startPos.y][7] = None;
			zobristKey ^= piecesZobrist[Rook][whiteToMove][7 + startPos.y * 8];
			zobristKey ^= piecesZobrist[Rook][whiteToMove][5 + startPos.y * 8];
		}
		else
		{
			board[startPos.y][3] = board[startPos.y][0];
			board[startPos.y][0] = None;
			zobristKey ^= piecesZobrist[Rook][whiteToMove][startPos.y * 8];
			zobristKey ^= piecesZobrist[Rook][whiteToMove][3 + startPos.y * 8];
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


		if (isPieceWhite(get(startPosi)))
		{
			board[endPos.y][endPos.x] = White | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][1][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][1][endPosi];
		}
		else
		{
			board[endPos.y][endPos.x] = Black | piecePromoType;
			zobristKey ^= piecesZobrist[Pawn][0][endPosi];
			zobristKey ^= piecesZobrist[piecePromoType][0][endPosi];
		}
		board[startPos.y][startPos.x] = None;
	}

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
	int tag = (move & tagMask) >> 12;


	sf::Vector2i startPos = posIntTo2D(startPosi);
	sf::Vector2i endPos = posIntTo2D(endPosi);

	if (startPosi == endPosi)
	{
		return;
	}

	if (tag == QuietMove || tag == DoublePawnPush)
	{
		board[startPos.y][startPos.x] = board[endPos.y][endPos.x];
		board[endPos.y][endPos.x] = None;
	}


	if (tag == Capture)
	{
		board[startPos.y][startPos.x] = board[endPos.y][endPos.x];
		board[endPos.y][endPos.x] = currentGameState.capturedPiece;
	}


	if (tag == EPCapture)
	{
		board[startPos.y][startPos.x] = board[endPos.y][endPos.x];
		board[endPos.y][endPos.x] = None;
		if (!whiteToMove)
		{
			board[endPos.y + 1][endPos.x] = Black | Pawn;
		}
		else
		{
			board[endPos.y - 1][endPos.x] = White | Pawn;
		}
	}

	if (tag == KingCastle || tag == QueenCastle)
	{
		board[startPos.y][startPos.x] = board[endPos.y][endPos.x];
		board[endPos.y][endPos.x] = None;
		if (tag == KingCastle)
		{
			board[startPos.y][7] = board[startPos.y][5];
			board[startPos.y][5] = None;
		}
		else
		{
			board[startPos.y][0] = board[startPos.y][3];
			board[startPos.y][3] = None;
		}
	}

	if (tag == KnightProm || tag == RookProm || tag == BishopProm || tag == QueenProm)
	{
		if (!whiteToMove)
		{
			board[startPos.y][startPos.x] = White | Pawn;
		}
		else
		{
			board[startPos.y][startPos.x] = Black | Pawn;
		}
		board[endPos.y][endPos.x] = None;
	}

	if (tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		if (!whiteToMove)
		{
			board[startPos.y][startPos.x] = White | Pawn;
		}
		else
		{
			board[startPos.y][startPos.x] = Black | Pawn;
		}
		board[endPos.y][endPos.x] = currentGameState.capturedPiece;
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

