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
	char depth;
	int value;
	int bestMove;
	char nodeType;
	bool isValid = false;
};

class TranspositionTable
{
public:
	TranspositionTable();

	Transposition get(uint64_t key, char depth, int alpha, int beta);

	void set(uint64_t key, char depth, int value, char nodeType, int bestMove);

	void clear();

	int count;
	

	Transposition table[tableSize];

};