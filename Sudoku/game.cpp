#include"game.h"
std::string g_ServeID;
Clock g_gametime, g_connecttime;
extern CButton button[20];
extern CCheckBox checkbox[20];
extern CInputBox inputbox[5];
extern std::queue<Msg>g_Send, g_Recv; //队列中的消息
extern SOCKET g_connSocket;
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
    m_loggedin = m_connected = 0;
    SetGameState(PREFACE);
    //load buttons in main menu
    button[ISINGLE_MODE].Create(ISINGLE_MODE, 271, 63, 250, 200, "button1");
    button[IMULTI_MODE].Create(IMULTI_MODE, 271, 63, 250, 280, "button4");
    button[IREC].Create(IREC, 271, 63, 250, 360, "button3");
    button[IHELP].Create(IHELP, 271, 63, 250, 440, "button2");
    button[IRETURN].Create(IRETURN, 80, 80, 0, 0, "return");
    button[IANSWER].Create(IANSWER, 130, 20, 335, 5, "answer");
    button[IOK].Create(IOK, 271, 63, 529, 500, "OK");
    button[ILOGIN].Create(ILOGIN, 189, 63, 0, 525, "login");
    button[IREG].Create(IREG, 183, 63, 617, 525, "reg");
    button[IJOIN].Create(IJOIN, 271, 63, 264, 250, "join");
    button[ICREATE].Create(ICREATE, 271, 63, 264, 400, "create");
    button[IBEGIN].Create(IBEGIN, 217, 63, 292, 400, "begin");
    button[IRANK].Create(IRANK, 80, 80, 30, 10, "rank");
    inputbox[IIPBOX].Create(IIPBOX, 250, 25, 80, 250, false);
    inputbox[IPASSWORD].Create(IPASSWORD, 250, 25, 80, 350, true);
    inputbox[IROOMBOX].Create(IROOMBOX, 250, 25, 80, 300, false);
    checkbox[J_SILENCE].Create(J_SILENCE, 50, 50, 720, 10, "silence", 0);
    Sudoku.GameInit();
    DInput_Init();
    DInput_Init_Keyboard();
    DInput_Init_Mouse();
    int mode = 1;
    int len = sizeof(SOCKADDR);
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    g_connSocket = socket(AF_INET, SOCK_STREAM, 0);
    ioctlsocket(g_connSocket, FIONBIO, (u_long FAR*) & mode);
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
    case CONNECTTOSERVE:
        ConnectToServ();
        break;
    case LOGIN:
        Login();
        break;
    case REGISTRY:
        Reg();
        break;
    case WAITTOCONNECT:
        WaitToConnect();
        break;
    case CHOOSE_MODE:
        ChooseMode();
        break;
    case CREATE_ROOM:
        CreateRoom();
        break;
    case JOIN_ROOM:
        JoinRoom();
        break;
    case WAIT_TO_BEGIN:
        WaitToBegin();
        break;
    case WAIT_FOR_BEGINNING:
        WaitForBeginning();
        break;
    case WAIT_GRID:
        WaitGrid();
        break;
    case MULTIPLAYER:
        MultiPlayer();
        break;
    case MULTIEND:
        MultiEnd();
        break;
    case SHOW_RANK:
        ShowRank();
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
        ProcessSerMessage();
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

void CGame::ConnectToServ()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\ConnectToServ.bmp"); //wait for load
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IOK].Draw();
    inputbox[IIPBOX].Draw();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
    return;
}

void CGame::WaitGrid()
{
    memset(Sudoku.m_const, 0, sizeof(Sudoku.m_const));
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;//绘制背景
    bitmap->Load_File(".\\background\\sky.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
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
        }
    }
    checkbox[J_SILENCE].Draw();
    DDraw_Flip();
    Sudoku.duration = 0;
    Sudoku.m_win = 0;
    return;
}


void CGame::ProcessButtonMsg()
{
    Msg message;
    switch (m_eGameState)
    {
    case MAINMENU:
        for (int i = ISINGLE_MODE; i <= IREG; i++)
        {
            button[i].Check();
            if (button[i].m_state == BSTATEUP)
            {
                if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
                switch (i)
                {
                case ISINGLE_MODE:
                    if (!m_loggedin)
                    {
                           if (IDYES == MessageBox(main_window_handle, "You've not logged in,so your record won't be saved in the server.Are you sure to start?", "Attention", MB_YESNO))
                                SetGameState(CREATE_SU);
                            else SetGameState(MAINMENU);
                    }
                    else SetGameState(CREATE_SU);
                    break;
                case IHELP:
                    SetGameState(HELP);
                    break;
                case IREC:
                    SetGameState(PRE_REC);
                    break;
                case IMULTI_MODE:
                    if (m_loggedin)
                        SetGameState(CHOOSE_MODE);
                    else SetGameState(LOGIN);
                    break;
                case IREG:
                    SetGameState(REGISTRY);
                    break;
                case ILOGIN:
                    SetGameState(LOGIN);
                    break;
                case IRANK:
                    if (!m_loggedin)
                    {
                        SetGameState(LOGIN);
                        break;
                    }
                    message.ID = RANK_RQ;
                    g_Send.push(message);
                    SetGameState(SHOW_RANK);
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
    case CONNECTTOSERVE:
        button[IOK].Check();
        if (button[IOK].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            g_ServeID = inputbox[IIPBOX].m_input;
            g_ServeID = g_ServeID.substr(0, g_ServeID.size() - 1);
            SetGameState(WAITTOCONNECT);
            g_connecttime.Start_Clock();
            button[IOK].m_state = BSTATENORMAL;
            memset(inputbox[IIPBOX].m_input, 0, sizeof(inputbox[IIPBOX].m_input));
            inputbox[IIPBOX].m_input[0] = '|';
        }
        if (ButtonReturn()) {
            memset(inputbox[IIPBOX].m_input, 0, sizeof(inputbox[IIPBOX].m_input));
            inputbox[IIPBOX].m_input[0] = '|';
            SetGameState(MAINMENU);
        }
        break;
    case WAITTOCONNECT:
        if (ButtonReturn())
        {
            SetGameState(MAINMENU);
        }
        break;
    case LOGIN:
        button[IOK].Check();
        if (button[IOK].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            Msg Tmessage;
            Tmessage.ID = LOG;
            inputbox[IIPBOX].Load(Tmessage.string1);
            inputbox[IPASSWORD].Load(Tmessage.string2);
            g_Send.push(Tmessage);
            m_username = Tmessage.string1;
            memset(inputbox[IIPBOX].m_input, 0, sizeof(inputbox[IIPBOX].m_input));
            memset(inputbox[IPASSWORD].m_input, 0, sizeof(inputbox[IPASSWORD].m_input));
            inputbox[IIPBOX].m_input[0] = '|';
            inputbox[IPASSWORD].m_input[0] = '|';
            button[IOK].m_state = BSTATENORMAL;
        }
        if (ButtonReturn()) {
            memset(inputbox[IIPBOX].m_input, 0, sizeof(inputbox[IIPBOX].m_input));
            memset(inputbox[IPASSWORD].m_input, 0, sizeof(inputbox[IPASSWORD].m_input));
            inputbox[IIPBOX].m_input[0] = '|';
            inputbox[IPASSWORD].m_input[0] = '|';
            SetGameState(MAINMENU);
        }
        break;
    case REGISTRY:
        button[IOK].Check();
        if (button[IOK].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            Msg Tmessage;
            Tmessage.ID = REG;
            inputbox[IIPBOX].Load(Tmessage.string1);
            inputbox[IPASSWORD].Load(Tmessage.string2);
            g_Send.push(Tmessage);
            m_username = Tmessage.string1;
            memset(inputbox[IIPBOX].m_input, 0, sizeof(inputbox[IIPBOX].m_input));
            memset(inputbox[IPASSWORD].m_input, 0, sizeof(inputbox[IPASSWORD].m_input));
            inputbox[IIPBOX].m_input[0] = '|';
            inputbox[IPASSWORD].m_input[0] = '|';
            button[IOK].m_state = BSTATENORMAL;
        }
        if (ButtonReturn()) {
            memset(inputbox[IIPBOX].m_input, 0, sizeof(inputbox[IIPBOX].m_input));
            memset(inputbox[IPASSWORD].m_input, 0, sizeof(inputbox[IPASSWORD].m_input));
            inputbox[IIPBOX].m_input[0] = '|';
            inputbox[IPASSWORD].m_input[0] = '|';
            SetGameState(MAINMENU);
        }
        break;
    case CHOOSE_MODE:
        button[IJOIN].Check();
        button[ICREATE].Check();
        if (button[IJOIN].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            SetGameState(JOIN_ROOM);
            button[IJOIN].m_state = BSTATENORMAL;
        }
        if (button[ICREATE].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            SetGameState(CREATE_ROOM);
            button[ICREATE].m_state = BSTATENORMAL;
        }
        if (ButtonReturn()) {
            SetGameState(MAINMENU);
        }
        break;
    case JOIN_ROOM:
        button[IOK].Check();
        if (!waiting_srv)
        {
            if (button[IOK].m_state == BSTATEUP)
            {
                if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
                Msg Tmessage;
                char tmp[10];
                Tmessage.ID = JOIN;
                inputbox[IROOMBOX].Load(tmp);
                Tmessage.num[0] = atoi(tmp);
                g_Send.push(Tmessage);
                waiting_srv = 1;
                button[IOK].m_state = BSTATENORMAL;
                memset(inputbox[IROOMBOX].m_input, 0, sizeof(inputbox[IROOMBOX].m_input));
                inputbox[IROOMBOX].m_input[0] = '|';
            }
        }
        if (ButtonReturn()) {
            memset(inputbox[IROOMBOX].m_input, 0, sizeof(inputbox[IROOMBOX].m_input));
            inputbox[IROOMBOX].m_input[0] = '|';
            SetGameState(CHOOSE_MODE);
        }
        break;
    case CREATE_ROOM:
        button[IOK].Check();
        if (!waiting_srv)
        {
            if (button[IOK].m_state == BSTATEUP)
            {
                if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
                Msg Tmessage;
                char tmp[10];
                Tmessage.ID = CREATE;
                inputbox[IROOMBOX].Load(tmp);
                Tmessage.num[0] = atoi(tmp);
                g_Send.push(Tmessage);
                waiting_srv = 1;
                button[IOK].m_state = BSTATENORMAL;
                memset(inputbox[IROOMBOX].m_input, 0, sizeof(inputbox[IROOMBOX].m_input));
                inputbox[IROOMBOX].m_input[0] = '|';
            }
        }
        if (ButtonReturn()) {
            memset(inputbox[IROOMBOX].m_input, 0, sizeof(inputbox[IROOMBOX].m_input));
            inputbox[IROOMBOX].m_input[0] = '|';
            SetGameState(CHOOSE_MODE);
        }
        break;
    case WAIT_TO_BEGIN:
        button[IBEGIN].Check();
        if (button[IBEGIN].m_state == BSTATEUP)
        {
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            Msg Tmessage;
            Tmessage.ID = BEGIN_GAME;
            g_Send.push(Tmessage);
            button[IBEGIN].m_state = BSTATENORMAL;
            SetGameState(WAIT_GRID);
        }
        if (ButtonReturn()) {
            Msg Tmessage;
            Tmessage.ID = CHANGE;
            g_Send.push(Tmessage);
            is_host = 0;
            SetGameState(CREATE_ROOM);
        }
        break;
    case WAIT_FOR_BEGINNING:
        if (ButtonReturn()) {
            Msg Tmessage;
            Tmessage.ID = CHANGE;
            g_Send.push(Tmessage);
            is_host = 0;
            SetGameState(JOIN_ROOM);
        }
    case MULTIPLAYER:
        button[IANSWER].Check();
        if (button[IANSWER].m_state == BSTATEUP)
        {
            if (m_ranklist.size() == 0 && joined > 1)
            {
                MessageBox(main_window_handle, "You can't see the answer until a player wins.", "Attention", MB_OK);
                button[IANSWER].m_state = BSTATENORMAL;
                break;
            }
            Sudoku.duration = 0;
            if (!g_IsSilent) mciSendString("play .\\Sounds\\click\\0.wav", NULL, 0, NULL);
            if (IDYES == MessageBox(main_window_handle, "Are you sure to give up?", "Attention", MB_YESNO))
                SetGameState(ANSWER);
            Msg Tmessage;
            Tmessage.ID = CHANGE;
            g_Send.push(Tmessage);
            is_host = 0;
            button[IANSWER].m_state = BSTATENORMAL;
        }
        if (ButtonReturn()) {
            if (IDYES == MessageBox(main_window_handle, "Are you sure to give up?", "Attention", MB_YESNO))
            {
                memset(Sudoku.m_Sudoku, 0, sizeof(Sudoku.m_Sudoku));
                memset(Sudoku.m_const, 0, sizeof(Sudoku.m_const));
                Sudoku.duration = 0;
                Msg Tmessage;
                Tmessage.ID = CHANGE;
                g_Send.push(Tmessage);
                is_host = 0;
                SetGameState(MAINMENU);
            }       
        }
        break;
    case MULTIEND:
        if (ButtonReturn()) {
            SetGameState(MAINMENU);
        }
        break;
    case SHOW_RANK:
        if (ButtonReturn()) {
            SetGameState(MAINMENU);
        }
        break;
    case WAIT_GRID:
        if (ButtonReturn()) {
            Msg Tmessage;
            Tmessage.ID = CHANGE;
            g_Send.push(Tmessage);
            is_host = 0;
            SetGameState(JOIN_ROOM);
        }
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
    case MULTIPLAYER:
        Sudoku.CheckKey();
        break;
    case CONNECTTOSERVE:
        inputbox[IIPBOX].Check();
        break;
    case LOGIN:case REGISTRY:
        inputbox[IIPBOX].Check();
        inputbox[IPASSWORD].Check();
        break;
    case JOIN_ROOM:case CREATE_ROOM:
        inputbox[IROOMBOX].Check();
        break;
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
    button[IMULTI_MODE].Draw();
    button[ILOGIN].Draw();
    button[IREG].Draw();
    button[IRANK].Draw();
    checkbox[J_SILENCE].Draw();
    if (m_loggedin) {
        CFont TCurText;
        lpddsback->GetDC(&TCurText.hdc);
        TCurText.SetType(25, 10, 15);
        TCurText.Uself();
        SetBkMode(TCurText.hdc, TRANSPARENT);
        SetTextColor(TCurText.hdc, RGB(255, 255, 0));
        SIZE siz;
        std::string TCur = "Username:" + m_username;
        GetTextExtentPoint32(TCurText.hdc, TCur.c_str(), strlen(TCur.c_str()), &siz);
        TextOut(TCurText.hdc, (800-siz.cx)/2, 20, TCur.c_str(), TCur.size());
        lpddsback->ReleaseDC(TCurText.hdc);
    }
    return;
}

void CGame::Create_Sudoku()
{
    Sudoku.Create();
    button[IRETURN].Draw();
    SetGameState(SINGLE_PLAY);
}

void CGame::SinglePlay()
{
    Sudoku.CheckFocus();
    Sudoku.Draw();
    button[IANSWER].Draw();
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
    if (m_loggedin)
    {
        Msg Tmessage;
        Tmessage.ID = END_GAME;
        Tmessage.num[0] = tmp.T_Consuming;
        Tmessage.num[1] = tmp.difficulty;
        Tmessage.num[2] = tmp.global;
        g_Send.push(Tmessage);
    }
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
    if(tmp.global>new_best) MessageBox(main_window_handle, (std::string("New best!\n") + std::string("Your score is") + std::to_string(tmp.global)).c_str(), "Congratulations", MB_OK);
    else MessageBox(main_window_handle, (std::string("You win!\n")+std::string("Your score is")+std::to_string(tmp.global)).c_str(), "Congratulations", MB_OK);
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
void CGame::Login()
{
    if (!m_connected) {
        SetGameState(CONNECTTOSERVE);
        return;
    }
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\login.bmp"); //wait for load
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IOK].Draw();
    checkbox[J_SILENCE].Draw();
    inputbox[IIPBOX].Draw();
    inputbox[IPASSWORD].Draw();
    button[IRETURN].Draw();
    return;
}
void CGame::Reg()
{
    if (!m_connected) {
        m_eGameState = eGameStateConnectToServ;
        return;
    }

    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\registry.bmp"); //wait for load
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IOK].Draw();
    inputbox[IIPBOX].Draw();
    inputbox[IPASSWORD].Draw();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
    return;
}
void CGame::WaitToConnect()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\WaitToConnect.bmp"); //wait for load
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();

    sockaddr_in addrSrv;
    addrSrv.sin_addr.S_un.S_addr = inet_addr(g_ServeID.c_str());
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(8000);
    if (connect(g_connSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != SOCKET_ERROR)
    {
        m_connected = true;
        MessageBox(NULL, "Connect Successfully", "Attention", MB_OK);
        CreateThread(NULL, 0, CreateCilent, 0, 0, NULL);
        SetGameState(MAINMENU);
        return;
    }
    else
    {
        int err = WSAGetLastError();
        if (err == WSAEISCONN)
        {
            m_connected = true;
            MessageBox(NULL, "Connect Successfully", "Attention", MB_OK);
            CreateThread(NULL, 0, CreateCilent, 0, 0, NULL);
            SetGameState(MAINMENU);
            return;
        }
    }
    int TCurTime = g_connecttime.Get_Time();
    if (TCurTime >= 30) {
        MessageBox(NULL, "Connection Time Limit Exceeded!", "Attention", MB_OK);
        SetGameState(CONNECTTOSERVE);
    }

    std::string TContext1 = "The Ip You are trying to connect is:";
    std::string TContext2 = g_ServeID;
    std::string TContext3 = "You have been waitting for: " + std::to_string(TCurTime) + "s";
    CFont TCurText;
    lpddsback->GetDC(&TCurText.hdc);
    TCurText.SetType(50, 20, 500);
    TCurText.Uself();
    SetBkMode(TCurText.hdc, TRANSPARENT);
    SetTextColor(TCurText.hdc, RGB(0, 0, 0));
    TextOut(TCurText.hdc, 100, 150, TContext1.c_str(), TContext1.size());
    TextOut(TCurText.hdc, 100, 250, TContext2.c_str(), TContext2.size());
    TextOut(TCurText.hdc, 100, 350, TContext3.c_str(), TContext3.size());
    lpddsback->ReleaseDC(TCurText.hdc);
    return;
}
void CGame::ProcessSerMessage()
{
    if (!m_connected) return;
    for (; !g_Recv.empty();)
    {
        Msg Tmessage = g_Recv.front(); g_Recv.pop();
        switch (Tmessage.ID)
        {
        case RANK_ITEM:
            if (strcmp(Tmessage.string2, "?") != 0)
            {
                Load_Rank(Tmessage.num[0], Tmessage.num[1], Tmessage.num[2], Tmessage.string1,atoi(Tmessage.string2));
                break;
            }
            Load_Rank(Tmessage.num[0],Tmessage.num[1],Tmessage.num[2], Tmessage.string1);
            break;
        case CLEAR_RANK:
            m_ranklist.clear();
            break;
        case LOG_RESULT:
            m_loggedin = Tmessage.num[0];
            if (Tmessage.num[0]) {
                MessageBox(NULL, "Log in Successfully", "Attention", MB_OK);
                SetGameState(MAINMENU);
            }
            else
                MessageBox(NULL, "Log fails", "Attention", MB_OK);
            break;
        case REG_RESULT:
            if (Tmessage.num[0])
            {
                MessageBox(NULL, "Regist Successfully", "Attention", MB_OK);
                SetGameState(MAINMENU);
            }   
            else
                MessageBox(NULL, "Registry fails", "Attention", MB_OK);
            break;
        case BEGIN_GAME:
            if (m_eGameState == WAIT_TO_BEGIN || m_eGameState == WAIT_FOR_BEGINNING)
            {
                SetGameState(WAIT_GRID);
                m_ranklist.clear();
            }
            else
            {
                Sudoku.duration = 0;
                Sudoku.start_time = clock();
                memset(Sudoku.m_Sudoku, 0, sizeof(Sudoku.m_Sudoku));
                memset(Sudoku.m_const, 0, sizeof(Sudoku.m_const));
                joined = Tmessage.num[1];
                int first = 1;
                Sudoku.m_difficulty = Tmessage.num[0];
                for (int i = 0; i < 9; i++)
                {
                    for (int j = 0; j < 9; j++)
                    {
                        Sudoku.m_Sudoku[i][j] = Tmessage.string1[9 * i + j]-'0';
                        Sudoku.m_ans[i][j] = Tmessage.string2[9 * i + j] - '0';
                        if (Sudoku.m_Sudoku[i][j] != 0)
                        {
                            Sudoku.m_const[i][j] = 1;
                        }
                        else
                        {
                            if (first)
                            {
                                Sudoku.focus.x = j;
                                Sudoku.focus.y = i;
                                first = 1;
                            }
                        }
                    }
                }
                SetGameState(MULTIPLAYER);
            }
            break;
        case DISCONNECT:
            MessageBox(NULL, "Disconnect with the server!", "Attention", MB_OK);
            m_connected = m_loggedin = false;
            SetGameState(MAINMENU);
            break;
        case JOIN:
            if (Tmessage.num[0] == 1)
            {
                is_host = 0;
                waiting_srv = 0;
                SetGameState(WAIT_FOR_BEGINNING);
            }
            else
            {
                MessageBox(main_window_handle, "The room doesn't have a host!Maybe You should create the room.", "Error", MB_OK);
                waiting_srv = 0;
            }
            break;
        case CREATE:
            if (Tmessage.num[0] == 1)
            {
                is_host = 1;
                waiting_srv = 0;
                SetGameState(WAIT_TO_BEGIN);
            }
            else
            {
                MessageBox(main_window_handle, "The room has been created!Try another room or join it.", "Error", MB_OK);
                waiting_srv = 0;
            }
            break;
        case CHANGE:
            if (strcmp(Tmessage.string1, m_username.c_str()) == 0)
            {
                is_host = 1;
                if(m_eGameState==WAIT_FOR_BEGINNING) SetGameState(WAIT_TO_BEGIN);
            }
            break;
        default:
            break;
        }
    }
}

void CGame::Load_Rank(int best_consume, int best_difficulty, int best_score, char* username,int rank)
{
    RankItem Tcur;
    if (rank!=0)
    {
        Tcur.is_self = 1;
        Tcur.rank = rank;
    }
    Tcur.best_consume = best_consume;
    Tcur.best_difficulty = best_difficulty;
    Tcur.best_score = best_score;
    strcpy(Tcur.name, username);
    m_ranklist.push_back(Tcur);
}
void CGame::ChooseMode()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\choose.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IJOIN].Draw();
    button[ICREATE].Draw();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
}
void CGame::JoinRoom()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\selroom.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    inputbox[IROOMBOX].Draw();
    checkbox[J_SILENCE].Draw();
    button[IOK].Draw();
    button[IRETURN].Draw();
}
void CGame::CreateRoom()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\selroom.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    inputbox[IROOMBOX].Draw();
    checkbox[J_SILENCE].Draw();
    button[IOK].Draw();
    button[IRETURN].Draw();
}

void CGame::WaitToBegin()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\WaitToBegin.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IBEGIN].Draw();
    button[IRETURN].Draw();
    checkbox[J_SILENCE].Draw();
}

void CGame::WaitForBeginning()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\WaitForBeginning.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    checkbox[J_SILENCE].Draw();
    button[IRETURN].Draw();
}

void CGame::MultiPlayer()
{
    Sudoku.CheckFocus();
    Sudoku.Draw();
    button[IANSWER].Draw();
    if (Sudoku.m_win)
    {
        Msg message;
        message.ID = END_GAME;
        message.num[0] = Sudoku.duration;
        message.num[1] = Sudoku.m_difficulty;
        message.num[2]= 2 * Sudoku.m_difficulty * 100 / Sudoku.duration;
        g_Send.push(message);
        message.ID = RANK_RQ;
        message.num[0] = message.num[1] = message.num[2] = 0;
        g_Send.push(message);
        FILE* InFile;
        if (_access(".\\data\\", 0) == -1)
            _mkdir(".\\data");
        if (_access(".\\data\\record.dat", 0) != -1)
            InFile = fopen(".\\data\\record.dat", "rb+");
        else
            InFile = fopen(".\\data\\record.dat", "wb+");
        FILE* OutFile = fopen(".\\data\\tmp.dat", "wb");
        RecordItem tmp, input, tmplist[11];
        int new_best = 0, num = 0;
        tmp.difficulty = Sudoku.m_difficulty;
        tmp.T_Consuming = Sudoku.duration;
        tmp.global = 2 * Sudoku.m_difficulty * 100 / Sudoku.duration;
        while (fread(&input, sizeof(RecordItem), 1, InFile))
        {
            if (input.global > new_best) new_best = input.global;
            tmplist[num++] = input;
        }
        tmplist[num++] = tmp;
        qsort(tmplist, num, sizeof(RecordItem), comp);
        for (int i = 0; i < min(10, num); i++)
        {
            fwrite(&tmplist[i], sizeof(RecordItem), 1, OutFile);
        }
        if (tmp.global > new_best) MessageBox(main_window_handle, "New best!", "Congratulations", MB_OK);
        else MessageBox(main_window_handle, "You win!", "Congratulations", MB_OK);
        fclose(InFile);
        fclose(OutFile);
        DeleteFile(".\\data\\record.dat");
        rename(".\\data\\tmp.dat", ".\\data\\record.dat");
        SetGameState(MULTIEND);
    }
    return;
}

void CGame::MultiEnd()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\rank.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    checkbox[J_SILENCE].Draw();
    button[IRETURN].Draw();
    if (!m_ranklist.empty())
    {
        char out[100], tmp[100];
        RankItem self;
        CFont CurText;
        int self_encountered = -1;
        lpddsback->GetDC(&CurText.hdc);
        CurText.SetType(20, 10, 10);
        CurText.Uself();
        SetTextColor(CurText.hdc, RGB(0, 0, 0));
        SetBkMode(CurText.hdc, TRANSPARENT);
        int spacex = 180, spacey = 35;
        TextOut(CurText.hdc, 85, 140, "Rank", strlen("Rank"));
        TextOut(CurText.hdc, 85 + spacex, 140, "Name", strlen("Name"));
        TextOut(CurText.hdc, 85 + spacex * 2, 140, "Time Consumed", strlen("Time Consumed"));
        TextOut(CurText.hdc, 85 + spacex * 3, 140, "Score", strlen("Score"));
        int i;
        for (i = 0; i < min(10, m_ranklist.size()); i++)
        {
            if (strcmp(m_ranklist[i].name, m_username.c_str()) == 0)
            {
                self = m_ranklist[i];
                self_encountered = i;
            }
            TextOut(CurText.hdc, 85, 140 + (i + 1) * spacey, std::to_string(i + 1).c_str(), strlen(std::to_string(i + 1).c_str()));
            sprintf(out, "%02d:%02d:%02d", (int)m_ranklist[i].best_consume / 3600, (int)m_ranklist[i].best_consume % 3600 / 60, (int)m_ranklist[i].best_consume % 60);
            TextOut(CurText.hdc, 85 + spacex, 140 + (i + 1) * spacey, m_ranklist[i].name, strlen(m_ranklist[i].name));
            TextOut(CurText.hdc, 85 + spacex * 2, 140 + (i + 1) * spacey, out, strlen(out));
            TextOut(CurText.hdc, 85 + spacex * 3, 140 + (i + 1) * spacey, std::to_string(m_ranklist[i].best_score).c_str(), strlen(std::to_string(m_ranklist[i].best_score).c_str()));
        }
        if (self_encountered==-1)
        {
            TextOut(CurText.hdc, 85, 550, std::to_string(i + 1).c_str(), strlen(std::to_string(i + 1).c_str()));
            sprintf(out, "%02d:%02d:%02d", (int)m_ranklist[i].best_consume / 3600, (int)m_ranklist[i].best_consume % 3600 / 60, (int)m_ranklist[i].best_consume % 60);
            TextOut(CurText.hdc, 85 + spacex, 550, m_ranklist[i].name, strlen(m_ranklist[i].name));
            TextOut(CurText.hdc, 85 + spacex * 2, 550, out, strlen(out));
            TextOut(CurText.hdc, 85 + spacex * 3, 550, std::to_string(m_ranklist[i].best_score).c_str(), strlen(std::to_string(m_ranklist[i].best_score).c_str()));
        }
        else
        {
            TextOut(CurText.hdc, 85, 550, std::to_string(self_encountered + 1).c_str(), strlen(std::to_string(self_encountered + 1).c_str()));
            sprintf(out, "%02d:%02d:%02d", (int)self.best_consume / 3600, (int)self.best_consume % 3600 / 60, (int)self.best_consume % 60);
            TextOut(CurText.hdc, 85 + spacex, 550, self.name, strlen(self.name));
            TextOut(CurText.hdc, 85 + spacex * 2, 550, out, strlen(out));
            TextOut(CurText.hdc, 85 + spacex * 3, 550, std::to_string(self.best_score).c_str(), strlen(std::to_string(self.best_score).c_str()));
        }
        lpddsback->ReleaseDC(CurText.hdc);
    }
    return;
}

void CGame::ShowRank()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\rank.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    checkbox[J_SILENCE].Draw();
    button[IRETURN].Draw();
    if (!m_ranklist.empty())
    {
        char out[100], tmp[100];
        RankItem self;
        CFont CurText;
        int self_encountered = -1;
        lpddsback->GetDC(&CurText.hdc);
        CurText.SetType(20, 10, 10);
        CurText.Uself();
        SetTextColor(CurText.hdc, RGB(0, 0, 0));
        SetBkMode(CurText.hdc, TRANSPARENT);
        int spacex = 145, spacey = 35;
        TextOut(CurText.hdc, 85, 140, "Rank", strlen("Rank"));
        TextOut(CurText.hdc, 85 + spacex, 140, "Name", strlen("Name"));
        TextOut(CurText.hdc, 85 + spacex * 2, 140, "Difficulty", strlen("Difficulty"));
        TextOut(CurText.hdc, 85 + spacex * 3, 140, "Time Used", strlen("Time Used"));
        TextOut(CurText.hdc, 85 + spacex * 4, 140, "Score", strlen("Score"));
        int i;
        for (i = 0; i < min(10, m_ranklist.size()); i++)
        {
            if (strcmp(m_ranklist[i].name, m_username.c_str()) == 0)
            {
                self = m_ranklist[i];
                self_encountered = i;
            }
            TextOut(CurText.hdc, 85, 140 + (i + 1) * spacey, std::to_string(i + 1).c_str(), strlen(std::to_string(i + 1).c_str()));
            sprintf(out, "%02d:%02d:%02d", (int)m_ranklist[i].best_consume / 3600, (int)m_ranklist[i].best_consume % 3600 / 60, (int)m_ranklist[i].best_consume % 60);
            TextOut(CurText.hdc, 85 + spacex, 140 + (i + 1) * spacey, m_ranklist[i].name, strlen(m_ranklist[i].name));
            TextOut(CurText.hdc, 85 + spacex * 2, 140 + (i + 1) * spacey, std::to_string(m_ranklist[i].best_difficulty).c_str(), strlen(std::to_string(m_ranklist[i].best_difficulty).c_str()));
            TextOut(CurText.hdc, 85 + spacex * 3, 140 + (i + 1) * spacey, out, strlen(out));
            TextOut(CurText.hdc, 85 + spacex * 4, 140 + (i + 1) * spacey, std::to_string(m_ranklist[i].best_score).c_str(), strlen(std::to_string(m_ranklist[i].best_score).c_str()));
        }
        if (self_encountered == -1)
        {
            TextOut(CurText.hdc, 85, 550, std::to_string(i + 1).c_str(), strlen(std::to_string(i + 1).c_str()));
            sprintf(out, "%02d:%02d:%02d", (int)m_ranklist[i].best_consume / 3600, (int)m_ranklist[i].best_consume % 3600 / 60, (int)m_ranklist[i].best_consume % 60);
            TextOut(CurText.hdc, 85 + spacex, 550, m_ranklist[i].name, strlen(m_ranklist[i].name));
            TextOut(CurText.hdc, 85 + spacex * 2, 550, std::to_string(m_ranklist[i].best_difficulty).c_str(), strlen(std::to_string(m_ranklist[i].best_difficulty).c_str()));
            TextOut(CurText.hdc, 85 + spacex * 3, 550, out, strlen(out));
            TextOut(CurText.hdc, 85 + spacex * 4, 550, std::to_string(m_ranklist[i].best_score).c_str(), strlen(std::to_string(m_ranklist[i].best_score).c_str()));
        }
        else
        {
            TextOut(CurText.hdc, 85, 550, std::to_string(self_encountered + 1).c_str(), strlen(std::to_string(self_encountered + 1).c_str()));
            sprintf(out, "%02d:%02d:%02d", (int)self.best_consume / 3600, (int)self.best_consume % 3600 / 60, (int)self.best_consume % 60);
            TextOut(CurText.hdc, 85 + spacex, 550, self.name, strlen(self.name));
            TextOut(CurText.hdc, 85 + spacex * 2, 550, std::to_string(self.best_difficulty).c_str(), strlen(std::to_string(self.best_difficulty).c_str()));
            TextOut(CurText.hdc, 85 + spacex * 3, 550, out, strlen(out));
            TextOut(CurText.hdc, 85 + spacex * 4, 550, std::to_string(self.best_score).c_str(), strlen(std::to_string(self.best_score).c_str()));
        }
        lpddsback->ReleaseDC(CurText.hdc);
    }
    return;
}