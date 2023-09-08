#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stack>
#include <iostream>
#include <string>
#include <ctype.h>
#include <algorithm>
#include <chrono>
#include <random>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <bitboardHelper.h>

#define USE_UCI false

#if !USE_UCI
#include <SFML/Graphics.hpp>
#endif


#define min(a,b) (a) > (b) ? (b) : (a)


#endif
