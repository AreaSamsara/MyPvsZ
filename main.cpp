/*
*开发日志
* 1.创建新项目
* 2.导入素材
* 3.实现最开始的游戏场景
* 4.实现游戏顶部的工具栏
* 5.实现工具栏中的植物卡牌
* 
* tips：凡是带有"-112"字样的参数皆为窗口向右滑动所致
*/

#include "Global.h"
#include "UpdateGameData.h"
#include "UpdateWindow.h"
#include "MyTools.h"
#include "GameInitialize.h"

enum{Peashooter,Sunflower,PlantNum};	//表示当前植物种类的枚举类型
enum{Sunshine_Down,Sunshine_Ground,Sunshine_Collected,Sunshine_Product};	//表示阳光球状态的枚举类型
enum{RUNNING,WIN,FAIL};		//表示当前游戏运行状态的枚举变量

IMAGE imgBackground;	//背景图片
IMAGE imgBar;	//植物工具栏图片 
IMAGE imgCards[PlantNum];	//植物卡牌
IMAGE *imgPlant[PlantNum][20];	//植物图片，每个植物图片数量不超过20
IMAGE imgSunshineBall[29];		//阳光球图片，共29张
IMAGE imgZombie[22];			//僵尸图片，共22张
IMAGE imgZombieDead[20];		//僵尸死亡图片，共20张
IMAGE imgZombieEat[21];			//僵尸吃植物图片，共21张
IMAGE imgZombieStand[11];		//僵尸站立图片，共11张
IMAGE imgPeaBulletNormal;		//豌豆子弹正常状态下的图片
IMAGE imgPeaBulletBlast[4];		//豌豆子弹爆炸状态的图片，共4张

int curX, curY;		//当前选中的植物在被拖动过程中的坐标
int curPlant;		//-1：表示未选中；n：表示选择了枚举类型中的第n种植物
int SunshineAmount=50;	//初始阳光总量设置为50
int killcount;		//已经击杀的僵尸数量
int zombieCreated;	//已经生成的僵尸总数
int gameStatus;		//当前游戏运行状态

bool sunshineShortage;		//表示是否在阳光不足时试图选择植物

struct Plant plants[PlantNum];

struct PlantinBlock map[RowofMap][ColumnofMap];

struct SunshineBall sunshine[10];

struct Zombie zombies[10];

struct PeaBullet peabullets[30];

void CollectSunshine(ExMessage* msg)
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//阳光池中最大阳光数目
	for (int i = 0; i < SunshineNumsinPool; i++)
	{
		int WidthofSunshine = imgSunshineBall[0].getwidth();
		int HeightofSunshine = imgSunshineBall[0].getheight();

		if (sunshine[i].used)		//如果阳光未被收集且未消失
		{
			int x = sunshine[i].pcur.x;
			int y = sunshine[i].pcur.y;
			if (msg->x > x && msg->x < x + WidthofSunshine &&
				msg->y > y && msg->y < y + HeightofSunshine)
			{
				sunshine[i].status = Sunshine_Collected;
				//mciSendString("play res/res/sunshine.mp3", 0, 0, 0);	//硬核播放法
				PlaySound("res/res/sunshine.wav",NULL,SND_FILENAME|SND_ASYNC);	//延迟更小的播放模式，但不支持mp3
				sunshine[i].p1 = sunshine[i].pcur;
				sunshine[i].p4 = vector2(262-112, 0);
				float distance = dis(sunshine[i].p1 - sunshine[i].p4);
				sunshine[i].time = 0;
				sunshine[i].speedBezier = 32.0 / distance;
				return;		//一次鼠标点击至多收集一次阳光
			}
		}
	}
}

void GameInitialize()
{
	//初始化游戏窗口相关数据
	WindowInitialize();

	//初始化全局参数，需要先初始化游戏窗口
	GlobalParameterInitialize();

	//初始化僵尸相关数据
	ZombieInitialize();

	//初始化植物相关数据
	PlantInitialize();

	//初始化其它实体相关数据
	EntityInitialize();
}

void UpdateWindow()
{
	BeginBatchDraw();	//开始缓冲
	 
	//渲染背景图片
	putimage(0-112, 0, &imgBackground);
	putimagePNG(250-112,0, &imgBar);	//用带有PNG后缀的函数可除去黑边

	//渲染植物工具栏
	DrawPlantBar();

	//渲染每一格草坪上方的植物
	DrawPlantinBlock();

	//渲染拖动过程中被拖动的植物
	DrawPlantDragged();

	//渲染阳光球和阳光总量
	DrawSunshine();

	//渲染僵尸
	DrawZombie();
	
	//渲染豌豆子弹
	DrawPeabullet();

	//渲染阳光不足的闪烁效果
	DrawSunshineShortage();

	EndBatchDraw();	//结束双缓冲
}

void UserClick()
{ 
	ExMessage msg;
	static bool status=false;	//表示是否选中了植物
	if (peekmessage(&msg))	//检测是否有信息
	{
		if (msg.message == WM_LBUTTONDOWN)	//左键按下
		{
			if (msg.x > 338-112 && msg.x < 338 -112+ 65 * PlantNum && msg.y < 96)	//如果鼠标在植物工具栏
			{
				curPlant = (msg.x - (338 - 112)) / 65;	//curPlant的序号从0开始
				if (SunshineAmount < plants[curPlant].cost)		//如果阳光不足
				{
					sunshineShortage = true;		//在阳光不足时试图选择植物
					PlaySound("res/res/audio/bleep.wav", NULL, SND_FILENAME | SND_ASYNC);	//播放阳光不足的音效
					curPlant = -1;					//重置当前植物
				}
				else
				{
					PlaySound("res/res/audio/plant1.wav", NULL, SND_FILENAME | SND_ASYNC);	//播放选中植物的音效
					status = true;			//表示已经选中了植物
				}
			}
			else                   //否则被判定为试图收集阳光
			{
				CollectSunshine(&msg);		
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status==true)	//植物被选中时鼠标移动
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status==true)	//植物被选中时鼠标松开
		{
			if (msg.x > 256-112 && msg.y > 77 && msg.y < 591)	//如果松开位置在草坪上
			{
				int row = (msg.y - 77) / BLOCK_HEIGHT;
				int col = (msg.x - (256-112)) / BLOCK_WIDTH;
				if (map[row][col].type == -1)	//如果该草坪上没有植物
				{
					map[row][col].type = curPlant;
					map[row][col].frameIndex = 0;
					map[row][col].eaten = false;
					map[row][col].HP = plants[curPlant].HP;
					map[row][col].x= 256-112 + col * 81;
					map[row][col].y = 77 + row * 102 + 14;
					PlaySound("res/res/audio/plant2.wav", NULL, SND_FILENAME | SND_ASYNC);	//播放植物种植的音效

					//消耗阳光
					switch (curPlant)
					{
					case Peashooter:
						SunshineAmount -= plants[Peashooter].cost;
						break;
					case Sunflower:
						SunshineAmount -= plants[Sunflower].cost;
						break;
					default:
						break;
					}
				}
			}
			
			curPlant = -1;			//重置当前植物
			status = false;			//重置选中状态为未选中
		}
	}
}

void UpdateGameData()
{
	static int timer = 0;
	timer++;
	if (timer < 5)	return;		//控制更新游戏数据的总频率
	timer = 0;

	UpdatePlantinBlock();	//更新草坪上的植物状态

	NatureCreateSunshine();	//自然生成阳光
	PlantCreateSunshine();	//植物生成阳光
	UpdateSunshine();	//更新阳光状态

	CreateZombie();		//生成僵尸
	UpdateZombie();		//更新僵尸状态

	Shoot();			//发射子弹
	UpdatePeabullet();	//更新豌豆子弹状态
	IncidentCheck();	//检测是否有事件发生
}

void StartUI()
{
	IMAGE imgBackgroundofUI,imgMenu1,imgMenu2;
	loadimage(&imgBackgroundofUI, "res//res//menu.png");
	loadimage(&imgMenu1, "res//res//menu1.png");
	loadimage(&imgMenu2, "res//res//menu2.png");

	//渲染启动菜单背景
	bool flag = false;		//true：已点击菜单；false：未点击菜单
	ExMessage msginUI;
	while (1)
	{
		BeginBatchDraw();	//开始缓冲

		putimage(0, 0, &imgBackgroundofUI);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		mciSendString("play res/res/audio/Faster.mp3 repeat", 0, 0, 0);		//循环播放UI的背景音乐

		if (peekmessage(&msginUI))
		{
			if (msginUI.message == WM_LBUTTONDOWN &&
				msginUI.x>474 && msginUI.x<474+300 &&
				msginUI.y>75 && msginUI.y <75+140)
			{
				flag = true;
			}
			else if (msginUI.message == WM_LBUTTONUP && flag==true)
			{
				mciSendString("close res/res/audio/Faster.mp3", 0, 0, 0);		//停止播放UI的背景音乐
				PlaySound("res/res/audio/evillaugh.wav", NULL, SND_FILENAME | SND_SYNC);	//播放开始游戏的音乐，播放完后再继续
				EndBatchDraw();		//结束双缓冲
				return;
			}
		}
		
		EndBatchDraw();		//结束双缓冲
	}
}

void ViewScene()
{
	int xMin = WIN_WIDTH - imgBackground.getwidth();	//900-1400=-500
	int speedSlide = 2;					//巡场动画中窗口滑动的速度
	const int ZombieNumsinScene = 9;		//巡场动画中僵尸的数量
	int frameIndex[9] = {0};			//各个僵尸的图片帧序号

	vector2 ZombiePosition[ZombieNumsinScene] = {
		{550,80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340} };	//僵尸的位移向量

	//控制僵尸抖动的频率
	static int timer = 0;
	int trembleFreq = 10;

	//随机设置每个僵尸的初始图片帧序号
	for (int i = 0; i < ZombieNumsinScene; i++)	
	{
		frameIndex[i] = rand() % 11;
	}

	//循环播放巡场动画的背景音乐
	mciSendString("play res/res/audio/Look_up_at_the_Sky.mp3 repeat", 0, 0, 0);	

	//窗口向右滑动
	for (int x = 0; x >= xMin; x -= speedSlide)
	{
		BeginBatchDraw();	//开始缓冲
		putimage(x, 0, &imgBackground);		//渲染背景图片
		timer++;		//控制僵尸抖动的频率

		for (int i = 0; i < ZombieNumsinScene; i++)		//渲染站立的僵尸
		{
			putimagePNG(ZombiePosition[i].x-xMin+x, ZombiePosition[i].y,&imgZombieStand[frameIndex[i]]);
			if (timer > trembleFreq)	
				frameIndex[i] = rand() % 11;	//随机刷新图片帧序号，造成抖动效果
		}
		if (timer > trembleFreq)	timer = 0;			//刷新计数器

		EndBatchDraw();		//结束双缓冲
		Sleep(5);			//控制巡场动画的播放速度
	}

	//窗口滑动到底以后停留2s左右
	for (int i=0;i<2000;i++)
	{
		BeginBatchDraw();	//开始缓冲
		putimage(xMin, 0, &imgBackground);		//渲染背景图片

		timer++;	//控制僵尸抖动的频率

		for (int i = 0; i < ZombieNumsinScene; i++)		//渲染站立的僵尸
		{
			putimagePNG(ZombiePosition[i].x, ZombiePosition[i].y, &imgZombieStand[frameIndex[i]]);
			if (timer > trembleFreq)
				frameIndex[i] = rand() % 11;	//随机刷新图片帧序号，造成抖动效果
		}
		if (timer > trembleFreq)	timer = 0;			//刷新计数器

		EndBatchDraw();		//结束双缓冲
		Sleep(1);			//停留时间
	}

	//窗口向左滑动
	for (int x = xMin; x <=0-112; x += speedSlide)		//向左滑动后不滑到底，而是滑到适合种植物的视野(-112)
	{
		BeginBatchDraw();	//开始缓冲
		putimage(x, 0, &imgBackground);		//渲染背景图片

		timer++;	//控制僵尸抖动的频率

		for (int i = 0; i < ZombieNumsinScene; i++)		//渲染站立的僵尸
		{
			putimagePNG(ZombiePosition[i].x-xMin+x, ZombiePosition[i].y,&imgZombieStand[frameIndex[i]]);
			if (timer > trembleFreq)	
				frameIndex[i] = rand() % 11;	//随机刷新图片帧序号，造成抖动效果
		}
		if (timer > trembleFreq)	timer = 0;			//刷新计数器

		EndBatchDraw();		//结束双缓冲
		Sleep(5);			//控制巡场动画的播放速度
	}

	//关闭巡场动画的背景音乐
	mciSendString("close res/res/audio/Look_up_at_the_Sky.mp3", 0, 0, 0);
}

void BarDown()
{
	int height = imgBar.getheight();
	for (int y = -height; y <= 0; y++)
	{
		BeginBatchDraw();	//开始缓冲

		putimage(-112, 0, &imgBackground);	//渲染背景图片
		putimagePNG(250-112, y, &imgBar);

		for (int i = 0; i < PlantNum; i++)
		{
			int x = 338-112 + i * 65;
			putimage(x, y+6, &imgCards[i]);
		}

		EndBatchDraw();		//结束双缓冲
		Sleep(5);			//控制动画的播放速度
	}

	//准备种植植物，待优化
	//播放准备种植植物的音效，播放完后再继续
	PlaySound("res/res/audio/readysetplant.wav", NULL, SND_FILENAME | SND_SYNC);
}

int main(void)
{
	GameInitialize();
	StartUI();		//进入游戏启动界面
	ViewScene();	//播放巡场动画
	BarDown();		//播放工具栏滑动的动画

	int timer = 0;			//游戏运行时的计时器
	int timerOver = 0;		//游戏结束的延迟计时器
	mciSendString("play res/res/audio/UraniwaNi.mp3 repeat", 0, 0, 0);		//循环播放游戏的背景音乐
	while (1)	//游戏循环
	{
		UserClick();
		
		//每间隔?ms更新一次游戏数据
		timer += getDelay();
		if (timer > 10)
		{
			UpdateWindow();
			UpdateGameData();

			if (OverCheck())	//如果游戏结束，则退出游戏循环
			{
				timerOver++;
				if (timerOver > 100)		//延迟一定的时间再结束游戏
					break;
			}

			timer = 0;		//计时器清零
		}
	}

	//游戏结束
	Sleep(1000);
	mciSendString("close res/res/audio/UraniwaNi.mp3", 0, 0, 0);		//关闭游戏的背景音乐

	if (gameStatus == WIN)
	{
		loadimage(0, "res/res/win2.png");
		mciSendString("play res/res/win.mp3", 0, 0, 0);	//硬核播放法
	}
	else if (gameStatus == FAIL)
	{
		loadimage(0, "res/res/fail2.png");
		PlaySound("res/res/lose.wav", NULL, SND_FILENAME | SND_SYNC);		//播放失败音效，播放完后再继续
		PlaySound("res/res/audio/scream.wav", NULL, SND_FILENAME | SND_SYNC);		//播放尖叫，播放完后再继续
	}

	system("pause");
	return 0;
}

