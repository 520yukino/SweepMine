#include "h.h"
using std::string;
const char* Sweepmine::WIN_WORD = "YOU  WIN";
const char* Sweepmine::OVER_WORD = "GAME  OVER";
const char* Sweepmine::MY_SIGN = "虾好硬";

Sweepmine::Sweepmine(int xlen, int ylen, int mine, string path)
{
    if (xlen <= 1 || ylen <= 1 || mine <= 0 || path.empty() || xlen * ylen / MAX_MINE_RATE < 1) //参数错误
    {
        std::cerr << "error parameters!\n";
        system("pause");
        exit(1); 
    }
    (xlen > MAX_XLEN) ? (this->xlen = xlen = MAX_XLEN) : (this->xlen = xlen); //限制长宽参数和雷的最大值
    (ylen > MAX_YLEN) ? (this->ylen = ylen = MAX_YLEN) : (this->ylen = ylen);
    (mine >= xlen * ylen / MAX_MINE_RATE) ? (this->mine = mine = xlen * ylen / MAX_MINE_RATE) : (this->mine = mine);
    initgraph(2 * EDGE_SIZE + xlen * BLOCK_SIZE, UPFRAME_SIZE + EDGE_SIZE + ylen * BLOCK_SIZE, EW_DBLCLKS); //初始化窗口
    if (*(path.end()-1) != '/') //填补图形文件夹路径最后的'/'符号
        path.push_back('/');
    for (int i = 0; i < NUM_IMAGE; i++) //载入路径中的所需图片，图片顺序和名称必须固定为i.jpg
    {
        this->path = path + std::to_string(i) + ".jpg";
        image[i] = new IMAGE;
        loadimage(image[i], this->path.c_str(), BLOCK_SIZE, BLOCK_SIZE, true);
    }
    this->path = path + "background.jpg"; //背景分辨率不同需单独加载
    background = new IMAGE;
    loadimage(background, this->path.c_str(), 2 * EDGE_SIZE + xlen * BLOCK_SIZE, UPFRAME_SIZE + EDGE_SIZE + ylen * BLOCK_SIZE, true);
    map = new int* [xlen]; //分配地图和块标志内存，并初始化
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
    for (int i = 0; i < NUM_IMAGE; i++) //清除分配的图片内存
    {
        delete image[i];
    }
    for (size_t i = 0; i < ylen; i++) //清除分配地图和块标志内存
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
        if (r != 2) //如果GUI内未点击笑脸重开
        {
            if (r == 1) //胜利界面，失败在GUI()内已经执行
            {
                Win();
            }
            ExMessage mouse;
            do //如果未点击笑脸，则等待点击后重开
            {
                getmessage(&mouse, EX_MOUSE);
            } while (!ClickSmile(mouse));
        }
        Reload();
    }
}

void Sweepmine::RandomMine()
{
    std::vector<int> arrmine(mine, -1); //暂时储存生成的雷，雷的位置用int表示且从0开始，后面再放入map中
    srand((unsigned)time(0));
    while (arrmine[arrmine.size() - 1] == -1) //使用随机数生成雷的位置
    {
        bool flag_same = 0;
        int i = 0;
        int r = rand() % (xlen * ylen);           
        while (arrmine[i] != -1) //排查是否有相同值
        {
            if (arrmine[i] == r)
            {
                flag_same = 1;
                break;
            }
            i++;
        }
        if (!flag_same) //确保没有相同值才赋值
            arrmine[i] = r;
    }
    for (size_t i = 0; i < mine; i++) //将雷放入map中
    {
        int x = arrmine[i] % xlen, y = arrmine[i] / xlen; //注意都使用xlen
        map[x][y] = 9;
    }
}

void Sweepmine::MakeMap()
{
    for (int i = 0; i < xlen; i++) //给每个炸弹周围8块的值+1，注意图的最外圈需要特殊对待以免出界
    {
        for (int j = 0; j < ylen; j++)
        {
            if (map[i][j] == 9)
            {
                if (i > 0 && j > 0 && map[i - 1][j - 1] != 9) //左上开始，顺时针旋转
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
    putimage(0, 0, background); //背景
    putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[15]); //笑脸
    for (int i = 0; i < xlen; i++) //显示未翻开的地图
    {
        for (int j = 0; j < ylen; j++)
        {
            ShowBlock(i, j, 11);
        }
    }
    int textwide = int(BLOCK_SIZE / 2);
    settextcolor(YELLOW); //设置签名和签名框的颜色，字体，显示位置
    setbkmode(TRANSPARENT);
    settextstyle(BLOCK_SIZE, textwide, "宋体");
    outtextxy(EDGE_SIZE + BLOCK_SIZE, (UPFRAME_SIZE - BLOCK_SIZE) / 2, MY_SIGN);
    setlinestyle(PS_SOLID);
    setlinecolor(RED);
    rectangle(EDGE_SIZE + BLOCK_SIZE, (UPFRAME_SIZE - BLOCK_SIZE) / 2,
        EDGE_SIZE + BLOCK_SIZE + static_cast<int>(strlen(MY_SIGN)) * textwide, (UPFRAME_SIZE + BLOCK_SIZE) / 2);
    ///显示翻开的完整地图代码
    //for (int i = 0; i < xlen; i++) //显示全部翻开的完整地图
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
    if (block == 0) //block为默认值则翻出地图块
    {
        if (state[x][y] != STATE_ORIGIN) //不是未翻出块则退出
        {
            return;
        }
        switch (map[x][y]) //翻出对应块
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
    else //否则贴上image[block]
    {
        putimage(EDGE_SIZE + BLOCK_SIZE * x, UPFRAME_SIZE + BLOCK_SIZE * y, image[block]);
    }
}

int Sweepmine::GUI()
{
    ///鼠标移动到某块未翻出的块时将此块变为触发形式的未翻出块，突出表示现在鼠标在此块上，
    ///下面是记录前一个触发的块的位置，鼠标移走后将其还原成未翻出块
    int x_trigger = -1, y_trigger = -1;
    bool flag = 0; //标志位
    ExMessage mouse = getmessage(EX_MOUSE);
    while (winnum < xlen * ylen - mine)
    {
        if (!mouse.lbutton && !mouse.rbutton) //没按键则为笑脸
        {
            putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[15]); //笑脸
        }
        if (!(mouse.lbutton && mouse.rbutton)) //如果左右键没有一起按下才重新获取鼠标信息
        {
            mouse = getmessage(EX_MOUSE); //获取鼠标信息并处理数组下标x和y的值
        }
        int x = (mouse.x - EDGE_SIZE) / BLOCK_SIZE, y = (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE;
        if (x >= 0 &&x < xlen && y >= 0 && y < ylen && mouse.x >= EDGE_SIZE && mouse.y >= UPFRAME_SIZE) //在交互区域内
        {
            if (mouse.lbutton || mouse.rbutton) //有按鼠标则为哦脸
            {
                putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[16]); //哦脸
            }
            if (mouse.message == WM_LBUTTONDOWN && !mouse.rbutton) //按下左键且右键未按下
            {
                while (mouse.message != WM_LBUTTONUP) //直到左键松开
                {
                    mouse = getmessage(EX_MOUSE);
                    if ((mouse.x - EDGE_SIZE) / BLOCK_SIZE != x || (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE != y || mouse.rbutton)
                    { //如果发现鼠标所在块与按下左键时所在块不同，或者右键按下，则设置标志物使其重新总循环
                        flag = true;
                        break;
                    }
                }
                if (flag)
                {
                    flag = false;
                    continue;
                }
                if (state[x][y] == STATE_ORIGIN) //未翻出的块
                {
                    if (map[x][y] == 9) //炸弹，游戏结束
                    {
                        Over(x, y, 1);
                        return 0;
                    }
                    else //非炸弹块，直接翻开或进一步展开地图
                    {
                        Expand(x, y);
                    }
                }
                else if (state[x][y] == STATE_NUM) //已翻出的数字块
                    ;
                else if (state[x][y] == STATE_SPACE) //已翻出的空块
                    ;
                else if (state[x][y] == STATE_FLAG) //插旗的块
                    ;
                else //不加else会有脑瘫warning
                    ;
            }
            else if (mouse.message == WM_RBUTTONDOWN && !mouse.lbutton) //按下右键且左键未按下
            {
                while (mouse.rbutton) //直到右键松开，此处逻辑与左键相同
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
                if (state[x][y] == STATE_ORIGIN) //未翻出的块，直接插旗
                {
                    ShowBlock(x, y, 12);
                    state[x][y] = STATE_FLAG;
                }
                else if (state[x][y] == STATE_NUM) //已翻出的数字块
                    ;
                else if (state[x][y] == STATE_SPACE) //已翻出的空块
                    ;
                else if (state[x][y] == STATE_FLAG) //插旗的块，取消插旗
                {
                    ShowBlock(x, y, 11);
                    state[x][y] = STATE_ORIGIN;
                }
            }
            else if (mouse.message == WM_LBUTTONDBLCLK || (mouse.lbutton && mouse.rbutton)) //左键双击或左右键同时按下
            {
                if (mouse.lbutton && mouse.rbutton) //左右键同时按下时9宫格需要有触发块处理
                {
                    if (state[x][y] == STATE_ORIGIN) //中间块未翻出则变为触发块
                    {
                        ShowBlock(x, y, 13);
                    }
                    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN) //左上
                    {
                        ShowBlock(x - 1, y - 1, 13);
                    }
                    if (y > 0 && state[x][y - 1] == STATE_ORIGIN) //上
                    {
                        ShowBlock(x, y - 1, 13);
                    }
                    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN) //右上
                    {
                        ShowBlock(x + 1, y - 1, 13);
                    }
                    if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN) //右
                    {
                        ShowBlock(x + 1, y, 13);
                    }
                    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN) //右下
                    {
                        ShowBlock(x + 1, y + 1, 13);
                    }
                    if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN) //下
                    {
                        ShowBlock(x, y + 1, 13);
                    }
                    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN) //左下
                    {
                        ShowBlock(x - 1, y + 1, 13);
                    }
                    if (x > 0 && state[x - 1][y] == STATE_ORIGIN) //左
                    {
                        ShowBlock(x - 1, y, 13);
                    }
                    while (mouse.message != WM_LBUTTONUP && mouse.message != WM_RBUTTONUP) //直到左键或右键松开
                    {
                        mouse = getmessage(EX_MOUSE);
                        if ((mouse.x - EDGE_SIZE) / BLOCK_SIZE != x || (mouse.y - UPFRAME_SIZE) / BLOCK_SIZE != y)
                        { //如果发现鼠标所在块与按下左键时所在块不同就设置标志物重新循环，但又会进入左右键同时按下的代码中，所以松开时会对所在块扫雷
                            flag = true;
                            break;
                        }
                    }
                    //还原之前的触发块并重新循环
                    if (state[x][y] == STATE_ORIGIN) //中间块，如果之前触发则需还原
                    {
                        ShowBlock(x, y, 11);
                    }
                    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN) //左上
                    {
                        ShowBlock(x - 1, y - 1, 11);
                    }
                    if (y > 0 && state[x][y - 1] == STATE_ORIGIN) //上
                    {
                        ShowBlock(x, y - 1, 11);
                    }
                    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN) //右上
                    {
                        ShowBlock(x + 1, y - 1, 11);
                    }
                    if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN) //右
                    {
                        ShowBlock(x + 1, y, 11);
                    }
                    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN) //右下
                    {
                        ShowBlock(x + 1, y + 1, 11);
                    }
                    if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN) //下
                    {
                        ShowBlock(x, y + 1, 11);
                    }
                    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN) //左下
                    {
                        ShowBlock(x - 1, y + 1, 11);
                    }
                    if (x > 0 && state[x - 1][y] == STATE_ORIGIN) //左
                    {
                        ShowBlock(x - 1, y, 11);
                    }
                    if (flag)
                    {
                        flag = false;
                        continue;
                    }
                }
                if (map[x][y] > 0 && map[x][y] < 9 && state[x][y] == STATE_NUM) //是数字块且已翻出，则如果此块周围的雷都被排出则打开此块的周围块，排满且排错则结束
                {
                    int flag_num = 0;
                    bool err_flag = false; //错误插旗的标志
                    //查看排雷是否有错
                    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_FLAG) //左上插了旗
                    {
                        flag_num++;
                        if (map[x - 1][y - 1] != 9) //重点！只要排雷出错则一定会有错误插旗
                        {
                            err_flag = true;
                        }
                    }
                    if (y > 0 && state[x][y - 1] == STATE_FLAG) //上
                    {
                        flag_num++;
                        if (map[x][y - 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_FLAG) //右上
                    {
                        flag_num++;
                        if (map[x + 1][y - 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x < xlen - 1 && state[x + 1][y] == STATE_FLAG) //右
                    {
                        flag_num++;
                        if (map[x + 1][y] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_FLAG) //右下
                    {
                        flag_num++;
                        if (map[x + 1][y + 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (y < ylen - 1 && state[x][y + 1] == STATE_FLAG) //下
                    {
                        flag_num++;
                        if (map[x][y + 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_FLAG) //左下
                    {
                        flag_num++;
                        if (map[x - 1][y + 1] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (x > 0 && state[x - 1][y] == STATE_FLAG) //左
                    {
                        flag_num++;
                        if (map[x - 1][y] != 9)
                        {
                            err_flag = true;
                        }
                    }
                    if (flag_num >= map[x][y] && err_flag) //插旗数量足够且有错，结束
                    {
                        Over(x, y, 2);
                        return 0;
                    }
                    else if (flag_num == map[x][y]) //没错，打开周围的未翻出块
                    {
                        if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN) //左上
                        {
                            Expand(x - 1, y - 1);
                        }
                        if (y > 0 && state[x][y - 1] == STATE_ORIGIN) //上
                        {
                            Expand(x, y - 1);
                        }
                        if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN) //右上
                        {
                            Expand(x + 1, y - 1);
                        }
                        if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN) //右
                        {
                            Expand(x + 1, y);
                        }
                        if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN) //右下
                        {
                            Expand(x + 1, y + 1);
                        }
                        if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN) //下
                        {
                            Expand(x, y + 1);
                        }
                        if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN) //左下
                        {
                            Expand(x - 1, y + 1);
                        }
                        if (x > 0 && state[x - 1][y] == STATE_ORIGIN) //左
                        {
                            Expand(x - 1, y);
                        }
                    }
                }
            }
            //鼠标所在块可能需要换一种深色的块来强调鼠标在这个块上，称之为触发块，下面是响应代码
            if ((x_trigger != x || y_trigger != y) && x_trigger != -1 &&
                state[x_trigger][y_trigger] == STATE_ORIGIN) //如果鼠标移动到其它块，需要恢复前一次的触发块
            {
                ShowBlock(x_trigger, y_trigger, 11);
            }
            if (state[x][y] == STATE_ORIGIN) //未翻出块，先恢复前一次触发块，后触发当前块并记录位置
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
    ShowBlock(x, y); //显示当前块，每个可扩展块都会由递归调用此函数来翻出
    if (map[x][y] != 0) //数字块则返回，空块需要继续扩展周围8块，角接的空快也会扩展开
    {
        return;
    }
    if (x > 0 && state[x - 1][y] == STATE_ORIGIN && map[x - 1][y] != 9) //左侧还有非雷块且未翻出
    {
        Expand(x - 1, y);
    }
    if (x < xlen - 1 && state[x + 1][y] == STATE_ORIGIN && map[x + 1][y] != 9) //右侧同理
    {
        Expand(x + 1, y);
    }
    if (y > 0 && state[x][y - 1] == STATE_ORIGIN && map[x][y - 1] != 9) //上
    {
        Expand(x, y - 1);
    }
    if (y < ylen - 1 && state[x][y + 1] == STATE_ORIGIN && map[x][y + 1] != 9) //下
    {
        Expand(x, y + 1);
    }
    if (x > 0 && y > 0 && state[x - 1][y - 1] == STATE_ORIGIN && map[x - 1][y - 1] != 9) //左上
    {
        Expand(x - 1, y - 1);
    }
    if (x < xlen - 1 && y > 0 && state[x + 1][y - 1] == STATE_ORIGIN && map[x + 1][y - 1] != 9) //右上
    {
        Expand(x + 1, y - 1);
    }
    if (x > 0 && y < ylen - 1 && state[x - 1][y + 1] == STATE_ORIGIN && map[x - 1][y + 1] != 9) //左下
    {
        Expand(x - 1, y + 1);
    }
    if (x < xlen - 1 && y < ylen - 1 && state[x + 1][y + 1] == STATE_ORIGIN && map[x + 1][y + 1] != 9) //右下
    {
        Expand(x + 1, y + 1);
    }
}

void Sweepmine::Over(int x, int y, int mode)
{
    for (int i = 0; i < xlen; i++) //未插旗的雷需要显示出来，插旗插错的需要显示错误旗帜
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
    if (mode == 1) //直接踩地雷的结束模式，显示红雷
    {
        ShowBlock(x, y, 10);
    }
    else //扫雷错误的结束模式，用红雷显示周围没扫出的雷
    {
        if (x > 0 && y > 0 && map[x - 1][y - 1] == 9 && state[x - 1][y - 1] == STATE_ORIGIN) //左上
        {
            ShowBlock(x - 1, y - 1, 10);
        }
        if (y > 0 && map[x][y - 1] == 9 && state[x][y - 1] == STATE_ORIGIN) //上
        {
            ShowBlock(x, y - 1, 10);
        }
        if (x < xlen - 1 && y > 0 && map[x + 1][y - 1] == 9 && state[x + 1][y - 1] == STATE_ORIGIN) //右上
        {
            ShowBlock(x + 1, y - 1, 10);
        }
        if (x < xlen - 1 && map[x + 1][y] == 9 && state[x + 1][y] == STATE_ORIGIN) //右
        {
            ShowBlock(x + 1, y, 10);
        }
        if (x < xlen - 1 && y < ylen - 1 && map[x + 1][y + 1] == 9 && state[x + 1][y + 1] == STATE_ORIGIN) //右下
        {
            ShowBlock(x + 1, y + 1, 10);
        }
        if (y < ylen - 1 && map[x][y + 1] == 9 && state[x][y + 1] == STATE_ORIGIN) //下
        {
            ShowBlock(x, y + 1, 10);
        }
        if (x > 0 && y < ylen - 1 && map[x - 1][y + 1] == 9 && state[x - 1][y + 1] == STATE_ORIGIN) //左下
        {
            ShowBlock(x - 1, y + 1, 10);
        }
        if (x > 0 && map[x - 1][y] == 9 && state[x - 1][y] == STATE_ORIGIN) //左
        {
            ShowBlock(x - 1, y, 10);
        }
    }
    settextcolor(LIGHTRED); //设置失败结束语的颜色，背景模式，字体，显示位置
    setbkmode(TRANSPARENT);
    settextstyle(WORD_HEIGHT, WORD_WIDTH, "黑体");
    outtextxy((xlen * BLOCK_SIZE - WORD_WIDTH * (int)strlen(OVER_WORD)) / 2 + EDGE_SIZE,
        (UPFRAME_SIZE - WORD_HEIGHT) / 2, OVER_WORD);
    putimage(EDGE_SIZE + (xlen - 1) * BLOCK_SIZE / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[18]); //失败脸
}

void Sweepmine::Win()
{
    for (int i = 0; i < xlen; i++) //将所有雷插上旗帜
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
    settextcolor(GREEN); //设置胜利结束语的颜色，背景模式，字体，显示位置
    setbkmode(TRANSPARENT);
    settextstyle(WORD_HEIGHT, WORD_WIDTH, "黑体");
    outtextxy((xlen * BLOCK_SIZE - WORD_WIDTH * (int)strlen(WIN_WORD)) / 2 + EDGE_SIZE,
        (UPFRAME_SIZE - WORD_HEIGHT) / 2, WIN_WORD);
    putimage(EDGE_SIZE + ((xlen - 1) * BLOCK_SIZE) / 2, (UPFRAME_SIZE - BLOCK_SIZE) / 2, image[17]); //胜利脸
}

void Sweepmine::Reload()
{ 
    //重载所有变量
    winnum = 0;
    for (size_t i = 0; i < xlen; i++)
    {
        for (size_t j = 0; j < ylen; j++)
        {
            map[i][j] = 0;
            state[i][j] = STATE_ORIGIN;
        }
    }
    flushmessage(); //清空鼠标缓存
}

bool Sweepmine::ClickSmile(ExMessage& mouse)
{
    if (mouse.x >= EDGE_SIZE + (xlen - 1) * BLOCK_SIZE / 2 && mouse.x <= EDGE_SIZE + (xlen + 1) * BLOCK_SIZE / 2
        && mouse.y >= (UPFRAME_SIZE - BLOCK_SIZE) / 2 && mouse.y <= (UPFRAME_SIZE + BLOCK_SIZE) / 2
        && mouse.message == WM_LBUTTONDOWN && !mouse.rbutton)
    { //左键在笑脸块按下且右键没按
        while (mouse.lbutton)
        {
            mouse = getmessage(EX_MOUSE);
            if (mouse.x < EDGE_SIZE + (xlen - 1) * BLOCK_SIZE / 2 || mouse.x > EDGE_SIZE + (xlen + 1) * BLOCK_SIZE / 2 || mouse.y < (UPFRAME_SIZE - BLOCK_SIZE) / 2 || mouse.y >(UPFRAME_SIZE + BLOCK_SIZE) / 2 || mouse.rbutton)
            { //鼠标出界或右键按下则无事发生
                return false;
            }
        }
        return true; //成功点击则重开
    }
    return false;
}