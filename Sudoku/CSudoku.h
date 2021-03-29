#include"pch.h"
#include "GUI.h"
#include"BobClass.h"
#include"generator.h"
#pragma once
class CSudoku
{
public:
	POINT focus;//焦点
	int m_Sudoku[9][9];//数独
	int m_small[9][9][10];//角标
	int m_const[9][9];//一开始就生成的数，不能改变
	void GameInit();
	void Create();
	void CheckFocus();
	void CheckKey();
	void Draw();
	POINT Get_Grid_By_Coor(int x, int y);
	int Win();//是否获胜
	int m_win;
	time_t duration;//游戏时长
	time_t start_time;
	int m_difficulty;
	int m_mode = 0;//整体输入模式，0表示正常模式，1表示角标模式
	int g_mode[9][9];//各个单元格的输入模式
	LPDIRECTDRAWSURFACE7 m_grid[3];
	int m_Width, m_Height;
};

