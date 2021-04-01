#include "CSudoku.h"
extern CButton button[20];
extern CCheckBox checkbox[20];

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
	memset(m_const, 0, sizeof(m_const));
	BITMAP_FILE_PTR bitmap = new BITMAP_FILE;//绘制背景
	bitmap->Load_File(".\\background\\sky.bmp");
	DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
	bitmap->Unload_File();
	button[IRETURN].Draw();
	char out[50];
	CFont CurText;
	//绘制网格
	RECT coor = { 0,0,m_Width,m_Height }, boarder;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)//i行j列
		{
			boarder.left = (800 - 9 * m_Width) / 2 + m_Width * j;
			boarder.top = (600 - 9 * m_Height) / 2 + m_Height * i;
			boarder.right = boarder.left + m_Width;
			boarder.bottom = boarder.top + m_Height;
			//绘制表格
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
	checkbox[J_SILENCE].Draw();
	DDraw_Flip();
	Generator puzzle;
	puzzle.CreateSudoku();
	puzzle.calculateDifficulty();
	m_difficulty = puzzle.difficultyLevel;
	memcpy(m_Sudoku, puzzle.grid, sizeof(puzzle.grid));
	memcpy(m_ans, puzzle.solnGrid, sizeof(puzzle.solnGrid));
	for (int i = 0; i < 81; i++)
	{
		if (*(*m_Sudoku + i) != 0) *(*m_const + i) = 1;
	}
	start_time = clock();
	duration = 0;
	m_win = 0;
	for (int i = 0; i < 81; i++)
	{
		if (m_const[i / 9][i % 9] == 0)
		{
			focus = { i % 9,i / 9 };
			break;
		}
	}
	return;
}

void CSudoku::CheckFocus()
{
	POINT coor = Get_Grid_By_Coor(pos.x, pos.y);
	if ((mouse_state.rgbButtons[MOUSE_LEFT_BUTTON] & 0x80)&&coor.x >= 0 && coor.x < 9 && coor.y >= 0 && coor.y < 9&&m_const[coor.y][coor.x]==0) focus = { coor.x,coor.y };
}

void CSudoku::CheckKey()
{
	static time_t last_input_time=clock();
	static char last_input='n';
	int check_win = 0;
	if (keyboard_state[DIK_BACKSPACE] & 0x80 && (clock() - last_input_time >= 300 || last_input != 'b'))
	{
		last_input_time = clock();
		last_input = 'b';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0) m_Sudoku[focus.y][focus.x] = 0;
		else
		{
			for (int i = 9; i >= 1; i--)
			{
				if (m_small[focus.y][focus.x][i] == 1)
				{
					m_small[focus.y][focus.x][i] = 0;
					break;
				}
			}
		}
	}
	if (keyboard_state[DIK_1] & 0x80 || keyboard_state[DIK_NUMPAD1] & 0x80)
	{
		last_input = '1';
		if ((g_mode[focus.y][focus.x] + m_mode)%2==2)
		{
			for (int i = 1; i < 9; i++) m_small[focus.y][focus.x][i] = 0;
			m_Sudoku[focus.y][focus.x] = 0;
		}
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 1;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][1] = 1;
	}
	if (keyboard_state[DIK_2] & 0x80 || keyboard_state[DIK_NUMPAD2] & 0x80)
	{
		last_input = '2';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 2;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][2] = 1;
	}
	if (keyboard_state[DIK_3] & 0x80 || keyboard_state[DIK_NUMPAD3] & 0x80)
	{
		last_input = '3';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 3;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][3] = 1;
	}
	if (keyboard_state[DIK_4] & 0x80 || keyboard_state[DIK_NUMPAD4] & 0x80)
	{
		last_input = '4';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 4;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][4] = 1;
	}
	if (keyboard_state[DIK_5] & 0x80 || keyboard_state[DIK_NUMPAD5] & 0x80)
	{
		last_input = '5';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 5;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][5] = 1;
	}
	if (keyboard_state[DIK_6] & 0x80 || keyboard_state[DIK_NUMPAD6] & 0x80)
	{
		last_input = '6';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 6;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][6] = 1;
	}
	if (keyboard_state[DIK_7] & 0x80 || keyboard_state[DIK_NUMPAD7] & 0x80)
	{
		last_input = '7';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 7;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][7] = 1;
	}
	if (keyboard_state[DIK_8] & 0x80 || keyboard_state[DIK_NUMPAD8] & 0x80)
	{
		last_input = '8';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 8;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][8] = 1;
	}
	if (keyboard_state[DIK_9] & 0x80 || keyboard_state[DIK_NUMPAD9] & 0x80)
	{
		last_input = '9';
		g_mode[focus.y][focus.x] = m_mode;
		if (m_mode == 0)
		{
			m_Sudoku[focus.y][focus.x] = 9;
			check_win = 1;
		}
		else
			m_small[focus.y][focus.x][9] = 1;
	}
	if (keyboard_state[DIK_S] & 0x80 && (clock() - last_input_time >= 300 || last_input != 's'))
	{
		last_input_time = clock();
		last_input = 's';
		m_mode = (m_mode == 0)?1:0;
	}
	if (keyboard_state[DIK_LEFT] & 0x80 && (clock() - last_input_time >= 300 || last_input != 'l'))
	{
		last_input_time = clock();
		last_input = 'l';
		for (int i = focus.x-1; i >= 0; i--)
		{
			if (m_const[focus.y][i] != 1)
			{
				focus.x = i;
				break;
			}
		}
	}
	if (keyboard_state[DIK_RIGHT] & 0x80 && (clock() - last_input_time >= 300 || last_input != 'r'))
	{
		last_input_time = clock();
		last_input = 'r';
		for (int i = focus.x+1; i < 9; i++)
		{
			if (m_const[focus.y][i] != 1)
			{
				focus.x = i;
				break;
			}
		}
	}
	if (keyboard_state[DIK_UP] & 0x80 && (clock() - last_input_time >= 300 || last_input != 'u'))
	{
		last_input_time = clock();
		last_input = 'l';
		for (int i = focus.y-1; i >= 0; i--)
		{
			if (m_const[i][focus.x] != 1)
			{
				focus.y = i;
				break;
			}
		}
	}
	if (keyboard_state[DIK_DOWN] & 0x80 && (clock() - last_input_time >= 300 || last_input != 'd'))
	{
		last_input_time = clock();
		last_input = 'd';
		for (int i = focus.y+1; i < 9; i++)
		{
			if (m_const[i][focus.x] != 1)
			{
				focus.y = i;
				break;
			}
		}
	}
	if (keyboard_state[DIK_SPACE] & 0x80 && (clock() - last_input_time >= 300 || last_input != 'p'))
	{
		last_input_time = clock();
		last_input = 'p';
		for (int i = focus.x + 1; i < 9; i++)
		{
			if (m_const[focus.y][i] != 1)
			{
				focus.x = i;
				break;
			}
		}
	}
	if (check_win) m_win = Win();
}

void CSudoku::Draw()
{
	BITMAP_FILE_PTR bitmap = new BITMAP_FILE;//绘制背景
	bitmap->Load_File(".\\background\\sky.bmp");
	DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
	bitmap->Unload_File();
	button[IRETURN].Draw();
	button[IANSWER].Draw();
	char out[50];
	CFont CurText;
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
			//绘制表格
			if (focus.x == j && focus.y == i)
			{
				lpddsback->Blt(&boarder, m_grid[2], &coor, DDBLT_WAIT, NULL);
			}
			else if (i / 3 == j / 3 || i / 3 + j / 3 == 2)
			{
				lpddsback->Blt(&boarder, m_grid[0], &coor, DDBLT_WAIT, NULL);
			}
			else
			{
				lpddsback->Blt(&boarder, m_grid[1], &coor, DDBLT_WAIT, NULL);
			}
			//绘制数字
			if (g_mode[i][j] == 0)
			{
				lpddsback->GetDC(&CurText.hdc);
				CurText.SetType(m_Height/6*5, m_Width/12*5, 15);
				CurText.Uself();
				if(m_const[i][j]==1) SetTextColor(CurText.hdc, RGB(0, 0, 0));
				else SetTextColor(CurText.hdc, RGB(255, 0, 0));
				SetBkMode(CurText.hdc, TRANSPARENT);
				sprintf(out, "%d", m_Sudoku[i][j]);
				if(m_Sudoku[i][j]!=0) TextOut(CurText.hdc, boarder.left+m_Width/24*7, boarder.top+m_Height/12, out, strlen(out));
				lpddsback->ReleaseDC(CurText.hdc);
			}
			else
			{
				lpddsback->GetDC(&CurText.hdc);
				CurText.SetType(m_Height/3, m_Width/7, 10);
				CurText.Uself();
				SetTextColor(CurText.hdc, RGB(255, 0, 0));
				SetBkMode(CurText.hdc, TRANSPARENT);
				for (int k = 1; k < 10; k++)
				{
					if (m_small[i][j][k] == 1)
					{
						sprintf(out, "%d", k);
						TextOut(CurText.hdc, boarder.left+20*((k-1)%3)+5, boarder.top+20*((k-1)/3), out, strlen(out));
					}
				}
				lpddsback->ReleaseDC(CurText.hdc);
			}
		}
	}
	checkbox[J_SILENCE].Draw();
	duration = (clock() - start_time) / 1000;
	sprintf(out, "Time consuming:%02d:%02d:%02d", ((int)duration) / 3600, ((int)duration) % 3600 / 60, ((int)duration) % 60);
	lpddsback->GetDC(&CurText.hdc);
	CurText.SetType(25, 9, 3);
	CurText.Uself();
	SetTextColor(CurText.hdc, RGB(0, 0, 0));
	SetBkMode(CurText.hdc, TRANSPARENT);
	TextOut(CurText.hdc, 490, 2, out, strlen(out));
	sprintf(out, "Difficulty:%d/1000", m_difficulty);
	TextOut(CurText.hdc, (800 - 9 * m_Width) / 2, 2, out, strlen(out));
	lpddsback->ReleaseDC(CurText.hdc);
}

POINT CSudoku::Get_Grid_By_Coor(int x, int y)
{
	int startx = x - (800 - 9*m_Width) / 2;
	int starty = y - (600 - 9*m_Height) / 2;
	POINT ret;
	ret.x = startx<0?-1:startx / m_Width;
	ret.y = starty<0?-1:starty / m_Height;
	return ret;
}

int CSudoku::Win()
{
	int test_row[10],test_col[10],test_sub_grid[10],flag;
	for (int i = 0; i < 9; i++)
	{
		memset(test_row, 0, sizeof(test_row));
		memset(test_col, 0, sizeof(test_col));
		memset(test_sub_grid, 0, sizeof(test_sub_grid));
		for (int j = 0; j < 9; j++)
		{
			test_row[m_Sudoku[i][j]] = 1;
			test_col[m_Sudoku[j][i]] = 1;
			test_sub_grid[m_Sudoku[i / 3 * 3 + j / 3][i % 3 * 3 + j % 3]] = 1;
		}
		for (int j = 1; j < 10; j++)
		{
			if (test_row[j] + test_col[j] + test_sub_grid[j] != 3) return 0;
		}
	}
	return 1;
}
