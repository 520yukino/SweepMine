#include "h.h"
int main()
{
    ///对图像加载速度和重复时的处理情况的实验代码
    //initgraph(1000, 500);
    //IMAGE im;
    //loadimage(&im, "D:/Visual Studio/VSPro/娱乐程序/扫雷_命令行/image/14.jpg");
    //int i = 0;
    //while (true)
    //{
    //    putimage(i++, i, &im);
    //    Sleep(10);
    //}
    Sweepmine* game = new Sweepmine(15, 10, 20, "D:/Visual Studio/VSPro/娱乐程序/扫雷/image");
    game->Play();
    delete game;
}
