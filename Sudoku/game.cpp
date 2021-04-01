#include"game.h"
extern CButton button[20];
extern CCheckBox checkbox[20];
EXTERN_INPUT_DATA()
inline void CGame::SetGameState(CGame::EGameState eGameStateCurrent)
{
    m_eGameState = eGameStateCurrent;
}

inline void CGame::SetWindowHandle(HWND hwnd)
{
    m_hWnd = hwnd;
}
void CGame::GameInit()
{
    g_IsSilent = 0;
    SetGameState(PREFACE);
    //load buttons in main menu
    button[ISINGLE_MODE].Create(ISINGLE_MODE, 271, 63, 250, 200, "button1");
    button[IREC].Create(IREC, 271, 63, 250, 280, "button3");
    button[IHELP].Create(IHELP, 271, 63, 250, 360, "button2");
    button[IRETURN].Create(IRETURN, 80, 80, 0, 0, "return");
    button[IANSWER].Create(IANSWER, 130, 20, 335, 5, "answer");
    checkbox[J_SILENCE].Create(J_SILENCE, 50, 50, 720, 2, "silence", 0);
    Sudoku.GameInit();
    DInput_Init();
    DInput_Init_Keyboard();
    DInput_Init_Mouse();
}
void CGame::GameMain()
{
    switch (m_eGameState)
    {
    case PREFACE:
        Preface();
        break;
    case MAINMENU:
        ShowMenu();
        break;
    case CREATE_SU:
        Create_Sudoku();
        break;
    case HELP:
        Help();
        break;
    case SINGLE_PLAY:
        SinglePlay();
        break;
    case SINGLE_END:
        SingleEnd();
        break;
    case ANSWER:
        ShowAnswer();
        break;
    case PRE_REC:
        PrepareRecordList();
        break;
    case SHOW_REC:
        ShowRecords();
        break;
    default:
        break;
    }
    HWND hw;
    hw = GetForegroundWindow();
    if (hw == main_window_handle)
    {
        GetCurMsg();//include mouse and keyboard.understand "cur" as "current",not "cursor"
        ProcessButtonMsg();
        ProcessCheckBoxMsg();
        ProcessKeyMsg();
        //ProcessSerMessage();
    }
}

void CGame::Preface()
{
    static BOB pref[2];
    static bool inited = false;
    if (!inited)
    {
        for (int i = 0; i < 2; ++i)
            pref[i].Create(0, 0, 800, 600, 30, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME);
        BITMAP_FILE picture1, picture2;
        for (int i = 0; i < 2; ++i) {
            std::string name1, name2;
            name1 = "Preface\\Preface" + std::to_string(i) + ".bmp";
            name2 = "Preface\\Preface" + std::to_string(i + 1) + ".bmp";
            picture1.Load_File(name1.c_str());
            picture2.Load_File(name2.c_str());
            pref[i].Load_Gradual_Frame(&picture1, &picture2, 0, 0, 10);
            pref[i].Set_Anim_Speed(3);
            picture1.Unload_File(); picture2.Unload_File();
        }
        inited = true;
        for (int i = 0; i < 5; ++i) pref[0].Animate();
    }
    Clock t_clock;
    t_clock.Start_Clock();

    static int frame{ -30 };
    DDraw_Fill_Surface(lpddsback, 0);
    static int counter = 0;
    if (frame == 76)
    {
        pref[1].Draw(lpddsback);
        if (counter >= 100) frame++;
        counter++;
        t_clock.Wait_Clock(30);
        return;
    }

    if (frame++ < 0) {
        pref[0].Draw(lpddsback);
    }
    else if (frame < 76) {
        pref[0].Animate();
        pref[0].Draw(lpddsback);
    }
    else if (frame < 150) {
        pref[1].Animate();
        pref[1].Draw(lpddsback);
    }
    else
    {
        pref[1].Draw(lpddsback);
        if (counter >= 170)
        {
            m_eGameState = MAINMENU;
            mciSendString(std::string("open ").append(GetRandomBGMusic()).append(" alias bgmusic").c_str(), NULL, 0, NULL);
            mciSendString("play  bgmusic repeat", NULL, 0, NULL);
        }
        counter++;
    }
    if (keyboard_state[DIK_SPACE] & 0x80)
        counter = frame = 200;
    t_clock.Wait_Clock(30);
}

void CGame::GetCurMsg()
{
    GetCursorPos(&pos);
    ScreenToClient(main_window_handle, &pos);
    DInput_Read_Mouse();
    DInput_Read_Keyboard();
    return;
}

bool CGame::ButtonReturn() {
    button[IRETURN].Check();
    if (button[IRETURN].m_state == BSTATEUP) {
        if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
        button[IRETURN].m_state = BSTATENORMAL;
        return true;
    }
    return false;
}


void CGame::ProcessButtonMsg()
{
    switch (m_eGameState)
    {
    case MAINMENU:
        for (int i = ISINGLE_MODE; i <= IREC; i++)
        {
            button[i].Check();
            if (button[i].m_state == BSTATEUP)
            {
                if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
                switch (i)
                {
                case ISINGLE_MODE:
                    SetGameState(CREATE_SU);
                    break;
                case IHELP:
                    SetGameState(HELP);
                    break;
                case IREC:
                    SetGameState(PRE_REC);
                    break;
                default:
                    break;
                }
                button[i].m_state = BSTATENORMAL;
            }
        }
        break;
    case HELP:
        if (ButtonReturn()) SetGameState(MAINMENU);
        break;
    case SINGLE_PLAY:
        button[IANSWER].Check();
        if (button[IANSWER].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            if (IDYES == MessageBox(main_window_handle, "Are you sure to give up?", "Attention", MB_YESNO))
                SetGameState(ANSWER);
            button[IANSWER].m_state = BSTATENORMAL;
        }
        if (ButtonReturn())
        {
            if (IDYES == MessageBox(main_window_handle, "Are you sure to give up?", "Attention", MB_YESNO))
                SetGameState(MAINMENU);
        }  
        break;
    case ANSWER:
        if (ButtonReturn())
            SetGameState(MAINMENU);
        break;
    case SHOW_REC:
        if (ButtonReturn())
            SetGameState(MAINMENU);
     break;
    default:
        break;
    }
    return;
}

void CGame::ProcessCheckBoxMsg()
{
    bool last = g_IsSilent;
    checkbox[J_SILENCE].Check();
    g_IsSilent = checkbox[J_SILENCE].m_state;
    if (last == 1 && g_IsSilent == 0)
    {
        mciSendString(std::string("open ").append(GetRandomBGMusic()).append(" alias bgmusic").c_str(), NULL, 0, NULL);
        mciSendString("play  bgmusic repeat", NULL, 0, NULL);
    }
    if (last == 0 && g_IsSilent == 1)
    {
        mciSendString("stop bgmusic", NULL, 0, NULL);
        mciSendString("close bgmusic",NULL,0,NULL);
    }
    return;
}
void CGame::ProcessKeyMsg()
{
    switch (m_eGameState)
    {
    case SINGLE_PLAY:
        Sudoku.CheckKey();
        break;
    //case SELECT_HARDNESS:
        //inputbox[IHARDSHIPBOX].Check();
        //break;
    default:
        break;
    }
    return;
}

int firstopen = true, firststop = true;
void CGame::ShowMenu()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\MainMenu.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[ISINGLE_MODE].Draw();
    button[IHELP].Draw();
    button[IREC].Draw();
    checkbox[J_SILENCE].Draw();
    return;
}

void CGame::Create_Sudoku()
{
    Sudoku.Create();
    SetGameState(SINGLE_PLAY);
}

void CGame::SinglePlay()
{
    Sudoku.CheckFocus();
    Sudoku.Draw();
    if (Sudoku.m_win) SetGameState(SINGLE_END);
}

void CGame::SingleEnd()
{
    FILE* InFile;
    if (_access(".\\data\\", 0) == -1)
         _mkdir(".\\data");
    if (_access(".\\data\\record.dat", 0) != -1)
         InFile = fopen(".\\data\\record.dat", "rb+");
    else
         InFile = fopen(".\\data\\record.dat", "wb+");
    FILE* OutFile = fopen(".\\data\\tmp.dat", "wb");
    RecordItem tmp,input,tmplist[11];
    int new_best=0,num=0;
    tmp.difficulty = Sudoku.m_difficulty;
    tmp.T_Consuming = Sudoku.duration;
    tmp.global = 2 * Sudoku.m_difficulty * 100 / Sudoku.duration; 
    while (fread(&input, sizeof(RecordItem), 1, InFile))
    {
        if (input.global > new_best) new_best = input.global;
        tmplist[num ++] = input;
    }
    tmplist[num ++] = tmp;
    qsort(tmplist, num, sizeof(RecordItem), comp);
    for (int i = 0; i < min(10,num); i++)
    {
        fwrite(&tmplist[i], sizeof(RecordItem), 1, OutFile);
    }
    if(tmp.global>new_best) MessageBox(main_window_handle, "New best!", "Congratulations", MB_OK);
    else MessageBox(main_window_handle, "You win!", "Congratulations", MB_OK);
    SetGameState(MAINMENU);
    fclose(InFile);
    fclose(OutFile);
    DeleteFile(".\\data\\record.dat");
    rename(".\\data\\tmp.dat", ".\\data\\record.dat");
    return;
}

void CGame::ShowAnswer()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;//绘制背景
    bitmap->Load_File(".\\background\\sky.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
    char out[50];
    CFont CurText;
    //绘制网格
    RECT coor = { 0,0,Sudoku.m_Width,Sudoku.m_Height }, boarder;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)//i行j列
        {
            boarder.left = (800 - 9 * Sudoku.m_Width) / 2 + Sudoku.m_Width * j;
            boarder.top = (600 - 9 * Sudoku.m_Height) / 2 + Sudoku.m_Height * i;
            boarder.right = boarder.left + Sudoku.m_Width;
            boarder.bottom = boarder.top + Sudoku.m_Height;
            //绘制表格
            if (i / 3 == j / 3 || i / 3 + j / 3 == 2)
            {
                lpddsback->Blt(&boarder, Sudoku.m_grid[0], &coor, DDBLT_WAIT, NULL);
            }
            else
            {
                lpddsback->Blt(&boarder, Sudoku.m_grid[1], &coor, DDBLT_WAIT, NULL);
            }
            //绘制数字
            lpddsback->GetDC(&CurText.hdc);
            CurText.SetType(Sudoku.m_Height / 6 * 5, Sudoku.m_Width / 12 * 5, 15);
            CurText.Uself();
            if (Sudoku.m_const[i][j] == 1) SetTextColor(CurText.hdc, RGB(0, 0, 0));
            else SetTextColor(CurText.hdc, RGB(255, 0, 0));
            SetBkMode(CurText.hdc, TRANSPARENT);
            sprintf(out, "%d", Sudoku.m_ans[i][j]);
            TextOut(CurText.hdc, boarder.left + Sudoku.m_Width / 24 * 7, boarder.top + Sudoku.m_Height / 12, out, strlen(out));
            lpddsback->ReleaseDC(CurText.hdc);
        }
    }
    lpddsback->GetDC(&CurText.hdc);
    CurText.SetType(25, 9, 3);
    CurText.Uself();
    SetTextColor(CurText.hdc, RGB(0, 0, 0));
    SetBkMode(CurText.hdc, TRANSPARENT);
    sprintf(out, "Difficulty:%d/1000", Sudoku.m_difficulty);
    CurText.SetType(25, 9, 5);
    CurText.Uself();
    TextOut(CurText.hdc, (800 - 9 * Sudoku.m_Width) / 2, 2, out, strlen(out));
    TextOut(CurText.hdc, 365, 5, "Solution", strlen("Solution"));
    lpddsback->ReleaseDC(CurText.hdc);
}

void CGame::Help()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\Help.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
}

void CGame::PrepareRecordList()
{
    FILE* InFile;
    if (_access(".\\data\\", 0) == -1)
        _mkdir(".\\data");
    if (_access(".\\data\\record.dat", 0) != -1)
        InFile = fopen(".\\data\\record.dat", "rb");
    else
        InFile = fopen(".\\data\\record.dat", "wb+");
    RecordItem* ite=new RecordItem;
    RecordItem records[10];
    m_RecordList.clear();
    int num = 0;
    while (fread(ite, sizeof(RecordItem), 1, InFile))
    {
        records[num++] = *ite;
    }
    int best=0;
    qsort(records, num, sizeof(RecordItem), comp);
    for (int i = 0; i < min(10, num); i++) //选出至多前十个
    {
        m_RecordList.push_back(records[i]);
    }
    SetGameState(SHOW_REC);
    fclose(InFile);
    return;
}

void CGame::ShowRecords()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\records.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
    CFont CurText;
    char out[20];
    lpddsback->GetDC(&CurText.hdc);
    CurText.SetType(20, 10, 10);
    CurText.Uself();
    SetTextColor(CurText.hdc, RGB(0, 0, 0));
    SetBkMode(CurText.hdc, TRANSPARENT);
    int spacex = 180,spacey=40;
    TextOut(CurText.hdc, 85, 120, "Rank", strlen("Rank"));
    TextOut(CurText.hdc, 85+spacex, 120, "Time Consumed", strlen("Time Consumed"));
    TextOut(CurText.hdc, 85+ spacex*2, 120 , "Difficulty", strlen("Difficulty"));
    TextOut(CurText.hdc, 85 + spacex*3, 120, "Score", strlen("Score"));
    for (int i = 0; i < m_RecordList.size(); i++)
    {
        TextOut(CurText.hdc, 85, 120 + (i + 1) * spacey,std::to_string(i+1).c_str(), strlen(std::to_string(i + 1).c_str()));
        sprintf(out, "%02d:%02d:%02d", (int)m_RecordList[i].T_Consuming / 3600, (int)m_RecordList[i].T_Consuming % 3600 / 60, (int)m_RecordList[i].T_Consuming % 60);
        TextOut(CurText.hdc, 85 + spacex, 120+(i + 1) * spacey, out, strlen(out));
        TextOut(CurText.hdc, 85 + spacex * 2, 120 + (i + 1) * spacey, std::to_string(m_RecordList[i].difficulty).c_str(), strlen(std::to_string(m_RecordList[i].difficulty).c_str()));
        TextOut(CurText.hdc, 85 + spacex * 3, 120 + (i + 1) * spacey, std::to_string(m_RecordList[i].global).c_str(), strlen(std::to_string(m_RecordList[i].global).c_str()));
    }
    lpddsback->ReleaseDC(CurText.hdc);
}

CGame::~CGame()
{
    DInput_Release_Keyboard();
    DInput_Read_Mouse();
    DInput_Shutdown();
    DDraw_Shutdown();
}

string GetRandomBGMusic()
{
    string path=".\\Sounds\\bgmusic";
    vector<string> files;
    srand((unsigned)time(NULL));
    //文件句柄 
    long   hFile = 0;
    //文件信息 
    struct  _finddata_t  fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if (!(fileinfo.attrib & _A_SUBDIR))
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
    return files[rand() % files.size()];
}

int comp(const void* p1, const void* p2)
{
    RecordItem i1 = *(RecordItem*)p1;
    RecordItem i2 = *(RecordItem*)p2;
    if (i1.global > i2.global) return -1;
    else if (i1.global < i2.global) return 1;
    else return 0;
    return 0;
}
