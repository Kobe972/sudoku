#include"pch.h"
#include "GUI.h"
#include"BobClass.h"
#pragma once
class CSudoku
{
public:
	POINT focus;//����
	int m_Sudoku[9][9];//����
	int m_small[9][9][10];//�Ǳ�
	void GameInit();
	void Create();
	void CheckFocus();
	void CheckKey();
	void Draw();
	POINT Get_Grid_By_Coor(int x, int y);
	int Win();//�Ƿ��ʤ
	time_t duration;//��Ϸʱ��
	time_t start_time;
	int m_mode = 0;//��������ģʽ��0��ʾ����ģʽ��1��ʾ�Ǳ�ģʽ
	int g_mode[9][9];//������Ԫ�������ģʽ
	LPDIRECTDRAWSURFACE7 m_grid[3];
	int m_Width, m_Height;
};

