#include "h.h"
using std::string;
const char* Sweepmine::WIN_WORD = "YOU  WIN";
const char* Sweepmine::OVER_WORD = "GAME  OVER";
const char* Sweepmine::MY_SIGN = "Ϻ��Ӳ";

Sweepmine::Sweepmine(int xlen, int ylen, int mine, string path)
{
    if (xlen <= 1 || ylen <= 1 || mine <= 0 || path.empty() || xlen * ylen / MAX_MINE_RATE < 1) //��������
    {
        std::cerr << "error parameters!\n";
        system("pause");
        exit(1); 
    }
    (xlen > MAX_XLEN) ? (this->xlen = xlen = MAX_XLEN) : (this->xlen = xlen); //���Ƴ���������׵����ֵ
    (ylen > MAX_YLEN) ? (this->ylen = ylen = MAX_YLEN) : (this->ylen = ylen);
    (mine >= xlen * ylen / MAX_MINE_RATE) ? (this->mine = mine = xlen * ylen / MAX_MINE_RATE) : (this->mine = mine);
    initgraph(2 * EDGE_SIZE + xlen * BLOCK_SIZE, UPFRAME_SIZE + EDGE_SIZE + ylen * BLOCK_SIZE, EW_DBLCLKS); //��ʼ������
    if (*(path.end()-1) != '/') //�ͼ���ļ���·������'/'����
        path.push_back('/');
    for (int i = 0; i < NUM_IMAGE; i++) //����·���е�����ͼƬ��ͼƬ˳������Ʊ���̶�Ϊi.jpg
    {
        this->path = path + std::to_string(i) + ".jpg";
        image[i] = new IMAGE;
        loadimage(image[i], this->path.c_str(), BLOCK_SIZE, BLOCK_SIZE, true);
    }
    this->path = path + "background.jpg"; //�����ֱ��ʲ�ͬ�赥������
    background = new IMAGE;
    loadimage(background, this->path.c_str(), 2 * EDGE_SIZE + xlen * BLOCK_SIZE, UPFRAME_SIZE + EDGE_SIZE + ylen * BLOCK_SIZE, true);
    map = new int* [xlen]; //�����ͼ�Ϳ��־�ڴ棬����ʼ��
    state = new int* [xlen];
    for (size_t i = 0; i < xlen; i++)
    {
        map[i] = new int[ylen];
        state[i] = new int[ylen];
        for (size_t j = 0; j < ylen; j++)
        {
            map[i][j] = 0;
            state[i][j] = STATE_ORIGIN;
        }
    }
}

Sweepmine::~Sweepmine()
{
    for (int i = 0; i < NUM_IMAGE; i++) //��������ͼƬ�ڴ�
    {
        delete image[i];
    }
    for (size_t i = 0; i < ylen; i++) //��������ͼ�Ϳ��־�ڴ�
    {
        delete[] map[i];
        delete[] state[i];
    }
    delete[] map;
    delete[] state;
    delete background;
    closegraph();
}

void Sweepmine::Play()
{
    while (true)
    {
        RandomMine();
        MakeMap();
        int r = GUI();
        if (r != 2) //���GUI��δ���Ц���ؿ�
        {
            if (r == 1) //ʤ�����棬ʧ����GUI()���Ѿ�ִ��
            {
                Win();
            }
            ExMessage mouse;
            do //���δ���Ц������ȴ�������ؿ�
            {
                getmessage(&mouse, EX_MOUSE);
            } while (!ClickSmile(mouse));
        }
        Reload();
    }
}

void Sweepmine::RandomMine()
{
    std::vector<int> arrmine(mine, -1); //��ʱ�������ɵ��ף��׵�λ����int��ʾ�Ҵ�0��ʼ�������ٷ���map��
    srand((unsigned)time(0));
    while (arrmine[arrmine.size() - 1] == -1) //ʹ������������׵�λ��
    {
        bool flag_same = 0;
        int i = 0;
        int r = rand() % (xlen * ylen);           
        while (arrmine[i] != -1) //�Ų��Ƿ�����ֵͬ
        {
            if (arrmine[i] == r)
            {
                flag_same = 1;
                break;
            }
            i++;
        }
        if (!flag_same) //ȷ��û����ֵͬ�Ÿ�ֵ
            arrmine[i] = r;
    }
    for (size_t i = 0; i < mine; i++) //���׷���map��
    {
        int x = arrmine[i] % xlen, y = arrmine[i] / xlen; //ע�ⶼʹ��xlen
        map[x][y] = 9;
    }
}

void Sweepmine::MakeMap()
{
    for (int i = 0; i < xlen; i++) //��ÿ��ը����Χ8���ֵ+1��ע��ͼ������Ȧ��Ҫ����Դ��������
    {
        for (int j = 0; j < ylen; j++)
        {
            if (map[i][j] == 9)
            {
                if (i > 0 && j > 0 && map[i - 1][j - 1] != 9) //���Ͽ�ʼ��˳ʱ����ת
                {
                    ++map[i - 1][j - 1];
                }
                if (j > 0 && map[i][j - 1] != 9)
                {
                    ++map[i][j - 1];
                }
                if (i < xlen - 1 && j > 0 && map[i + 1][j - 1] != 9)
                {
                    ++map[i + 1][j - 1];
                }
                if (i < xlen - 1 && map[i + 1][j] != 9)
                {
                    ++map[i + 1][j];
                }
                if (i < xlen - 1 && j < ylen - 1 && map[i + 1][j + 1] != 9)
                {
                    ++map[i + 1][j + 1];
                }
                if (j < ylen - 1 && map[i][j + 1] != 9)
                {
                    ++map[i][j + 1];
                }
                if (i > 0 && j < ylen - 1 && map[i - 1][j + 1] != 9)
                {
                    ++map[i - 1][j + 1];
                }
                if (i > 0 && map[i - 1][j] != 9)
                {
                    ++map[i - 1][j];
                }
            }
        }
    }
    putimage(0, 0, background); //����
    putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[15]); //Ц��
    for (int i = 0; i < xlen; i++) //��ʾδ�����ĵ�ͼ
    {
        for (int j = 0; j < ylen; j++)
        {
            ShowBlock(i, j, 11);
        }
    }
    int textwide = int(BLOCK_SIZE / 2);
    settextcolor(YELLOW); //����ǩ����ǩ�������ɫ�����壬��ʾλ��
    setbkmode(TRANSPARENT);
    settextstyle(BLOCK_SIZE, textwide, "����");
    outtextxy(EDGE_SIZE + BLOCK_SIZE, (UPFRAME_SIZE - BLOCK_SIZE) / 2, MY_SIGN);
    setlinestyle(PS_SOLID);
    setlinecolor(RED);
    rectangle(EDGE_SIZE + BLOCK_SIZE, (UPFRAME_SIZE - BLOCK_SIZE) / 2,
        EDGE_SIZE + BLOCK_SIZE + static_cast<int>(strlen(MY_SIGN)) * textwide, (UPFRAME_SIZE + BLOCK_SIZE) / 2);
    ///��ʾ������������ͼ����
    //for (int i = 0; i < xlen; i++) //��ʾȫ��������������ͼ
    //{
    //    for (int j = 0; j < ylen; j++)
    //    {
    //        switch (map[i][j])
    //        {
    //        case 0:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[0]);
    //            break;
    //        case 1:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[1]);
    //            break;
    //        case 2:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[2]);
    //            break;
    //        case 3:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[3]);
    //            break;
    //        case 4:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[4]);
    //            break;
    //        case 5:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[5]);
    //            break;
    //        case 6:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[6]);
    //            break;
    //        case 7:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[7]);
    //            break;
    //        case 8:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[8]);
    //            break;
    //        case 9:
    //            putimage(EDGE_SIZE + BLOCK_SIZE * i, UPFRAME_SIZE + BLOCK_SIZE * j, image[9]);
    //            break;
    //        }
    //    }
    //}
    //system("pause");
}

void Sweepmine::ShowBlock(int x, int y, int block)
{
    if (block == 0) //blockΪĬ��ֵ�򷭳���ͼ��
    {
        if (state[x][y] != STATE_ORIGIN) //����δ���������˳�
        {
            return;
        }
        switch (map[x][y]) //������Ӧ��
        {
        case 0:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[0]);
            state[x][y] = STATE_SPACE;
            break;
        case 1:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[1]);
            state[x][y] = STATE_NUM;
            break;
        case 2:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[2]);
            state[x][y] = STATE_NUM;
            break;
        case 3:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[3]);
            state[x][y] = STATE_NUM;
            break;
        case 4:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[4]);
            state[x][y] = STATE_NUM;
            break;
        case 5:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[5]);
            state[x][y] = STATE_NUM;
            break;
        case 6:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[6]);
            state[x][y] = STATE_NUM;
            break;
        case 7:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[7]);
            state[x][y] = STATE_NUM;
            break;
        case 8:
            putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[8]);
            state[x][y] = STATE_NUM;
            break;
            /*case 9:
                putimage(EDGE_SIZE + BLOCK_SIZE * x, EDGE_SIZE + BLOCK_SIZE * y, image[9]);
                break;*/
        }
        winnum++;
    }
    else //��������image[block]
    {
        putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[block]);
    }
}

int Sweepmine::GUI()
{
    ///����ƶ���ĳ��δ�����Ŀ�ʱ���˿��Ϊ������ʽ��δ�����飬ͻ����ʾ��������ڴ˿��ϣ�
    ///�����Ǽ�¼ǰһ�������Ŀ��λ�ã�������ߺ��仹ԭ��δ������
    int x_trigger = -1, y_trigger = -1;
    bool flag = 0; //��־λ
    ExMessage mouse = getmessage(EX_MOUSE);
    while (winnum < xlen * ylen - mine)
    {
        if (!mouse.lbutton && !mouse.rbutton) //û������ΪЦ��
        {
            putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[15]); //Ц��
        }
        if (!(mouse.lbutton && mouse.rbutton)) //������Ҽ�û��һ���²����»�ȡ�����Ϣ
        {
            mouse = getmessage(EX_MOUSE); //��ȡ�����Ϣ�����������±�x��y��ֵ
        }
        int x = (mouse.x - EDGE_SIZE) / BLOCK_SIZE, y = (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE;
        if (x >= 0 &&x < xlen && y >= 0 && y < ylen && mouse.x >= EDGE_SIZE && mouse.y >= UPFRAME_SIZE) //�ڽ���������
        {
            if (mouse.lbutton || mouse.rbutton) //�а������ΪŶ��
            {
                putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[16]); //Ŷ��
            }
            if (mouse.message == WM_LBUTTONDOWN && !mouse.rbutton) //����������Ҽ�δ����
            {
                while (mouse.message != WM_LBUTTONUP) //ֱ������ɿ�
                {
                    mouse = getmessage(EX_MOUSE);
                    if ((mouse.x - EDGE_SIZE) / BLOCK_SIZE != x || (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE != y || mouse.rbutton)
                    { //�������������ڿ��밴�����ʱ���ڿ鲻ͬ�������Ҽ����£������ñ�־��ʹ��������ѭ��
                        flag = true;
                        break;
                    }
                }
                if (flag)
                {
                    flag = false;
                    continue;
                }
                if (state[x][y] == STATE_ORIGIN) //δ�����Ŀ�
                {
                    if (map[x][y] == 9) //ը������Ϸ����
                    {
                        Over(x, y, 1);
                        return 0;
                    }
                    else //��ը���飬ֱ�ӷ������һ��չ����ͼ
                    {
                        Expand(x, y);
                    }
                }
                else if (state[x][y] == STATE_NUM) //�ѷ��������ֿ�
                    ;
                else if (state[x][y] == STATE_SPACE) //�ѷ����Ŀտ�
                    ;
                else if (state[x][y] == STATE_FLAG) //����Ŀ�
                    ;
                else //����else������̱warning
                    ;
            }
            else if (mouse.message == WM_RBUTTONDOWN && !mouse.lbutton) //�����Ҽ������δ����
            {
                while (mouse.rbutton) //ֱ���Ҽ��ɿ����˴��߼��������ͬ
                {
                    mouse = getmessage(EX_MOUSE);
                    if ((mouse.x - EDGE_SIZE) / BLOCK_SIZE != x || (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE != y || mouse.lbutton)
                    {
                        flag = true;
                        break;
                    }
                }
                if (flag)
                {
                    flag = false;
                    continue;
                }
                if (state[x][y] == STATE_ORIGIN) //δ�����Ŀ飬ֱ�Ӳ���
                {
                    ShowBlock(x, y, 12);
                    state[x][y] = STATE_FLAG;
                }
                else if (state[x][y] == STATE_NUM) //�ѷ��������ֿ�
                    ;
                else if (state[x][y] == STATE_SPACE) //�ѷ����Ŀտ�
                    ;
                else if (state[x][y] == STATE_FLAG) //����Ŀ飬ȡ������
                {
                    ShowBlock(x, y, 11);
                    state[x][y] = STATE_ORIGIN;
                }
            }
            else if (mouse.message == WM_LBUTTONDBLCLK || (mouse.lbutton && mouse.rbutton)) //���˫�������Ҽ�ͬʱ����
            {
                if (mouse.lbutton && mouse.rbutton) //���Ҽ�ͬʱ����ʱ9������Ҫ�д����鴦��
                {
                    if (state[x][y] == STATE_ORIGIN) //�м��δ�������Ϊ������
                    {
                        ShowBlock(x, y, 13);
                    }
                    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x - 1, y - 1, 13);
                    }
                    if (y > 0 && state[x][y - 1] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x, y - 1, 13);
                    }
                    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x + 1, y - 1, 13);
                    }
                    if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x + 1, y, 13);
                    }
                    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x + 1, y + 1, 13);
                    }
                    if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x, y + 1, 13);
                    }
                    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x - 1, y + 1, 13);
                    }
                    if (x > 0 && state[x - 1][y] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x - 1, y, 13);
                    }
                    while (mouse.message != WM_LBUTTONUP && mouse.message != WM_RBUTTONUP) //ֱ��������Ҽ��ɿ�
                    {
                        mouse = getmessage(EX_MOUSE);
                        if ((mouse.x - EDGE_SIZE) / BLOCK_SIZE != x || (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE != y)
                        { //�������������ڿ��밴�����ʱ���ڿ鲻ͬ�����ñ�־������ѭ�������ֻ�������Ҽ�ͬʱ���µĴ����У������ɿ�ʱ������ڿ�ɨ��
                            flag = true;
                            break;
                        }
                    }
                    //��ԭ֮ǰ�Ĵ����鲢����ѭ��
                    if (state[x][y] == STATE_ORIGIN) //�м�飬���֮ǰ�������軹ԭ
                    {
                        ShowBlock(x, y, 11);
                    }
                    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x - 1, y - 1, 11);
                    }
                    if (y > 0 && state[x][y - 1] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x, y - 1, 11);
                    }
                    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x + 1, y - 1, 11);
                    }
                    if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x + 1, y, 11);
                    }
                    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x + 1, y + 1, 11);
                    }
                    if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x, y + 1, 11);
                    }
                    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN) //����
                    {
                        ShowBlock(x - 1, y + 1, 11);
                    }
                    if (x > 0 && state[x - 1][y] == STATE_ORIGIN) //��
                    {
                        ShowBlock(x - 1, y, 11);
                    }
                    if (flag)
                    {
                        flag = false;
                        continue;
                    }
                }
                if (map[x][y] > 0 && map[x][y] < 9 && state[x][y] == STATE_NUM) //�����ֿ����ѷ�����������˿���Χ���׶����ų���򿪴˿����Χ�飬�������Ŵ������
                {
                    int flag_num = 0;
                    bool err_flag = false; //�������ı�־
                    //�鿴�����Ƿ��д�
                    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_FLAG) //���ϲ�����
                    {
                        flag_num++;
                        if (map[x - 1][y - 1] != 9) //�ص㣡ֻҪ���׳�����һ�����д������
                        {
                            err_flag = true;
                        }
                    }
                    if (y > 0 && state[x][y - 1] == STATE_FLAG) //��
                    {
                        flag_num++;
                        if (map[x][y - 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_FLAG) //����
                    {
                        flag_num++;
                        if (map[x + 1][y - 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x < xlen - 1 && state[x + 1][y] == STATE_FLAG) //��
                    {
                        flag_num++;
                        if (map[x + 1][y] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_FLAG) //����
                    {
                        flag_num++;
                        if (map[x + 1][y + 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (y < ylen - 1 && state[x][y + 1] == STATE_FLAG) //��
                    {
                        flag_num++;
                        if (map[x][y + 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_FLAG) //����
                    {
                        flag_num++;
                        if (map[x - 1][y + 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x > 0 && state[x - 1][y] == STATE_FLAG) //��
                    {
                        flag_num++;
                        if (map[x - 1][y] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (flag_num >= map[x][y] && err_flag) //���������㹻���д�����
                    {
                        Over(x, y, 2);
                        return 0;
                    }
                    else if (flag_num == map[x][y]) //û������Χ��δ������
                    {
                        if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN) //����
                        {
                            Expand(x - 1, y - 1);
                        }
                        if (y > 0 && state[x][y - 1] == STATE_ORIGIN) //��
                        {
                            Expand(x, y - 1);
                        }
                        if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN) //����
                        {
                            Expand(x + 1, y - 1);
                        }
                        if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN) //��
                        {
                            Expand(x + 1, y);
                        }
                        if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN) //����
                        {
                            Expand(x + 1, y + 1);
                        }
                        if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN) //��
                        {
                            Expand(x, y + 1);
                        }
                        if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN) //����
                        {
                            Expand(x - 1, y + 1);
                        }
                        if (x > 0 && state[x - 1][y] == STATE_ORIGIN) //��
                        {
                            Expand(x - 1, y);
                        }
                    }
                }
            }
            //������ڿ������Ҫ��һ����ɫ�Ŀ���ǿ�������������ϣ���֮Ϊ�����飬��������Ӧ����
            if ((x_trigger != x || y_trigger != y) && x_trigger != -1 &&
                state[x_trigger][y_trigger] == STATE_ORIGIN) //�������ƶ��������飬��Ҫ�ָ�ǰһ�εĴ�����
            {
                ShowBlock(x_trigger, y_trigger, 11);
            }
            if (state[x][y] == STATE_ORIGIN) //δ�����飬�Ȼָ�ǰһ�δ����飬�󴥷���ǰ�鲢��¼λ��
            {
                ShowBlock(x, y, 13);
                x_trigger = x, y_trigger = y;
            }
        }
        else if (ClickSmile(mouse))
        {
            return 2;
        }
    }
    return 1;
}

void Sweepmine::Expand(int x, int y)
{
    ShowBlock(x, y); //��ʾ��ǰ�飬ÿ������չ�鶼���ɵݹ���ô˺���������
    if (map[x][y] != 0) //���ֿ��򷵻أ��տ���Ҫ������չ��Χ8�飬�ǽӵĿտ�Ҳ����չ��
    {
        return;
    }
    if (x > 0 && state[x - 1][y] == STATE_ORIGIN && map[x - 1][y] != 9) //��໹�з��׿���δ����
    {
        Expand(x - 1, y);
    }
    if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN && map[x + 1][y] != 9) //�Ҳ�ͬ��
    {
        Expand(x + 1, y);
    }
    if (y > 0 && state[x][y - 1] == STATE_ORIGIN && map[x][y - 1] != 9) //��
    {
        Expand(x, y - 1);
    }
    if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN && map[x][y + 1] != 9) //��
    {
        Expand(x, y + 1);
    }
    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN && map[x - 1][y - 1] != 9) //����
    {
        Expand(x - 1, y - 1);
    }
    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN && map[x + 1][y - 1] != 9) //����
    {
        Expand(x + 1, y - 1);
    }
    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN && map[x - 1][y + 1] != 9) //����
    {
        Expand(x - 1, y + 1);
    }
    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN && map[x + 1][y + 1] != 9) //����
    {
        Expand(x + 1, y + 1);
    }
}

void Sweepmine::Over(int x, int y, int mode)
{
    for (int i = 0; i < xlen; i++) //δ���������Ҫ��ʾ���������������Ҫ��ʾ��������
    {
        for (int j = 0; j < ylen; j++)
        {
            if (map[i][j] == 9)
            {
                if (state[i][j] != STATE_FLAG)
                {
                    ShowBlock(i, j, 9);
                }
            }
            else if (state[i][j] == STATE_FLAG)
            {
                ShowBlock(i, j, 14);
            }
        }
    }
    if (mode == 1) //ֱ�Ӳȵ��׵Ľ���ģʽ����ʾ����
    {
        ShowBlock(x, y, 10);
    }
    else //ɨ�״���Ľ���ģʽ���ú�����ʾ��Χûɨ������
    {
        if (x > 0 && y > 0 && map[x - 1][y - 1] == 9 && state[x - 1][y - 1] == STATE_ORIGIN) //����
        {
            ShowBlock(x - 1, y - 1, 10);
        }
        if (y > 0 && map[x][y - 1] == 9 && state[x][y - 1] == STATE_ORIGIN) //��
        {
            ShowBlock(x, y - 1, 10);
        }
        if (x < xlen - 1 && y > 0 && map[x + 1][y - 1] == 9 && state[x + 1][y - 1] == STATE_ORIGIN) //����
        {
            ShowBlock(x + 1, y - 1, 10);
        }
        if (x < xlen - 1 && map[x + 1][y] == 9 && state[x + 1][y] == STATE_ORIGIN) //��
        {
            ShowBlock(x + 1, y, 10);
        }
        if (x < xlen - 1 && y < ylen - 1 && map[x + 1][y + 1] == 9 && state[x + 1][y + 1] == STATE_ORIGIN) //����
        {
            ShowBlock(x + 1, y + 1, 10);
        }
        if (y < ylen - 1 && map[x][y + 1] == 9 && state[x][y + 1] == STATE_ORIGIN) //��
        {
            ShowBlock(x, y + 1, 10);
        }
        if (x > 0 && y < ylen - 1 && map[x - 1][y + 1] == 9 && state[x - 1][y + 1] == STATE_ORIGIN) //����
        {
            ShowBlock(x - 1, y + 1, 10);
        }
        if (x > 0 && map[x - 1][y] == 9 && state[x - 1][y] == STATE_ORIGIN) //��
        {
            ShowBlock(x - 1, y, 10);
        }
    }
    settextcolor(LIGHTRED); //����ʧ�ܽ��������ɫ������ģʽ�����壬��ʾλ��
    setbkmode(TRANSPARENT);
    settextstyle(WORD_HEIGHT, WORD_WIDTH, "����");
    outtextxy((xlen * BLOCK_SIZE - WORD_WIDTH * (int)strlen(OVER_WORD)) / 2 + EDGE_SIZE,
        (UPFRAME_SIZE - WORD_HEIGHT) / 2, OVER_WORD);
    putimage(EDGE_SIZE + (xlen - 1) * BLOCK_SIZE / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[18]); //ʧ����
}

void Sweepmine::Win()
{
    for (int i = 0; i < xlen; i++) //�������ײ�������
    {
        for (int j = 0; j < ylen; j++)
        {
            if (map[i][j] == 9)
            {
                if (state[i][j] != STATE_FLAG)
                {
                    ShowBlock(i, j, 12);
                }
            }
        }
    }
    settextcolor(GREEN); //����ʤ�����������ɫ������ģʽ�����壬��ʾλ��
    setbkmode(TRANSPARENT);
    settextstyle(WORD_HEIGHT, WORD_WIDTH, "����");
    outtextxy((xlen * BLOCK_SIZE - WORD_WIDTH * (int)strlen(WIN_WORD)) / 2 + EDGE_SIZE,
        (UPFRAME_SIZE - WORD_HEIGHT) / 2, WIN_WORD);
    putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[17]); //ʤ����
}

void Sweepmine::Reload()
{ 
    //�������б���
    winnum = 0;
    for (size_t i = 0; i < xlen; i++)
    {
        for (size_t j = 0; j < ylen; j++)
        {
            map[i][j] = 0;
            state[i][j] = STATE_ORIGIN;
        }
    }
    flushmessage(); //�����껺��
}

bool Sweepmine::ClickSmile(ExMessage& mouse)
{
    if (mouse.x >= EDGE_SIZE + (xlen - 1) * BLOCK_SIZE / 2 && mouse.x <= EDGE_SIZE + (xlen + 1) * BLOCK_SIZE / 2
        && mouse.y >= (UPFRAME_SIZE - BLOCK_SIZE) / 2 && mouse.y <= (UPFRAME_SIZE + BLOCK_SIZE) / 2
        && mouse.message == WM_LBUTTONDOWN && !mouse.rbutton)
    { //�����Ц���鰴�����Ҽ�û��
        while (mouse.lbutton)
        {
            mouse = getmessage(EX_MOUSE);
            if (mouse.x < EDGE_SIZE + (xlen - 1) * BLOCK_SIZE / 2 || mouse.x > EDGE_SIZE + (xlen + 1) * BLOCK_SIZE / 2 || mouse.y < (UPFRAME_SIZE - BLOCK_SIZE) / 2 || mouse.y >(UPFRAME_SIZE + BLOCK_SIZE) / 2 || mouse.rbutton)
            { //��������Ҽ����������·���
                return false;
            }
        }
        return true; //�ɹ�������ؿ�
    }
    return false;
}