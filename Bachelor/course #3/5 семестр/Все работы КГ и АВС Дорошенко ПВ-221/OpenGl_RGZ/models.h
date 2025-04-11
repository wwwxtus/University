#ifndef MODELS_H
#define MODELS_H

#include <windows.h>
#include "glut.h"
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct Checker {
    bool isBlack;
    bool isKing;
    float x, y;
};

const float checkerHeight = 0.2f;
const float checkerRadius = 0.4f;

const int boardSize = 8;
const float cellSize = 1.0f;

void drawChair(float x, float y, float z);
void drawChecker(float x, float y, bool isBlack);
void drawBoard();
void drawCheckers();
void initGame();
void drawLeg(float x, float y, float z, float legWidth, float legDepth, float legHeight);

#endif // MODELS_H

