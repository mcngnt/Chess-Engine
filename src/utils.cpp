#include "utils.h"

int min(int i, int j)
{
	if (i < j)
	{
		return i;
	}
	else
	{
		return j;
	}
}


int abs(int x)
{
	if ( x > 0)
	{
		return x;
	}
	else
	{
		return -x;
	}
}