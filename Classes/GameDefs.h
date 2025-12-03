#pragma once
#include "cocos2d.h"

// Grid cell size
const cocos2d::Size CELLSIZE = cocos2d::Size(130, 120);

// Grid origin position (bottom-left corner)
const cocos2d::Vec2 GRID_ORIGIN = cocos2d::Vec2(10, 10);

// Maximum number of rows and columns in the grid
const int MAX_ROW = 5;
const int MAX_COL = 9;

// Layer constants for z-ordering
const int BACKGROUND_LAYER = 0;
const int PLANT_LAYER = 1;
const int BULLET_LAYER = 2;
const int ENEMY_LAYER = 3;
const int SUNLAYER = 4;
