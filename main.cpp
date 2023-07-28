/*
*������־
* 1.��������Ŀ
* 2.�����ز�
* 3.ʵ���ʼ����Ϸ����
* 4.ʵ����Ϸ�����Ĺ�����
* 5.ʵ�ֹ������е�ֲ�￨��
* 
* tips�����Ǵ���"-112"�����Ĳ�����Ϊ�������һ�������
*/

#include "Global.h"
#include "UpdateGameData.h"
#include "UpdateWindow.h"
#include "MyTools.h"
#include "GameInitialize.h"

enum{Peashooter,Sunflower,PlantNum};	//��ʾ��ǰֲ�������ö������
enum{Sunshine_Down,Sunshine_Ground,Sunshine_Collected,Sunshine_Product};	//��ʾ������״̬��ö������
enum{RUNNING,WIN,FAIL};		//��ʾ��ǰ��Ϸ����״̬��ö�ٱ���

IMAGE imgBackground;	//����ͼƬ
IMAGE imgBar;	//ֲ�﹤����ͼƬ 
IMAGE imgCards[PlantNum];	//ֲ�￨��
IMAGE *imgPlant[PlantNum][20];	//ֲ��ͼƬ��ÿ��ֲ��ͼƬ����������20
IMAGE imgSunshineBall[29];		//������ͼƬ����29��
IMAGE imgZombie[22];			//��ʬͼƬ����22��
IMAGE imgZombieDead[20];		//��ʬ����ͼƬ����20��
IMAGE imgZombieEat[21];			//��ʬ��ֲ��ͼƬ����21��
IMAGE imgZombieStand[11];		//��ʬվ��ͼƬ����11��
IMAGE imgPeaBulletNormal;		//�㶹�ӵ�����״̬�µ�ͼƬ
IMAGE imgPeaBulletBlast[4];		//�㶹�ӵ���ը״̬��ͼƬ����4��

int curX, curY;		//��ǰѡ�е�ֲ���ڱ��϶������е�����
int curPlant;		//-1����ʾδѡ�У�n����ʾѡ����ö�������еĵ�n��ֲ��
int SunshineAmount=50;	//��ʼ������������Ϊ50
int killcount;		//�Ѿ���ɱ�Ľ�ʬ����
int zombieCreated;	//�Ѿ����ɵĽ�ʬ����
int gameStatus;		//��ǰ��Ϸ����״̬

bool sunshineShortage;		//��ʾ�Ƿ������ⲻ��ʱ��ͼѡ��ֲ��

struct Plant plants[PlantNum];

struct PlantinBlock map[RowofMap][ColumnofMap];

struct SunshineBall sunshine[10];

struct Zombie zombies[10];

struct PeaBullet peabullets[30];

void CollectSunshine(ExMessage* msg)
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//����������������Ŀ
	for (int i = 0; i < SunshineNumsinPool; i++)
	{
		int WidthofSunshine = imgSunshineBall[0].getwidth();
		int HeightofSunshine = imgSunshineBall[0].getheight();

		if (sunshine[i].used)		//�������δ���ռ���δ��ʧ
		{
			int x = sunshine[i].pcur.x;
			int y = sunshine[i].pcur.y;
			if (msg->x > x && msg->x < x + WidthofSunshine &&
				msg->y > y && msg->y < y + HeightofSunshine)
			{
				sunshine[i].status = Sunshine_Collected;
				//mciSendString("play res/res/sunshine.mp3", 0, 0, 0);	//Ӳ�˲��ŷ�
				PlaySound("res/res/sunshine.wav",NULL,SND_FILENAME|SND_ASYNC);	//�ӳٸ�С�Ĳ���ģʽ������֧��mp3
				sunshine[i].p1 = sunshine[i].pcur;
				sunshine[i].p4 = vector2(262-112, 0);
				float distance = dis(sunshine[i].p1 - sunshine[i].p4);
				sunshine[i].time = 0;
				sunshine[i].speedBezier = 32.0 / distance;
				return;		//һ������������ռ�һ������
			}
		}
	}
}

void GameInitialize()
{
	//��ʼ����Ϸ�����������
	WindowInitialize();

	//��ʼ��ȫ�ֲ�������Ҫ�ȳ�ʼ����Ϸ����
	GlobalParameterInitialize();

	//��ʼ����ʬ�������
	ZombieInitialize();

	//��ʼ��ֲ���������
	PlantInitialize();

	//��ʼ������ʵ���������
	EntityInitialize();
}

void UpdateWindow()
{
	BeginBatchDraw();	//��ʼ����
	 
	//��Ⱦ����ͼƬ
	putimage(0-112, 0, &imgBackground);
	putimagePNG(250-112,0, &imgBar);	//�ô���PNG��׺�ĺ����ɳ�ȥ�ڱ�

	//��Ⱦֲ�﹤����
	DrawPlantBar();

	//��Ⱦÿһ���ƺ�Ϸ���ֲ��
	DrawPlantinBlock();

	//��Ⱦ�϶������б��϶���ֲ��
	DrawPlantDragged();

	//��Ⱦ���������������
	DrawSunshine();

	//��Ⱦ��ʬ
	DrawZombie();
	
	//��Ⱦ�㶹�ӵ�
	DrawPeabullet();

	//��Ⱦ���ⲻ�����˸Ч��
	DrawSunshineShortage();

	EndBatchDraw();	//����˫����
}

void UserClick()
{ 
	ExMessage msg;
	static bool status=false;	//��ʾ�Ƿ�ѡ����ֲ��
	if (peekmessage(&msg))	//����Ƿ�����Ϣ
	{
		if (msg.message == WM_LBUTTONDOWN)	//�������
		{
			if (msg.x > 338-112 && msg.x < 338 -112+ 65 * PlantNum && msg.y < 96)	//��������ֲ�﹤����
			{
				curPlant = (msg.x - (338 - 112)) / 65;	//curPlant����Ŵ�0��ʼ
				if (SunshineAmount < plants[curPlant].cost)		//������ⲻ��
				{
					sunshineShortage = true;		//�����ⲻ��ʱ��ͼѡ��ֲ��
					PlaySound("res/res/audio/bleep.wav", NULL, SND_FILENAME | SND_ASYNC);	//�������ⲻ�����Ч
					curPlant = -1;					//���õ�ǰֲ��
				}
				else
				{
					PlaySound("res/res/audio/plant1.wav", NULL, SND_FILENAME | SND_ASYNC);	//����ѡ��ֲ�����Ч
					status = true;			//��ʾ�Ѿ�ѡ����ֲ��
				}
			}
			else                   //�����ж�Ϊ��ͼ�ռ�����
			{
				CollectSunshine(&msg);		
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status==true)	//ֲ�ﱻѡ��ʱ����ƶ�
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status==true)	//ֲ�ﱻѡ��ʱ����ɿ�
		{
			if (msg.x > 256-112 && msg.y > 77 && msg.y < 591)	//����ɿ�λ���ڲ�ƺ��
			{
				int row = (msg.y - 77) / BLOCK_HEIGHT;
				int col = (msg.x - (256-112)) / BLOCK_WIDTH;
				if (map[row][col].type == -1)	//����ò�ƺ��û��ֲ��
				{
					map[row][col].type = curPlant;
					map[row][col].frameIndex = 0;
					map[row][col].eaten = false;
					map[row][col].HP = plants[curPlant].HP;
					map[row][col].x= 256-112 + col * 81;
					map[row][col].y = 77 + row * 102 + 14;
					PlaySound("res/res/audio/plant2.wav", NULL, SND_FILENAME | SND_ASYNC);	//����ֲ����ֲ����Ч

					//��������
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
			
			curPlant = -1;			//���õ�ǰֲ��
			status = false;			//����ѡ��״̬Ϊδѡ��
		}
	}
}

void UpdateGameData()
{
	static int timer = 0;
	timer++;
	if (timer < 5)	return;		//���Ƹ�����Ϸ���ݵ���Ƶ��
	timer = 0;

	UpdatePlantinBlock();	//���²�ƺ�ϵ�ֲ��״̬

	NatureCreateSunshine();	//��Ȼ��������
	PlantCreateSunshine();	//ֲ����������
	UpdateSunshine();	//��������״̬

	CreateZombie();		//���ɽ�ʬ
	UpdateZombie();		//���½�ʬ״̬

	Shoot();			//�����ӵ�
	UpdatePeabullet();	//�����㶹�ӵ�״̬
	IncidentCheck();	//����Ƿ����¼�����
}

void StartUI()
{
	IMAGE imgBackgroundofUI,imgMenu1,imgMenu2;
	loadimage(&imgBackgroundofUI, "res//res//menu.png");
	loadimage(&imgMenu1, "res//res//menu1.png");
	loadimage(&imgMenu2, "res//res//menu2.png");

	//��Ⱦ�����˵�����
	bool flag = false;		//true���ѵ���˵���false��δ����˵�
	ExMessage msginUI;
	while (1)
	{
		BeginBatchDraw();	//��ʼ����

		putimage(0, 0, &imgBackgroundofUI);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		mciSendString("play res/res/audio/Faster.mp3 repeat", 0, 0, 0);		//ѭ������UI�ı�������

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
				mciSendString("close res/res/audio/Faster.mp3", 0, 0, 0);		//ֹͣ����UI�ı�������
				PlaySound("res/res/audio/evillaugh.wav", NULL, SND_FILENAME | SND_SYNC);	//���ſ�ʼ��Ϸ�����֣���������ټ���
				EndBatchDraw();		//����˫����
				return;
			}
		}
		
		EndBatchDraw();		//����˫����
	}
}

void ViewScene()
{
	int xMin = WIN_WIDTH - imgBackground.getwidth();	//900-1400=-500
	int speedSlide = 2;					//Ѳ�������д��ڻ������ٶ�
	const int ZombieNumsinScene = 9;		//Ѳ�������н�ʬ������
	int frameIndex[9] = {0};			//������ʬ��ͼƬ֡���

	vector2 ZombiePosition[ZombieNumsinScene] = {
		{550,80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340} };	//��ʬ��λ������

	//���ƽ�ʬ������Ƶ��
	static int timer = 0;
	int trembleFreq = 10;

	//�������ÿ����ʬ�ĳ�ʼͼƬ֡���
	for (int i = 0; i < ZombieNumsinScene; i++)	
	{
		frameIndex[i] = rand() % 11;
	}

	//ѭ������Ѳ�������ı�������
	mciSendString("play res/res/audio/Look_up_at_the_Sky.mp3 repeat", 0, 0, 0);	

	//�������һ���
	for (int x = 0; x >= xMin; x -= speedSlide)
	{
		BeginBatchDraw();	//��ʼ����
		putimage(x, 0, &imgBackground);		//��Ⱦ����ͼƬ
		timer++;		//���ƽ�ʬ������Ƶ��

		for (int i = 0; i < ZombieNumsinScene; i++)		//��Ⱦվ���Ľ�ʬ
		{
			putimagePNG(ZombiePosition[i].x-xMin+x, ZombiePosition[i].y,&imgZombieStand[frameIndex[i]]);
			if (timer > trembleFreq)	
				frameIndex[i] = rand() % 11;	//���ˢ��ͼƬ֡��ţ���ɶ���Ч��
		}
		if (timer > trembleFreq)	timer = 0;			//ˢ�¼�����

		EndBatchDraw();		//����˫����
		Sleep(5);			//����Ѳ�������Ĳ����ٶ�
	}

	//���ڻ��������Ժ�ͣ��2s����
	for (int i=0;i<2000;i++)
	{
		BeginBatchDraw();	//��ʼ����
		putimage(xMin, 0, &imgBackground);		//��Ⱦ����ͼƬ

		timer++;	//���ƽ�ʬ������Ƶ��

		for (int i = 0; i < ZombieNumsinScene; i++)		//��Ⱦվ���Ľ�ʬ
		{
			putimagePNG(ZombiePosition[i].x, ZombiePosition[i].y, &imgZombieStand[frameIndex[i]]);
			if (timer > trembleFreq)
				frameIndex[i] = rand() % 11;	//���ˢ��ͼƬ֡��ţ���ɶ���Ч��
		}
		if (timer > trembleFreq)	timer = 0;			//ˢ�¼�����

		EndBatchDraw();		//����˫����
		Sleep(1);			//ͣ��ʱ��
	}

	//�������󻬶�
	for (int x = xMin; x <=0-112; x += speedSlide)		//���󻬶��󲻻����ף����ǻ����ʺ���ֲ�����Ұ(-112)
	{
		BeginBatchDraw();	//��ʼ����
		putimage(x, 0, &imgBackground);		//��Ⱦ����ͼƬ

		timer++;	//���ƽ�ʬ������Ƶ��

		for (int i = 0; i < ZombieNumsinScene; i++)		//��Ⱦվ���Ľ�ʬ
		{
			putimagePNG(ZombiePosition[i].x-xMin+x, ZombiePosition[i].y,&imgZombieStand[frameIndex[i]]);
			if (timer > trembleFreq)	
				frameIndex[i] = rand() % 11;	//���ˢ��ͼƬ֡��ţ���ɶ���Ч��
		}
		if (timer > trembleFreq)	timer = 0;			//ˢ�¼�����

		EndBatchDraw();		//����˫����
		Sleep(5);			//����Ѳ�������Ĳ����ٶ�
	}

	//�ر�Ѳ�������ı�������
	mciSendString("close res/res/audio/Look_up_at_the_Sky.mp3", 0, 0, 0);
}

void BarDown()
{
	int height = imgBar.getheight();
	for (int y = -height; y <= 0; y++)
	{
		BeginBatchDraw();	//��ʼ����

		putimage(-112, 0, &imgBackground);	//��Ⱦ����ͼƬ
		putimagePNG(250-112, y, &imgBar);

		for (int i = 0; i < PlantNum; i++)
		{
			int x = 338-112 + i * 65;
			putimage(x, y+6, &imgCards[i]);
		}

		EndBatchDraw();		//����˫����
		Sleep(5);			//���ƶ����Ĳ����ٶ�
	}

	//׼����ֲֲ����Ż�
	//����׼����ֲֲ�����Ч����������ټ���
	PlaySound("res/res/audio/readysetplant.wav", NULL, SND_FILENAME | SND_SYNC);
}

int main(void)
{
	GameInitialize();
	StartUI();		//������Ϸ��������
	ViewScene();	//����Ѳ������
	BarDown();		//���Ź����������Ķ���

	int timer = 0;			//��Ϸ����ʱ�ļ�ʱ��
	int timerOver = 0;		//��Ϸ�������ӳټ�ʱ��
	mciSendString("play res/res/audio/UraniwaNi.mp3 repeat", 0, 0, 0);		//ѭ��������Ϸ�ı�������
	while (1)	//��Ϸѭ��
	{
		UserClick();
		
		//ÿ���?ms����һ����Ϸ����
		timer += getDelay();
		if (timer > 10)
		{
			UpdateWindow();
			UpdateGameData();

			if (OverCheck())	//�����Ϸ���������˳���Ϸѭ��
			{
				timerOver++;
				if (timerOver > 100)		//�ӳ�һ����ʱ���ٽ�����Ϸ
					break;
			}

			timer = 0;		//��ʱ������
		}
	}

	//��Ϸ����
	Sleep(1000);
	mciSendString("close res/res/audio/UraniwaNi.mp3", 0, 0, 0);		//�ر���Ϸ�ı�������

	if (gameStatus == WIN)
	{
		loadimage(0, "res/res/win2.png");
		mciSendString("play res/res/win.mp3", 0, 0, 0);	//Ӳ�˲��ŷ�
	}
	else if (gameStatus == FAIL)
	{
		loadimage(0, "res/res/fail2.png");
		PlaySound("res/res/lose.wav", NULL, SND_FILENAME | SND_SYNC);		//����ʧ����Ч����������ټ���
		PlaySound("res/res/audio/scream.wav", NULL, SND_FILENAME | SND_SYNC);		//���ż�У���������ټ���
	}

	system("pause");
	return 0;
}

