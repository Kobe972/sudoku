#include"game.h"
extern CButton button[20];
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
    SetGameState(PREFACE);
    //load buttons in main menu
    button[ISINGLE_MODE].Create(ISINGLE_MODE, 271, 63, 250, 200, "button1");
    button[IHELP].Create(IHELP, 271, 63, 250, 280, "button2");
    button[IRETURN].Create(IRETURN, 80, 80, 0, 0, "return");
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
    default:
        break;
    }
    HWND hw;
    hw = GetForegroundWindow();
    if (hw == main_window_handle)
    {
        GetCurMsg();//include mouse and keyboard.understand "cur" as "current",not "cursor"
        ProcessButtonMsg();
        //ProcessCheckBoxMsg();
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
            m_eGameState = MAINMENU;
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
        for (int i = ISINGLE_MODE; i <= IHELP; i++)
        {
            button[i].Check();
            if (button[i].m_state == BSTATEUP)
            {
                switch (i)
                {
                case ISINGLE_MODE:
                    SetGameState(CREATE_SU);
                    break;
                case IHELP:
                    SetGameState(HELP);
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
        if (ButtonReturn())
            SetGameState(MAINMENU);
        break;
    default:
        break;
    }
    return;
}
/*
void CGame::ProcessCheckBoxMsg()
{
    switch (m_eGameState)
    {
    case SETTINGS:
        checkbox[JSILENCE].Check();
        g_IsSilent = !checkbox[JSILENCE].m_state;
    default:
        break;
    }
    return;
}*/
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
    MessageBox(main_window_handle, "You win!", "Congratulations", MB_OK);
    SetGameState(MAINMENU);
    return;
}

void CGame::Help()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\Help.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
}

CGame::~CGame()
{
    DInput_Release_Keyboard();
    DInput_Read_Mouse();
    DInput_Shutdown();
    DDraw_Shutdown();
}