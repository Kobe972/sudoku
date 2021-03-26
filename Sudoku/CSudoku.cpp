#include "CSudoku.h"
extern CButton button[20];

void CSudoku::GameInit()
{
	m_Width = m_Height = 60;
	memset(m_small, 0, sizeof(m_small));
	m_mode = 0;
	memset(g_mode, 0, sizeof(g_mode));
	BITMAP_FILE grid[3];
	char tmp[3][20];
	strcpy(tmp[0], ".\\Grid\\pink.bmp");
	strcpy(tmp[1], ".\\Grid\\green.bmp");
	strcpy(tmp[2], ".\\Grid\\gray.bmp");
	for (int i = 0; i < 3; i++)
		m_grid[i] = DDraw_Create_Surface(m_Width, m_Height, 0, 0);
	for (int i = 0; i < 3; i++)
	{
		grid[i].Load_File(tmp[i]);
		DDraw_Draw_Bitmap(&grid[i], m_grid[i], { 0,0 });
	}
}

void CSudoku::Create()
{
	memset(m_Sudoku, 0, sizeof(m_Sudoku));
	start_time = clock();
	duration = 0;
	focus = { 0,0 };
	return;
}

void CSudoku::CheckFocus()
{
	POINT coor = Get_Grid_By_Coor(pos.x, pos.y);
	if ((mouse_state.rgbButtons[MOUSE_LEFT_BUTTON] & 0x80)&&coor.x >= 0 && coor.x < 9 && coor.y >= 0 && coor.y < 9) focus = { coor.x,coor.y };
}

void CSudoku::Draw()
{
	BITMAP_FILE_PTR bitmap = new BITMAP_FILE;//绘制背景
	bitmap->Load_File(".\\background\\sky.bmp");
	DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
	bitmap->Unload_File();
	button[IRETURN].Draw();
	//绘制网格
	RECT coor = { 0,0,m_Width,m_Height }, boarder;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)//i行j列
		{
			boarder.left = (800 - 9 * m_Width) / 2 + m_Width*j;
			boarder.top = (600 - 9 * m_Height) / 2 + m_Height * i;
			boarder.right = boarder.left + m_Width;
			boarder.bottom = boarder.top + m_Height;
			if (focus.x == j && focus.y == i)
			{
				lpddsback->Blt(&boarder, m_grid[2], &coor, DDBLT_WAIT, NULL);
				continue;
			}
			if (i / 3 == j / 3 || i / 3 + j / 3 == 2)
			{
				lpddsback->Blt(&boarder, m_grid[0], &coor, DDBLT_WAIT, NULL);
			}
			else
			{
				lpddsback->Blt(&boarder, m_grid[1], &coor, DDBLT_WAIT, NULL);
			}
		}
	}
}

POINT CSudoku::Get_Grid_By_Coor(int x, int y)
{
	int startx = x - (800 - 9*m_Width) / 2;
	int starty = y - (600 - 9*m_Height) / 2;
	POINT ret;
	ret.x = startx / m_Width;
	ret.y = starty / m_Height;
	return ret;
}
