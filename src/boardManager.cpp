#include "boardManager.h"



int directions[8] = {North, South, East, West, NorthEast, NorthWest, SouthEast, SouthWest};


int numSquares[64][8];

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

bool isPieceWhite(int piece)
{
	return (piece >> 3) == 1;
}
int pieceType(int piece)
{
	return piece & 7;
}

int pid(int i, int j)
{
	return i + 8 * j;
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

			// printf("%d %d\n", i + j*8, min(numSouth, numEast));
		}
	}

	// currentGameState = canCastleMask;
	currentGameState.capturedPiece = 0;
	currentGameState.canWhiteKingCastle = true;
	currentGameState.canWhiteQueenCastle = true;
	currentGameState.canBlackKingCastle = true;
	currentGameState.canBlackQueenCastle = true;
	currentGameState.doublePushRank = 0;
	currentGameState.moveCount = 0;

	loadFen(startingFen);
	// currentBitboard = boardToBitBoard();
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
							// controlled[j-1][i+1] = true;
							assign(j-1,i+1);
						}
						if (numSquares[currentPID][NorthWestID] >= 1 && isSquareFree(i-1,j-1))
						{
							// controlled[j-1][i+1] = true;
							assign(j-1,i-1);
						}

					}
					else
					{
						if (numSquares[currentPID][SouthEastID] >= 1 && isSquareFree(i+1,j+1))
						{
							// controlled[j+1][i+1] = true;
							assign(j+1, i+1);
						}
						if (numSquares[currentPID][SouthWestID] >= 1 && isSquareFree(i-1,j+1))
						{
							// controlled[j+1][i-1] = true;
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
							// controlled[targetPos / 8][targetPos % 8] = true;
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

							// controlled[targetPos / 8][targetPos % 8] = true;
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

std::vector<int> BoardManager::generateMoves()
{
	std::vector<int> pseudoMoves = generatePseudoMoves();

	std::vector<int> legalMoves;

	// int kingPos;


	for (int pseudoMove : pseudoMoves)
	{
		// if (startPos(pseudoMove) == endPos(pseudoMove))
		// {
		// 	printf("WTF %s\n", standardNotation(pseudoMove).c_str());
		// 	continue;
		// }

		makeMove(pseudoMove);
		// kingPos = !whiteToMove ? currentGameState.whiteKingPos : currentGameState.blackKingPos;
		// bool check = false;
		// for (int opponentMove : generatePseudoMoves())
		// {
		// 	if (endPos(opponentMove) == kingPos)
		// 	{
		// 		check = true;
		// 	}
		// }

		whiteToMove = !whiteToMove;

		if (!isChecked())
		{
			legalMoves.push_back(pseudoMove);
		}

		whiteToMove = !whiteToMove;

		unmakeMove(pseudoMove);
		// if (get(5) == (Black | Bishop))
		// {
		// 	printf("%s\n", standardNotation(pseudoMove).c_str());
		// }
		// if (!check)
		// {
		// 	legalMoves.push_back(pseudoMove);
		// }
	}

	// printf("%s\n", "Ouf !");


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

						// En passant
						// (currentGameState >> 10) & 7) - 1 == i+1)

						if (numSquares[currentPID][NorthEastID] >= 1 && j == 3 && isSquareEnemy(i+1,j) && pieceType(get(i+1,j)) == Pawn && isSquareEmpty(i+1, j-1) && currentGameState.doublePushRank - 1 == i+1)
						{
							moves.push_back(genMove(i,j,i+1,j-1, EPCapture));
						}
						if (numSquares[currentPID][NorthWestID] >= 1 && j == 3 && isSquareEnemy(i-1,j) && pieceType(get(i-1,j)) == Pawn && isSquareEmpty(i-1, j-1) && currentGameState.doublePushRank - 1 == i-1)
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

						if (numSquares[currentPID][SouthEastID] >= 1 && j == 4 && isSquareEnemy(i+1,j)&& pieceType(get(i+1,j)) == Pawn && isSquareEmpty(i+1, j+1) && (currentGameState.doublePushRank - 1 == i+1))
						{
							moves.push_back(genMove(i,j,i+1,j+1, EPCapture));
						}
						if (numSquares[currentPID][SouthWestID] >= 1 && j == 4 && isSquareEnemy(i-1,j) && pieceType(get(i-1,j)) == Pawn && isSquareEmpty(i-1, j+1) && (currentGameState.doublePushRank - 1 == i-1))
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
							for (int p = 2; p < i; ++p )
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
							for (int p = i+1; p < 7; ++p )
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

	// printf("Bruh8 %d \n", currentGameState.blackKingPos);

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
					i += 1; 
					break;
				case 'n':
					board[j][i] = Black | Knight;
					i += 1; 
					break;
				case 'b':
					board[j][i] = Black | Bishop;
					i += 1;
					break; 
				case 'q':
					board[j][i] = Black | Queen;
					i += 1; 
					break;
				case 'k':
					board[j][i] = Black | King;
					currentGameState.blackKingPos = i + j*8;
					i += 1;
					break; 
				case 'p':
					board[j][i] = Black | Pawn;
					i += 1;
					break; 
				case 'R':
					board[j][i] = White | Rook;
					i += 1;
					break; 
				case 'N':
					board[j][i] = White | Knight;
					i += 1;
					break; 
				case 'B':
					board[j][i] = White | Bishop;
					i += 1; 
					break;
				case 'Q':
					board[j][i] = White | Queen;
					i += 1; 
					break;
				case 'K':
					board[j][i] = White | King;
					currentGameState.whiteKingPos = i + j*8;
					i += 1; 
					break;
				case 'P':
					board[j][i] = White | Pawn;
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
					currentGameState.doublePushRank = (c - '0');
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

	fen += '0' + currentGameState.doublePushRank;

	fen += ' ';

	fen += '1';

	return fen;

}



// BitBoard BoardManager::boardToBitBoard()
// {
// 	BitBoard bitboard;
// 		// printf("White pawn : %llu\n", bitboard.whitePawn);
// 	for (int i = 0 ; i < 64 ; ++i)
// 	{
// 		int piece = get(i);
// 		uint64_t bitPos = 1;
// 		bitPos <<= i;
// 		switch (piece)
// 		{
// 				case White | Pawn:
// 						// printf("%llu\n", bitboard.whitePawn);
// 						bitboard.whitePawn |= bitPos;
// 						break;
// 				case White | Rook:
// 						bitboard.whiteRook |= bitPos;
// 						break;
// 				case White | Knight:
// 						bitboard.whiteKnight |= bitPos;
// 						break;
// 				case White | Bishop:
// 						bitboard.whiteBishop |= bitPos;
// 						break;
// 				case White | Queen:
// 						bitboard.whiteQueen |= bitPos;
// 						break;
// 				case White | King:
// 						bitboard.whiteKing |= bitPos;
// 						break;
// 				case Black | Pawn:
// 						bitboard.blackPawn |= bitPos;
// 						break;
// 				case Black | Rook:
// 						bitboard.blackRook |= bitPos;
// 						break;
// 				case Black | Knight:
// 						bitboard.blackKnight |= bitPos;
// 						break;
// 				case Black | Bishop:
// 						bitboard.blackBishop |= bitPos;
// 						break;
// 				case Black | Queen:
// 						bitboard.blackQueen |= bitPos;
// 						break;
// 				case Black | King:
// 						bitboard.blackKing |= bitPos;
// 						break;
// 		}
// 	}
// 	return bitboard;
// }





// void BoardManager::loadBitBoard(BitBoard bitboard)
// {
// 	// printf("%llu\n",bitboard.blackQueen);
// 	for (int i = 0 ; i < 64; ++i)
// 	{
// 		int piece =     ((bitboard.whitePawn >> i) & 1) * (White | Pawn) +
// 						((bitboard.whiteRook >> i) & 1) * (White | Rook) +
// 						((bitboard.whiteKnight >> i) & 1) * (White | Knight) +
// 						((bitboard.whiteBishop >> i) & 1) * (White | Bishop) +
// 						((bitboard.whiteQueen >> i) & 1) * (White | Queen) +
// 						((bitboard.whiteKing >> i) & 1) * (White | King) +
// 						((bitboard.blackPawn >> i) & 1) * (Black | Pawn) +
// 						((bitboard.blackRook >> i) & 1) * (Black | Rook) +
// 						((bitboard.blackKnight >> i) & 1) * (Black | Knight) +
// 						((bitboard.blackBishop >> i) & 1) * (Black | Bishop) +
// 						((bitboard.blackQueen >> i) & 1) * (Black | Queen) +
// 						((bitboard.blackKing >> i) & 1) * (Black | King);

// 		if (piece > 22)
// 		{
// 			printf("%s %d\n", "load", i);
// 			// printf("%llu %llu %llu %llu %llu %llu\n", ((bitboard.blackPawn >> i) & 1) * (Black | Pawn),
// 			// 			((bitboard.blackRook >> i) & 1) * (Black | Rook) ,
// 			// 			((bitboard.blackKnight >> i) & 1) * (Black | Knight) ,
// 			// 			((bitboard.blackBishop >> i) & 1) * (Black | Bishop) ,
// 			// 			((bitboard.blackQueen >> i) & 1) * (Black | Queen) ,
// 			// 			((bitboard.blackKing >> i) & 1) * (Black | King) );
// 		}
// 		board[i / 8][i % 8] = piece;
// 		if (piece == (White | King))
// 		{
// 			whiteKingPos = i;
// 		}
// 		if (piece == (Black | King))
// 		{
// 			blackKingPos = i;
// 		}
// 	}
// }




void BoardManager::makeMove(int move)
{

	// BoardStruct nboard;
	// // nboard.board = {0};
	// for (int i = 0; i < 64; ++i)
	// {
	// 	printf("%d\n",i );
	// 	nboard.board[i/8][i%8] = board[i/8][i%8];
	// }

	// nboard.whiteKingPos = whiteKingPos;
	// nboard.blackKingPos = blackKingPos;
	// boardHistory.push(nboard);
	// // printf("%s\n", "aga");

	// printf(" --> %d <-- \n", move);



	int startPosi = startPos(move);
	int endPosi = endPos(move);
	int tag = (move & tagMask) >> 12;

	GameState newGameState;
	newGameState.canBlackQueenCastle = currentGameState.canBlackQueenCastle;
	newGameState.canWhiteQueenCastle = currentGameState.canWhiteQueenCastle;
	newGameState.canWhiteKingCastle = currentGameState.canWhiteKingCastle;
	newGameState.canBlackKingCastle = currentGameState.canBlackKingCastle;
	newGameState.doublePushRank = 0;
	newGameState.whiteKingPos = currentGameState.whiteKingPos;
	newGameState.blackKingPos = currentGameState.blackKingPos;

	sf::Vector2i startPos = posIntTo2D(startPosi);
	sf::Vector2i endPos = posIntTo2D(endPosi);

	if (startPosi == endPosi)
	{
		return;
	}

	if (tag == Capture || tag == KnightPromCapture || tag == RookPromCapture || tag == BishopPromCapture || tag == QueenPromCapture)
	{
		newGameState.capturedPiece = get(endPosi);
	}

	if (tag == QuietMove || tag == Capture)
	{
		if ((pieceType(get(startPosi)) == Rook && startPos.x == 0) || (pieceType(get(endPosi)) == Rook && startPos.x == 0))
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
		if ((pieceType(get(startPosi)) == Rook && startPos.x == 7) || (pieceType(get(endPosi)) == Rook && startPos.x == 7))
		{
			if (whiteToMove)
			{
				newGameState.canWhiteKingCastle = false;
			}
			else
			{
				newGameState.canWhiteKingCastle = false;
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
		}
		else
		{
			board[endPos.y][endPos.x] = Black | piecePromoType;
		}
		board[startPos.y][startPos.x] = None;
	}


	if (tag == DoublePawnPush)
	{
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
		newGameState.doublePushRank = startPos.x + 1;
	}

	if (tag == EPCapture)
	{
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
		board[startPos.y][endPos.x] = None;
	}

	if (tag == KingCastle || tag == QueenCastle)
	{
		board[endPos.y][endPos.x] = board[startPos.y][startPos.x];
		board[startPos.y][startPos.x] = None;
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
		if (tag == KingCastle)
		{
			board[startPos.y][startPos.x + 1] = board[startPos.y][7];
			board[startPos.y][7] = None;
		}
		else
		{
			board[startPos.y][startPos.x - 1] = board[startPos.y][0];
			board[startPos.y][0] = None;
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
		}
		else
		{
			board[endPos.y][endPos.x] = Black | piecePromoType;
		}
		board[startPos.y][startPos.x] = None;
	}


	// for (int i = 0; i < 64; ++i)
	// {
	// 	printf("%d ", get(i));
	// 	if (i % 8 == 0)
	// 	{
	// 		printf("\n");
	// 	}
	// }

	// currentBitboard = boardToBitBoard();


	whiteToMove = !whiteToMove;

	// for (int i = 0; i < 64; ++i)
	// {
	// 	int piece = get(i);
	// 	if (pieceType(piece) == King)
	// 	{
	// 		if (isPieceWhite(piece))
	// 		{
	// 			newGameState.whiteKingPos = i;
	// 		}
	// 		else
	// 		{
	// 			newGameState.blackKingPos = i;
	// 		}
	// 	}
	// }

	gameStateHistory.push(currentGameState);

	// printf("Bruh %d %d %s\n", currentGameState.blackKingPos, newGameState.blackKingPos,standardNotation(move).c_str());

	// currentGameState.canBlackQueenCastle = newGameState.canBlackQueenCastle;
	// currentGameState.canWhiteQueenCastle = newGameState.canWhiteQueenCastle;
	// currentGameState.canWhiteKingCastle = newGameState.canWhiteKingCastle;
	// currentGameState.canBlackKingCastle = newGameState.canBlackKingCastle;
	// currentGameState.doublePushRank = newGameState.doublePushRank;
	// currentGameState.whiteKingPos = newGameState.whiteKingPos;
	// currentGameState.blackKingPos = newGameState.blackKingPos;

	currentGameState = newGameState;
	
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
			board[startPos.y][7] = board[startPos.y][startPos.x+1];
			board[startPos.y][startPos.x+1] = None;
		}
		else
		{
			board[startPos.y][0] = board[startPos.y][startPos.x-1];
			board[startPos.y][startPos.x-1] = None;
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
	gameStateHistory.pop();


	// BoardStruct nboard = boardHistory.top();

	// for (int i = 0; i < 64; ++i)
	// {
	// 	board[i/8][i%8] = nboard.board[i/8][i%8];
	// }
	// whiteKingPos = nboard.whiteKingPos;
	// blackKingPos = nboard.blackKingPos;
	// boardHistory.pop();

	// printf("%s\n", "aaaaaa");

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


