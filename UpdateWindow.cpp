#include "UpdateWindow.h"

extern enum { Peashooter, Sunflower, PlantNum };	//��ʾ��ǰֲ�������ö������
extern enum { Sunshine_Down, Sunshine_Ground, Sunshine_Collected, Sunshine_Product };	//��ʾ������״̬��ö������
extern enum { RUNNING, WIN, FAIL };		//��ʾ��ǰ��Ϸ����״̬��ö�ٱ���

extern IMAGE imgBackground;	//����ͼƬ
extern IMAGE imgBar;	//ֲ�﹤����ͼƬ 
extern IMAGE imgCards[PlantNum];	//ֲ�￨��
extern IMAGE* imgPlant[PlantNum][20];	//ֲ��ͼƬ��ÿ��ֲ��ͼƬ����������20
extern IMAGE imgSunshineBall[29];		//������ͼƬ����29��
extern IMAGE imgZombie[22];			//��ʬͼƬ����22��
extern IMAGE imgZombieDead[20];		//��ʬ����ͼƬ����20��
extern IMAGE imgZombieEat[21];			//��ʬ��ֲ��ͼƬ����21��
extern IMAGE imgZombieStand[11];		//��ʬվ��ͼƬ����11��
extern IMAGE imgPeaBulletNormal;		//�㶹�ӵ�����״̬�µ�ͼƬ
extern IMAGE imgPeaBulletBlast[4];		//�㶹�ӵ���ը״̬��ͼƬ����4��

extern int curX, curY;		//��ǰѡ�е�ֲ���ڱ��϶������е�����
extern int curPlant;		//-1����ʾδѡ�У�n����ʾѡ����ö�������еĵ�n��ֲ��
extern int SunshineAmount;	//��ʼ��������
extern int killcount;		//�Ѿ���ɱ�Ľ�ʬ����
extern int zombieCreated;	//һ����Ϸ����ֵĽ�ʬ����
extern int gameStatus;		//��ǰ��Ϸ����״̬

extern bool sunshineShortage;		//��ʾ�Ƿ������ⲻ��ʱ��ͼѡ��ֲ��

extern struct Plant plants[PlantNum];
extern struct PlantinBlock map[RowofMap][ColumnofMap];
extern struct SunshineBall sunshine[10];
extern struct Zombie zombies[10];
extern struct PeaBullet peabullets[30];


//��Ⱦֲ�﹤����
void DrawPlantBar()
{
	for (int i = 0; i < PlantNum; i++)
	{
		int x = 338-112 + i * 65;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

//��Ⱦÿһ���ƺ�Ϸ���ֲ��
void DrawPlantinBlock()
{
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type > -1)
			{
				int TypeofPlant = map[i][j].type;
				int FrameIndexofPlant = map[i][j].frameIndex;
				putimagePNG(map[i][j].x, map[i][j].y, imgPlant[TypeofPlant][FrameIndexofPlant]);
			}
		}
	}
}

//��Ⱦ�϶������б��϶���ֲ��
void DrawPlantDragged()
{
	if (curPlant > -1)	//���ѡ����ֲ��
	{
		IMAGE* img = imgPlant[curPlant][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);	//ʹ֧��͸����ֲ��ͼƬ
	}
}

//��Ⱦ���������������
void DrawSunshine()
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//����������������Ŀ
	//��Ⱦת�������е�������
	for (int i = 0; i < SunshineNumsinPool; i++)
	{
		if (sunshine[i].used || sunshine[i].vector_x)
		{
			IMAGE* img = &imgSunshineBall[sunshine[i].frameIndex];
			putimagePNG(sunshine[i].pcur.x, sunshine[i].pcur.y, img);
		}
	}

	//��Ⱦ��������
	char SunshineamountText[8];
	sprintf_s(SunshineamountText, sizeof(SunshineamountText), "%d", SunshineAmount);
	outtextxy(276-112, 67, SunshineamountText);		//��ӡ���������ı�
}

//��Ⱦ��ʬ
void DrawZombie()
{
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//��ʬ�������ʬ��Ŀ
	for (int i = 0; i < ZombieNumsinPool; i++)
	{
		if (zombies[i].used)
		{
			IMAGE* img = NULL;		//���ݽ�ʬ��״̬ѡ��ͼƬ����
			if (zombies[i].dead)
				img = imgZombieDead;
			else if (zombies[i].eating)
				img = imgZombieEat;
			else
				img = imgZombie;

			img += zombies[i].frameIndex;
			putimagePNG(zombies[i].x, zombies[i].y - img->getheight(), img);
		}
	}
}

//��Ⱦ�㶹�ӵ�
void DrawPeabullet()
{
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//�㶹�ӵ���������㶹�ӵ���Ŀ
	for (int i = 0; i < PeaBulletNumsinPool; i++)
	{
		if (peabullets[i].used == false)	continue;	//����δ��ʹ�õ��㶹�ӵ�
		if (peabullets[i].blast)	//����ӵ����ڷ�����ը
		{
			IMAGE* img = &imgPeaBulletBlast[peabullets[i].frameIndex];
			putimagePNG(peabullets[i].x, peabullets[i].y, img);
		}
		else	//����ӵ�δ������ը
		{
			putimagePNG(peabullets[i].x, peabullets[i].y, &imgPeaBulletNormal);
		}
	}
}

//��Ⱦ���ⲻ�����˸Ч��
void DrawSunshineShortage()
{
	if (!sunshineShortage)			//���δ�����ⲻ��ʱ��ͼѡ��ֲ�����Ⱦ
	{
		setcolor(BLACK);			//��������Ϊ��ɫ
		return;
	}
	
	//ʵ�ֺ�ڽ����Ч��
	setcolor(RED);				//��������Ϊ��ɫ

	static int flashTimer = 0;
	flashTimer++;
	if (flashTimer < 10)	//������˸��Ƶ��
		return;

	sunshineShortage = false;		//�������ⲻ��״̬
	flashTimer = 0;					//������˸��ʱ��
}










