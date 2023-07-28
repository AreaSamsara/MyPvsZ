# include "GameInitialize.h"

extern enum { Peashooter, Sunflower, PlantNum };	//表示当前植物种类的枚举类型
extern enum { Sunshine_Down, Sunshine_Ground, Sunshine_Collected, Sunshine_Product };	//表示阳光球状态的枚举类型
extern enum { RUNNING, WIN, FAIL };		//表示当前游戏运行状态的枚举变量

extern IMAGE imgBackground;	//背景图片
extern IMAGE imgBar;	//植物工具栏图片 
extern IMAGE imgCards[PlantNum];	//植物卡牌
extern IMAGE* imgPlant[PlantNum][20];	//植物图片，每个植物图片数量不超过20
extern IMAGE imgSunshineBall[29];		//阳光球图片，共29张
extern IMAGE imgZombie[22];			//僵尸图片，共22张
extern IMAGE imgZombieDead[20];		//僵尸死亡图片，共20张
extern IMAGE imgZombieEat[21];			//僵尸吃植物图片，共21张
extern IMAGE imgZombieStand[11];		//僵尸站立图片，共11张
extern IMAGE imgPeaBulletNormal;		//豌豆子弹正常状态下的图片
extern IMAGE imgPeaBulletBlast[4];		//豌豆子弹爆炸状态的图片，共4张

extern int curX, curY;		//当前选中的植物在被拖动过程中的坐标
extern int curPlant;		//-1：表示未选中；n：表示选择了枚举类型中的第n种植物
extern int SunshineAmount;	//初始阳光总量
extern int killcount;		//已经击杀的僵尸数量
extern int zombieCreated;	//一局游戏会出现的僵尸总数
extern int gameStatus;		//当前游戏运行状态

extern struct Plant plants[PlantNum];
extern struct PlantinBlock map[RowofMap][ColumnofMap];
extern struct SunshineBall sunshine[10];
extern struct Zombie zombies[10];
extern struct PeaBullet peabullets[30];

//初始化游戏窗口相关数据
void WindowInitialize()
{
	//加载游戏背景图片
	loadimage(&imgBackground, "res/res/Map/map0.jpg");

	//加载工具栏图片
	loadimage(&imgBar, "res/res/bar5.png");

	//创建游戏的图形窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);	//不预留调试窗口
}

//初始化全局参数，需要先初始化游戏窗口
void GlobalParameterInitialize()
{
	//把字符集修改为“多字节字符集”，在编译环境中执行此操作

	//初始化游戏数据
	killcount = 0;			//已经击杀的僵尸数量初始化为0
	zombieCreated = 0;		//已经生成的僵尸总数初始化为0
	gameStatus = RUNNING;		//当前游戏运行状态初始化为Running

	//配置随机数种子
	srand(time(NULL));

	//设置字体
	LOGFONT font;

	gettextstyle(&font);
	font.lfHeight = 30;
	font.lfWeight = 15;
	strcpy(font.lfFaceName, "Segoe UI Black");
	font.lfQuality = ANTIALIASED_QUALITY;	//设置字体的抗锯齿效果
	settextstyle(&font);

	setbkmode(TRANSPARENT);		//设置字体背景为透明
	setcolor(BLACK);			//设置字体为黑色
}

//初始化僵尸相关数据
void ZombieInitialize()
{
	char filename[64];		//临时文件名，表示加载的图片文件名长度不超过64

	//初始化僵尸池中僵尸的数据
	memset(zombies, 0, sizeof(zombies));
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm/%d.png", i + 1);
		loadimage(&imgZombie[i], filename);
	}

	//初始化僵尸死亡图片
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm_dead/%d.png", i + 1);
		loadimage(&imgZombieDead[i], filename);
	}

	//初始化僵尸吃植物图片
	for (int i = 0; i < 21; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm_eat/%d.png", i + 1);
		loadimage(&imgZombieEat[i], filename);
	}

	//初始化僵尸站立图片
	for (int i = 0; i < 11; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/zm_stand/%d.png", i + 1);
		loadimage(&imgZombieStand[i],filename);
	}
}

//初始化植物相关数据
void PlantInitialize()
{
	char filename[64];		//临时文件名，表示加载的图片文件名长度不超过64

	memset(imgPlant, 0, sizeof(imgPlant));	//清空植物图片

	for (int i = 0; i < PlantNum; i++)
	{
		//初始化植物工具栏图片
		sprintf_s(filename, sizeof(filename), "res//res//Cards//card_%d.png", i + 1);
		loadimage(&imgCards[i], filename);

		//初始化植物图片
		for (int j = 0; j < 20; j++)
		{
			sprintf_s(filename, sizeof(filename), "res//res//zhiwu//%d//%d.png", i, j + 1);

			if (fileExist(filename))	//先判断这个文件是否存在
			{
				imgPlant[i][j] = new IMAGE;
				loadimage(imgPlant[i][j], filename);
			}
			else break;
		}

		//初始化植物种类参数
		plants[i].kind = i;
	}

	//初始化植物固有参数
	plants[Peashooter].cost = 100;
	plants[Sunflower].cost = 50;
	plants[Peashooter].HP = 50;
	plants[Sunflower].HP = 50;

	//初始化草坪上的植物
	memset(map, 0, sizeof(map));
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			map[i][j].type = -1;	//-1表示没有植物
		}
	}

	//初始化当前植物状态
	curPlant = -1;		
}

//初始化其它实体相关数据
void EntityInitialize()
{
	char filename[64];		//临时文件名，表示加载的图片文件名长度不超过64

	//初始化阳光池数据
	memset(sunshine, 0, sizeof(sunshine));
	for (int i = 0; i < 29; i++)
	{
		sprintf_s(filename, sizeof(filename), "res/res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], filename);
	}

	//初始化豌豆子弹数据
	memset(peabullets, 0, sizeof(peabullets));
	loadimage(&imgPeaBulletNormal, "res/res/bullets/PeaNormal/PeaNormal_0.png");
	loadimage(&imgPeaBulletBlast[3], "res/res/bullets/PeaNormalExplode/PeaNormalExplode_0.png");;
	for (int i = 0; i < 3; i++)		//实现爆炸颗粒逐渐放大的效果
	{
		float k = (i + 1) * 0.2;
		loadimage(&imgPeaBulletBlast[i], "res/res/bullets/PeaNormalExplode/PeaNormalExplode_0.png",
			imgPeaBulletBlast[3].getwidth() * k,
			imgPeaBulletBlast[3].getheight() * k, true);
	}
}


