#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <graphics.h>
#include <time.h>
//#pragma comment(lib, "winmm.lib")

//控制台图形化扫雷游戏，有正常的扫雷和排雷功能，朴素的界面，可自定义矩阵大小和雷数
class Sweepmine
{
public:
	Sweepmine(int xlen = 20, int ylen = 15, int mine = 30, std::string path = "./image"); //一步到位
	~Sweepmine(); //清除后就走人
	void Play(); //总执行
private:
	static const int NUM_IMAGE = 19, MAX_XLEN = 35, MAX_YLEN = 20, //各种块图片的总数，长宽最大值
		BLOCK_SIZE = 35, EDGE_SIZE = 8, UPFRAME_SIZE = 60, MAX_MINE_RATE = 4; //每一块的大小，边缘宽度，块与雷数之比的最大值
	int xlen, ylen, mine; //长、宽、雷数
	int** map; //总地图，数字对应着相同数字的图片
	int** state; //每一块的状态标志，未翻出块0，翻出的数字块1，翻出的空块2，插旗块3
	enum { STATE_ORIGIN, STATE_NUM, STATE_SPACE, STATE_FLAG };
	std::string path; //图片读取路径
	IMAGE* image[NUM_IMAGE]; //需要的所有块图片
	IMAGE* background; //背景图
	int winnum = 0; //已翻出的块，等于总非雷块数时胜利
	static const char* WIN_WORD, * OVER_WORD; //胜利和失败后显示的文字（注意非静态int常量要在类定义文件中初始化而非头文件）
	static const char* MY_SIGN; //我的防伪签名
	static const int WORD_HEIGHT = 50, WORD_WIDTH = 32; //结束语的字体长宽
//随机生成雷并据此插入到地图矩阵中
	void RandomMine();
//根据雷计算所有块值并显示开局总地图
	void MakeMap();
//如果block不为默认值则在传入的坐标处翻出未翻出的基本块（0-8），否则在此位置贴上image[block]
	void ShowBlock(int x, int y, int block = 0);
//处理用户的各种鼠标操作，返回0为失败1为成功2为重开，及其复杂！（注意，贴图不需要整体刷新，而是需要更换的块才予以更换）
	int GUI();
//下面是功能模块函数，用在其它函数中
//左键点击数字块时直接翻出，空块时使用递归扩展显示相连空块及其周围8块
	void Expand(int x, int y);
//结束并显示雷
	void Over(int x, int y, int mode);
//胜利
	void Win();
//游戏结束后重新开始
	void Reload();
//检测是否点击笑脸以重开
	bool ClickSmile(ExMessage& mouse);
};
