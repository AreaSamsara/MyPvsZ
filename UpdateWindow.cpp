#include "UpdateWindow.h"

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

extern bool sunshineShortage;		//表示是否在阳光不足时试图选择植物

extern struct Plant plants[PlantNum];
extern struct PlantinBlock map[RowofMap][ColumnofMap];
extern struct SunshineBall sunshine[10];
extern struct Zombie zombies[10];
extern struct PeaBullet peabullets[30];


//渲染植物工具栏
void DrawPlantBar()
{
	for (int i = 0; i < PlantNum; i++)
	{
		int x = 338-112 + i * 65;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

//渲染每一格草坪上方的植物
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

//渲染拖动过程中被拖动的植物
void DrawPlantDragged()
{
	if (curPlant > -1)	//如果选中了植物
	{
		IMAGE* img = imgPlant[curPlant][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);	//使支持透明的植物图片
	}
}

//渲染阳光球和阳光总量
void DrawSunshine()
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//阳光池中最大阳光数目
	//渲染转动过程中的阳光球
	for (int i = 0; i < SunshineNumsinPool; i++)
	{
		if (sunshine[i].used || sunshine[i].vector_x)
		{
			IMAGE* img = &imgSunshineBall[sunshine[i].frameIndex];
			putimagePNG(sunshine[i].pcur.x, sunshine[i].pcur.y, img);
		}
	}

	//渲染阳光总量
	char SunshineamountText[8];
	sprintf_s(SunshineamountText, sizeof(SunshineamountText), "%d", SunshineAmount);
	outtextxy(276-112, 67, SunshineamountText);		//打印阳光总量文本
}

//渲染僵尸
void DrawZombie()
{
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//僵尸池中最大僵尸数目
	for (int i = 0; i < ZombieNumsinPool; i++)
	{
		if (zombies[i].used)
		{
			IMAGE* img = NULL;		//根据僵尸的状态选择图片序列
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

//渲染豌豆子弹
void DrawPeabullet()
{
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//豌豆子弹池中最大豌豆子弹数目
	for (int i = 0; i < PeaBulletNumsinPool; i++)
	{
		if (peabullets[i].used == false)	continue;	//跳过未被使用的豌豆子弹
		if (peabullets[i].blast)	//如果子弹正在发生爆炸
		{
			IMAGE* img = &imgPeaBulletBlast[peabullets[i].frameIndex];
			putimagePNG(peabullets[i].x, peabullets[i].y, img);
		}
		else	//如果子弹未发生爆炸
		{
			putimagePNG(peabullets[i].x, peabullets[i].y, &imgPeaBulletNormal);
		}
	}
}

//渲染阳光不足的闪烁效果
void DrawSunshineShortage()
{
	if (!sunshineShortage)			//如果未在阳光不足时试图选择植物，则不渲染
	{
		setcolor(BLACK);			//设置字体为黑色
		return;
	}
	
	//实现红黑交替的效果
	setcolor(RED);				//设置字体为红色

	static int flashTimer = 0;
	flashTimer++;
	if (flashTimer < 10)	//控制闪烁的频率
		return;

	sunshineShortage = false;		//重置阳光不足状态
	flashTimer = 0;					//重置闪烁计时器
}










