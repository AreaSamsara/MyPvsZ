#include "UpdateGameData.h"

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

/*
*	植物相关游戏数据更新
*/

//更新草坪上的植物状态
void UpdatePlantinBlock()
{
	//更新每一格草坪上植物的帧的序号
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type > -1)
			{
				map[i][j].frameIndex++;
				int TypeofPlant = map[i][j].type;
				int FrameIndexofPlant = map[i][j].frameIndex;
				if (imgPlant[TypeofPlant][FrameIndexofPlant] == NULL)	//如果已经播放到最后一帧
				{
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}



/*
*	阳光相关游戏数据更新
*/

//自然生成阳光
void NatureCreateSunshine()
{
	static int SunshineTimer = 0;
	static int SunshineFreq = 50;
	SunshineTimer++;
	if (SunshineTimer < SunshineFreq)	//控制阳光自然生成的频率
		return;

	//刷新计数器
	SunshineFreq = 100 + (rand() % 150);
	SunshineTimer = 0;

	//从阳光池中取一个可以使用的阳光
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//阳光池中最大阳光数目
	int i = 0;
	for (i = 0; i < SunshineNumsinPool && sunshine[i].used; i++);
	if (i == SunshineNumsinPool)	return;		//未找到可使用阳光则退出

	sunshine[i].used = true;
	sunshine[i].status = Sunshine_Down;		//将阳光球设置为竖直下落状态

	sunshine[i].frameIndex = 0;		//初始化阳光球图片帧序号
	sunshine[i].timer = 0;			//初始化阳光消失计时器

	sunshine[i].pcur.x = 260-112 + rand() % (900 - 260);	//在(260-112,900-112)随机取x的值
	sunshine[i].pcur.y = 60;

	sunshine[i].destinationY = 256 + (rand() % RowofMap) * BLOCK_HEIGHT;	//将目标位置设在草坪上

	//设置阳光球竖直落地的贝塞尔参数
	sunshine[i].p1 = vector2(sunshine[i].pcur.x, sunshine[i].pcur.y);
	sunshine[i].p4 = vector2(sunshine[i].pcur.x, sunshine[i].destinationY);
	float distance = sunshine[i].p4.y - sunshine[i].p1.y;
	sunshine[i].speedBezier = 2.0 / distance;
	sunshine[i].time = 0;
}

//植物生成阳光
void PlantCreateSunshine()
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//阳光池中最大阳光数目
	static int SunshineFreq = 20;		//向日葵首次产生阳光较快

	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type == Sunflower)
			{
				map[i][j].sunshineTimer++;
				if (map[i][j].sunshineTimer < SunshineFreq)		continue;	//控制阳光自然生成的频率

				map[i][j].sunshineTimer = 0;			//重置阳光生成计时器
				SunshineFreq = 100 + (rand() % 200);	//设置阳光生成频率

				//从阳光池中取一个可以使用的阳光
				int k = 0;
				for (k = 0; k < SunshineNumsinPool && sunshine[k].used; k++);
				if (k == SunshineNumsinPool)	return;		//未找到可使用阳光则退出

				//初始化生成的阳光球
				sunshine[k].used = true;
				sunshine[k].status = Sunshine_Product;		//将阳光球设置为产出状态
				sunshine[k].frameIndex = 0;		//初始化阳光球图片帧序号
				sunshine[k].timer = 0;			//初始化阳光消失计时器

				//设置阳光球沿弧线抛射落地的贝塞尔参数
				int Dx = (50 + rand() % 50) * (rand() % 2 ? 1 : -1);		//设置阳光球落地x的随机变化范围为(-150,-100)∪(100,150)
				sunshine[k].p1 = vector2(map[i][j].x, map[i][j].y);
				sunshine[k].p4 = vector2(map[i][j].x + Dx, map[i][j].y + imgPlant[Sunflower][0]->getheight() - imgSunshineBall[0].getheight());
				sunshine[k].p2 = vector2(sunshine[k].p1.x + Dx * 0.3, sunshine[k].p1.y - 75);
				sunshine[k].p3 = vector2(sunshine[k].p1.x + Dx * 0.7, sunshine[k].p1.y - 75);
				float distance = sunshine[i].p4.x - sunshine[i].p1.x;
				sunshine[k].speedBezier = 0.1;		//10次走完贝塞尔曲线
				sunshine[i].time = 0;
			}
		}
	}
}

//更新阳光状态
void UpdateSunshine()
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//阳光池中最大阳光数目
	for (int i = 0; i < SunshineNumsinPool; i++)
	{
		if (sunshine[i].used)	//如果阳光正在被使用
		{
			sunshine[i].frameIndex = (sunshine[i].frameIndex + 1) % 29;	//更新阳光转动的图片帧序号

			if (sunshine[i].status == Sunshine_Down)	//如果阳光正在竖直下落
			{
				sunshine[i].time += sunshine[i].speedBezier;
				sunshine[i].pcur = sunshine[i].p1 + sunshine[i].time * (sunshine[i].p4 - sunshine[i].p1);	//pcur=p1+t*(p4-p1)
				if (sunshine[i].time >= 1)		//贝塞尔曲线到达地面后变为落地状态
				{
					sunshine[i].status = Sunshine_Ground;
					sunshine[i].timer = 0;
				}
			}
			else if (sunshine[i].status == Sunshine_Ground)		//如果阳光已经落地
			{
				sunshine[i].timer++;
				if (sunshine[i].timer > 100)	//落地一段时间后消失
					sunshine[i].used = false;
			}
			else if (sunshine[i].status == Sunshine_Collected)	//如果阳光正在被收集
			{
				sunshine[i].time += sunshine[i].speedBezier;
				sunshine[i].pcur = sunshine[i].p1 + sunshine[i].time * (sunshine[i].p4 - sunshine[i].p1);	//pcur=p1+t*(p4-p1)
				if (sunshine[i].time >= 1)		//贝塞尔曲线到达阳光槽后阳光消失
				{
					sunshine[i].used = false;
					SunshineAmount += 25;		//阳光总量增加
				}
			}
			else if (sunshine[i].status == Sunshine_Product)		//如果阳光正在被植物生成
			{
				sunshine[i].time += sunshine[i].speedBezier;
				sunshine[i].pcur = calcBezierPoint(sunshine[i].time,
					sunshine[i].p1, sunshine[i].p2, sunshine[i].p3, sunshine[i].p4);	//绘制贝塞尔曲线

				if (sunshine[i].time >= 1)		//贝塞尔曲线到达地面后变为落地状态
				{
					sunshine[i].status = Sunshine_Ground;
					sunshine[i].timer = 0;
				}
			}
		}
	}
}



/*
*	飞行道具相关游戏数据更新
*/

//发射豌豆子弹
void Shoot()
{
	static int timer = 0;
	timer++;
	if (timer < 5)	return;		//控制所有豌豆射手的发射频率
	timer = 0;

	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//僵尸池中最大僵尸数目
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//豌豆子弹池中最大豌豆子弹数目
	int thresholdX = WIN_WIDTH - imgZombie[0].getwidth() / 2;		//植物进攻的警戒线

	//豌豆射手发射子弹
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type == Peashooter)
			{
				bool zombieExist=false;		//判断该植物攻击范围内是否存在僵尸
				for (int k = 0; k < ZombieNumsinPool; k++)
				{
					//判定每一路有没有僵尸出现且：1.越过了临界线；2：在植物的攻击范围内；3：与植物在同一排
					if (zombies[k].used && zombies[k].x < thresholdX && zombies[k].x > map[i][j].x && zombies[k].row==i)
					{
						zombieExist = true;
						break;
					}
				}
				if (zombieExist == false)	continue;		//如果没有发现满足条件的僵尸，该植物不发射

				map[i][j].shootTimer++;
				if (map[i][j].shootTimer < 10)	continue;	//	控制单个豌豆射手的发射频率

				map[i][j].shootTimer = 0;		//刷新发射计时器
				int k;
				for (k = 0; k < PeaBulletNumsinPool && peabullets[k].used; k++);
				if (k == PeaBulletNumsinPool)		return;		//未找到可使用的豌豆子弹则退出

				peabullets[k].used = true;
				peabullets[k].row = i;
				peabullets[k].speedFly = 10;
				peabullets[k].blast = false;
				peabullets[k].frameIndex = 0;

				int PlantX = 256-112 + j * BLOCK_WIDTH;
				int PlantY = 77 + i  * BLOCK_HEIGHT + 14;
				peabullets[k].x = PlantX + imgPlant[Peashooter][0]->getwidth() - 10;
				peabullets[k].y = PlantY;		//豌豆子弹发射的起始位置	

			}
		}
	}
}

//更新豌豆子弹状态
void UpdatePeabullet()
{
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//豌豆子弹池中最大豌豆子弹数目
	for (int i = 0; i < PeaBulletNumsinPool; i++)
	{
		if (peabullets[i].used)
		{
			peabullets[i].x += peabullets[i].speedFly;

			if (peabullets[i].x > WIN_WIDTH)	//若豌豆子弹飞出窗口
			{
				peabullets[i].used = false;		//回收豌豆子弹
			}

			if (peabullets[i].blast)	//若豌豆子弹发生爆炸
			{
				peabullets[i].frameIndex++;		//更新豌豆爆炸的帧序号
				if (peabullets[i].frameIndex >= 4)
				{
					peabullets[i].used = false;	//回收豌豆子弹
				}
			}
		}
	}
}



/*
*	僵尸相关游戏数据更新
*/

//生成僵尸
void CreateZombie()
{
	static int ZombieTimer = 0;
	static int ZombieFreq = 50;
	ZombieTimer++;
	if (ZombieTimer < ZombieFreq)	//控制僵尸自然生成的频率
		return;
	if (zombieCreated >= ZombieAmount)	//如果已经生成了所有的僵尸，则停止生成
		return;

	//刷新计数器
	ZombieFreq = 100 + (rand() % 100);
	ZombieTimer = 0;

	//从僵尸池中取一个可以使用的僵尸
	int i;
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//僵尸池中最大僵尸数目
	for (i = 0; i < ZombieNumsinPool && zombies[i].used; i++);
	if (i == ZombieNumsinPool)		return;		//未找到可使用的僵尸则退出

	zombies[i].used = true;
	zombies[i].x = WIN_WIDTH;
	zombies[i].row = rand() % RowofMap;	//允许僵尸在1,2,3,4,5路出现
	zombies[i].y = 172 + zombies[i].row * 100;
	zombies[i].speed = 1;		//僵尸移动速度设置为1
	zombies[i].HP = 100;		//僵尸的生命值设置为100
	zombies[i].damage = 1;		//僵尸的攻击伤害设置为1
	zombies[i].dead = false;	//重置僵尸的死亡状态
	zombies[i].eating = false;
	zombieCreated++;		
}

//更新僵尸状态
void UpdateZombie()
{
	static int timer = 0;
	timer++;
	if (timer < 2)	return;		//控制更新僵尸数据的频率
	timer = 0;

	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//僵尸池中最大僵尸数目
	for (int i = 0; i < ZombieNumsinPool; i++)
	{
		if (zombies[i].used)
		{
			if (zombies[i].dead)
			{
				//更新僵尸死亡动作的图片帧序号
				zombies[i].frameIndex++;
				if (zombies[i].frameIndex >= 20)	//播放完死亡动画后回收僵尸
					zombies[i].used = false;
			}
			else if (zombies[i].eating)
			{
				//更新僵尸吃植物动作的图片帧序号
				zombies[i].frameIndex = (zombies[i].frameIndex + 1) % 21;
			}
			else
			{
				//更新僵尸的位置
				zombies[i].x -= zombies[i].speed;	//僵尸移动
				if (zombies[i].x < 170-112)				//如果僵尸进了家门，游戏失败
				{
					gameStatus = FAIL;
				}

				//更新僵尸移动动作的图片帧序号
				zombies[i].frameIndex = (zombies[i].frameIndex + 1) % 22;
			}
		}
	}
}



/*
*	事件相关游戏数据更新
*/

//检测是否有事件发生
void IncidentCheck()
{
	CollisionCheck();	//检测是否有豌豆子弹命中僵尸
	EatCheck();			//检测是否有僵尸准备吃植物
}

//检测是否有豌豆子弹命中僵尸
void CollisionCheck()
{
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//豌豆子弹池中最大豌豆子弹数目
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//僵尸池中最大僵尸数目
	for (int i = 0; i < PeaBulletNumsinPool; i++)
	{
		//筛选掉没必要检测的情况
		if (peabullets[i].used == false || peabullets[i].blast)
			continue;

		for (int j = 0; j < ZombieNumsinPool; j++)
		{
			//筛选掉没必要检测的情况
			if (zombies[j].used == false || zombies[j].dead || peabullets[i].row != zombies[j].row)
				continue;

			int x1 = zombies[j].x + 70;
			int x2 = zombies[j].x + 110;
			int x = peabullets[i].x;
			if (x > x1 && x < x2)
			{
				PlaySound("res/res/audio/splat1.wav", NULL, SND_FILENAME | SND_ASYNC);	//播放豌豆子弹命中的音效
				zombies[j].HP -= 10;		//僵尸受到伤害
				peabullets[i].blast = true;
				peabullets[i].speedFly = 0;		//使豌豆子弹停止
				
				if (zombies[j].HP <= 0)		//判定僵尸是否死亡
				{
					zombies[j].dead = true;
					zombies[j].speed = 0;
					zombies[j].frameIndex = 0;	//重置图片帧使之为死亡所用
					killcount++;				//计算击杀的僵尸数目

					if (killcount >= ZombieAmount)		//击杀所有的僵尸后游戏胜利
						gameStatus = WIN;
				}
				break;	//豌豆子弹已被消耗，直接进入下一颗子弹的判定
			}
		}
	}
}

//检测是否有僵尸准备吃植物
void EatCheck()
{
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//僵尸池中最大僵尸数目
	for (int i = 0; i < ZombieNumsinPool; i++)
	{
		if (zombies[i].used == false || zombies[i].dead)	continue;	//筛选掉没必要检测的情况
		int row = zombies[i].row;
		for (int j = ColumnofMap - 1; j >= 0; j--)
		{
			int x0 = 256-112 + j * 81;		//植物的x坐标
			int x1 = x0 + 10;
			int x2 = x0 + 60;
			int x = zombies[i].x + 80;		//僵尸吃植物的有效x坐标

			if (x > x1 && x < x2)		//如果僵尸在吃植物的范围内
			{
				if (map[row][j].type == -1)		//若该处没有植物
				{
					if (zombies[i].eating)
					zombies[i].eating = false;		//不要继续吃空气了
					continue;					//没有植物则跳过
				}

				if (zombies[i].eating == false)		//如果僵尸正准备吃
				{
					//循环播放僵尸吃植物的音效
					mciSendString("play res/res/audio/chompsoft.mp3 repeat", 0, 0, 0);

					zombies[i].eating = true;
					zombies[i].speed = 0;
					map[row][j].eaten = true;
				}
				
				map[row][j].HP -= zombies[i].damage;

				if (map[row][j].HP <= 0)	//如果植物被吃掉
				{
					//关闭僵尸吃植物的音效
					mciSendString("close res/res/audio/chompsoft.mp3", 0, 0, 0);
					map[row][j].type = -1;
					map[row][j].eaten = false;
					zombies[i].eating = false;
					zombies[i].frameIndex = 0;	//重置僵尸图片帧序号
					zombies[i].speed = 1;
				}
			}
		}
	}
}

//检测游戏是否结束
bool OverCheck()
{
	return gameStatus == WIN || gameStatus == FAIL;
}