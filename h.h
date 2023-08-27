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

//����̨ͼ�λ�ɨ����Ϸ����������ɨ�׺����׹��ܣ����صĽ��棬���Զ�������С������
class Sweepmine
{
public:
	Sweepmine(int xlen = 20, int ylen = 15, int mine = 30, std::string path = "./image"); //һ����λ
	~Sweepmine(); //����������
	void Play(); //��ִ��
private:
	static const int NUM_IMAGE = 19, MAX_XLEN = 35, MAX_YLEN = 20, //���ֿ�ͼƬ���������������ֵ
		BLOCK_SIZE = 35, EDGE_SIZE = 8, UPFRAME_SIZE = 60, MAX_MINE_RATE = 4; //ÿһ��Ĵ�С����Ե��ȣ���������֮�ȵ����ֵ
	int xlen, ylen, mine; //����������
	int** map; //�ܵ�ͼ�����ֶ�Ӧ����ͬ���ֵ�ͼƬ
	int** state; //ÿһ���״̬��־��δ������0�����������ֿ�1�������Ŀտ�2�������3
	enum { STATE_ORIGIN, STATE_NUM, STATE_SPACE, STATE_FLAG };
	std::string path; //ͼƬ��ȡ·��
	IMAGE* image[NUM_IMAGE]; //��Ҫ�����п�ͼƬ
	IMAGE* background; //����ͼ
	int winnum = 0; //�ѷ����Ŀ飬�����ܷ��׿���ʱʤ��
	static const char* WIN_WORD, * OVER_WORD; //ʤ����ʧ�ܺ���ʾ�����֣�ע��Ǿ�̬int����Ҫ���ඨ���ļ��г�ʼ������ͷ�ļ���
	static const char* MY_SIGN; //�ҵķ�αǩ��
	static const int WORD_HEIGHT = 50, WORD_WIDTH = 32; //����������峤��
//��������ײ��ݴ˲��뵽��ͼ������
	void RandomMine();
//�����׼������п�ֵ����ʾ�����ܵ�ͼ
	void MakeMap();
//���block��ΪĬ��ֵ���ڴ�������괦����δ�����Ļ����飨0-8���������ڴ�λ������image[block]
	void ShowBlock(int x, int y, int block = 0);
//�����û��ĸ���������������0Ϊʧ��1Ϊ�ɹ�2Ϊ�ؿ������临�ӣ���ע�⣬��ͼ����Ҫ����ˢ�£�������Ҫ�����Ŀ�����Ը�����
	int GUI();
//�����ǹ���ģ�麯������������������
//���������ֿ�ʱֱ�ӷ������տ�ʱʹ�õݹ���չ��ʾ�����տ鼰����Χ8��
	void Expand(int x, int y);
//��������ʾ��
	void Over(int x, int y, int mode);
//ʤ��
	void Win();
//��Ϸ���������¿�ʼ
	void Reload();
//����Ƿ���Ц�����ؿ�
	bool ClickSmile(ExMessage& mouse);
};
