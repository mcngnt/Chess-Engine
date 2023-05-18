#include "transposition.h"

TranspositionTable::TranspositionTable()
{

}

Transposition TranspositionTable::get(uint64_t key, char depth, int alpha, int beta)
{
	int index = key % tableSize;
	Transposition t = table[index];
	if (t.key == key)
	{
		if (t.depth >= depth)
		{
			if (t.nodeType == ExactNode)
			{
				return t;
			}
			if (t.nodeType == AlphaNode && t.value <= alpha)
			{
				return t;
			}
			if (t.nodeType == BetaNode && t.value >= beta)
			{
				return t;
			}
		}
	}
	t.isValid = false;
	return t;
}


void TranspositionTable::set(uint64_t key, char depth, int value, char nodeType, int bestMove)
{
	Transposition t;
	t.isValid = true;
	t.key = key;
	t.depth = depth;
	t.value = value;
	t.nodeType = nodeType;
	t.bestMove = bestMove;

	table[key % tableSize] = t;

}