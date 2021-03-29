#pragma once
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define UNASSIGNED 0

using namespace std;

class Generator {
public:
    int grid[9][9];
    int solnGrid[9][9];
    int guessNum[9];
    int gridPos[81];
    int difficultyLevel;

public:
    Generator();
    void createSeed();
    bool solveGrid();
    string getGrid();
    void countSoln(int& number);
    void genPuzzle();
    void CreateSudoku();
};