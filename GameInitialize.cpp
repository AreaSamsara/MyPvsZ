# include "GameInitialize.h"

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

extern struct Plant plants[PlantNum];
extern struct PlantinBlock map[RowofMap][ColumnofMap];
extern struct SunshineBall sunshine[10];
extern struct Zombie zombies[10];
extern struct PeaBullet peabullets[30];

//��ʼ����Ϸ�����������
void WindowInitialize()
{
	//������Ϸ����ͼƬ
	loadimage(&imgBackground, "res/res/Map/map0.jpg");

	//���ع�����ͼƬ
	loadimage(&imgBar, "res/res/bar5.png");

	//������Ϸ��ͼ�δ���
	initgraph(WIN_WIDTH, WIN_HEIGHT);	//��Ԥ�����Դ���
}

//��ʼ��ȫ�ֲ�������Ҫ�ȳ�ʼ����Ϸ����
void GlobalParameterInitialize()
{
	//���ַ����޸�Ϊ�����ֽ��ַ��������ڱ��뻷����ִ�д˲���

	//��ʼ����Ϸ����
	killcount = 0;			//�Ѿ���ɱ�Ľ�ʬ������ʼ��Ϊ0
	zombieCreated = 0;		//�Ѿ����ɵĽ�ʬ������ʼ��Ϊ0
	gameStatus = RUNNING;		//��ǰ��Ϸ����״̬��ʼ��ΪRunning

	//�������������
	srand(time(NULL));

	//��������
	LOGFONT font;

	gettextstyle(&font);
	font.lfHeight = 30;
	font.lfWeight = 15;
	strcpy(font.lfFaceName, "Segoe UI Black");
	font.lfQuality = ANTIALIASED_QUALITY;	//��������Ŀ����Ч��
	settextstyle(&font);

	setbkmode(TRANSPARENT);		//�������屳��Ϊ͸��
	setcolor(BLACK);			//��������Ϊ��ɫ
}

//��ʼ����ʬ�������
void ZombieInitialize()
{
	char filename[64];		//��ʱ�ļ�������ʾ���ص�ͼƬ�ļ������Ȳ�����64

	//��ʼ����ʬ���н�ʬ������
	memset(zombies, 0, sizeof(zombies));
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm/%d.png", i + 1);
		loadimage(&imgZombie[i], filename);
	}

	//��ʼ����ʬ����ͼƬ
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm_dead/%d.png", i + 1);
		loadimage(&imgZombieDead[i], filename);
	}

	//��ʼ����ʬ��ֲ��ͼƬ
	for (int i = 0; i < 21; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm_eat/%d.png", i + 1);
		loadimage(&imgZombieEat[i], filename);
	}

	//��ʼ����ʬվ��ͼƬ
	for (int i = 0; i < 11; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm_stand/%d.png", i + 1);
		loadimage(&imgZombieStand[i],filename);
	}
}

//��ʼ��ֲ���������
void PlantInitialize()
{
	char filename[64];		//��ʱ�ļ�������ʾ���ص�ͼƬ�ļ������Ȳ�����64

	memset(imgPlant, 0, sizeof(imgPlant));	//���ֲ��ͼƬ

	for (int i = 0; i < PlantNum; i++)
	{
		//��ʼ��ֲ�﹤����ͼƬ
		sprintf_s(filename, sizeof(filename), "res//res//Cards//card_%d.png", i + 1);
		loadimage(&imgCards[i], filename);

		//��ʼ��ֲ��ͼƬ
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(filename, sizeof(filename), "res//res//zhiwu//%d//%d.png", i, j + 1);

			if (fileExist(filename))	//���ж�����ļ��Ƿ����
			{
				imgPlant[i][j] = new IMAGE;
				loadimage(imgPlant[i][j], filename);
			}
			else break;
		}

		//��ʼ��ֲ���������
		plants[i].kind = i;
	}

	//��ʼ��ֲ����в���
	plants[Peashooter].cost = 100;
	plants[Sunflower].cost = 50;
	plants[Peashooter].HP = 50;
	plants[Sunflower].HP = 50;

	//��ʼ����ƺ�ϵ�ֲ��
	memset(map, 0, sizeof(map));
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			map[i][j].type = -1;	//-1��ʾû��ֲ��
		}
	}

	//��ʼ����ǰֲ��״̬
	curPlant = -1;		
}

//��ʼ������ʵ���������
void EntityInitialize()
{
	char filename[64];		//��ʱ�ļ�������ʾ���ص�ͼƬ�ļ������Ȳ�����64

	//��ʼ�����������
	memset(sunshine, 0, sizeof(sunshine));
	for (int i = 0; i < 29; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], filename);
	}

	//��ʼ���㶹�ӵ�����
	memset(peabullets, 0, sizeof(peabullets));
	loadimage(&imgPeaBulletNormal, "res/res/bullets/PeaNormal/PeaNormal_0.png");
	loadimage(&imgPeaBulletBlast[3], "res/res/bullets/PeaNormalExplode/PeaNormalExplode_0.png");;
	for (int i = 0; i < 3; i++)		//ʵ�ֱ�ը�����𽥷Ŵ��Ч��
	{
		float k = (i + 1) * 0.2;
		loadimage(&imgPeaBulletBlast[i], "res/res/bullets/PeaNormalExplode/PeaNormalExplode_0.png",
			imgPeaBulletBlast[3].getwidth() * k,
			imgPeaBulletBlast[3].getheight() * k, true);
	}
}


