#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stack>
#include <math.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <ctype.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <cstdint>

#define SCREEN_W 1300
#define SCREEN_H 1000

#define infinity 999999



#define PI 3.14

float rand_float(bool);
float rad_to_deg(float);
float mod_2pi(float);
float angle_dist(float, float);
float clamp(float, float, float);
float sign(float);
float neg_range(float);
int min(int x, int y);
int abs(int x);

#endif
