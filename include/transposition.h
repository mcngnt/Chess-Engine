#include <utils.h>

#define tableSize 35000

enum NodeType
{
	ExactNode = 0,
	AlphaNode = 1,
	BetaNode = 2
};

struct Transposition
{
	uint64_t key;
	int depth;
	int value;
	int bestMove;
	int nodeType;
	bool isValid = false;
};

class TranspositionTable
{
public:
	TranspositionTable();

	Transposition get(uint64_t key, int depth, int alpha, int beta);

	void set(uint64_t key, int depth, int value, int nodeType, int bestMove);

	void clear();

	int count;
	Transposition table[tableSize];

};