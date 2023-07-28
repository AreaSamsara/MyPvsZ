#include "UpdateGameData.h"

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

/*
*	ֲ�������Ϸ���ݸ���
*/

//���²�ƺ�ϵ�ֲ��״̬
void UpdatePlantinBlock()
{
	//����ÿһ���ƺ��ֲ���֡�����
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type > -1)
			{
				map[i][j].frameIndex++;
				int TypeofPlant = map[i][j].type;
				int FrameIndexofPlant = map[i][j].frameIndex;
				if (imgPlant[TypeofPlant][FrameIndexofPlant] == NULL)	//����Ѿ����ŵ����һ֡
				{
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}



/*
*	���������Ϸ���ݸ���
*/

//��Ȼ��������
void NatureCreateSunshine()
{
	static int SunshineTimer = 0;
	static int SunshineFreq = 50;
	SunshineTimer++;
	if (SunshineTimer < SunshineFreq)	//����������Ȼ���ɵ�Ƶ��
		return;

	//ˢ�¼�����
	SunshineFreq = 100 + (rand() % 150);
	SunshineTimer = 0;

	//���������ȡһ������ʹ�õ�����
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//����������������Ŀ
	int i = 0;
	for (i = 0; i < SunshineNumsinPool && sunshine[i].used; i++);
	if (i == SunshineNumsinPool)	return;		//δ�ҵ���ʹ���������˳�

	sunshine[i].used = true;
	sunshine[i].status = Sunshine_Down;		//������������Ϊ��ֱ����״̬

	sunshine[i].frameIndex = 0;		//��ʼ��������ͼƬ֡���
	sunshine[i].timer = 0;			//��ʼ��������ʧ��ʱ��

	sunshine[i].pcur.x = 260-112 + rand() % (900 - 260);	//��(260-112,900-112)���ȡx��ֵ
	sunshine[i].pcur.y = 60;

	sunshine[i].destinationY = 256 + (rand() % RowofMap) * BLOCK_HEIGHT;	//��Ŀ��λ�����ڲ�ƺ��

	//������������ֱ��صı���������
	sunshine[i].p1 = vector2(sunshine[i].pcur.x, sunshine[i].pcur.y);
	sunshine[i].p4 = vector2(sunshine[i].pcur.x, sunshine[i].destinationY);
	float distance = sunshine[i].p4.y - sunshine[i].p1.y;
	sunshine[i].speedBezier = 2.0 / distance;
	sunshine[i].time = 0;
}

//ֲ����������
void PlantCreateSunshine()
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//����������������Ŀ
	static int SunshineFreq = 20;		//���տ��״β�������Ͽ�

	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type == Sunflower)
			{
				map[i][j].sunshineTimer++;
				if (map[i][j].sunshineTimer < SunshineFreq)		continue;	//����������Ȼ���ɵ�Ƶ��

				map[i][j].sunshineTimer = 0;			//�����������ɼ�ʱ��
				SunshineFreq = 100 + (rand() % 200);	//������������Ƶ��

				//���������ȡһ������ʹ�õ�����
				int k = 0;
				for (k = 0; k < SunshineNumsinPool && sunshine[k].used; k++);
				if (k == SunshineNumsinPool)	return;		//δ�ҵ���ʹ���������˳�

				//��ʼ�����ɵ�������
				sunshine[k].used = true;
				sunshine[k].status = Sunshine_Product;		//������������Ϊ����״̬
				sunshine[k].frameIndex = 0;		//��ʼ��������ͼƬ֡���
				sunshine[k].timer = 0;			//��ʼ��������ʧ��ʱ��

				//�����������ػ���������صı���������
				int Dx = (50 + rand() % 50) * (rand() % 2 ? 1 : -1);		//�������������x������仯��ΧΪ(-150,-100)��(100,150)
				sunshine[k].p1 = vector2(map[i][j].x, map[i][j].y);
				sunshine[k].p4 = vector2(map[i][j].x + Dx, map[i][j].y + imgPlant[Sunflower][0]->getheight() - imgSunshineBall[0].getheight());
				sunshine[k].p2 = vector2(sunshine[k].p1.x + Dx * 0.3, sunshine[k].p1.y - 75);
				sunshine[k].p3 = vector2(sunshine[k].p1.x + Dx * 0.7, sunshine[k].p1.y - 75);
				float distance = sunshine[i].p4.x - sunshine[i].p1.x;
				sunshine[k].speedBezier = 0.1;		//10�����건��������
				sunshine[i].time = 0;
			}
		}
	}
}

//��������״̬
void UpdateSunshine()
{
	int SunshineNumsinPool = sizeof(sunshine) / sizeof(sunshine[0]);	//����������������Ŀ
	for (int i = 0; i < SunshineNumsinPool; i++)
	{
		if (sunshine[i].used)	//����������ڱ�ʹ��
		{
			sunshine[i].frameIndex = (sunshine[i].frameIndex + 1) % 29;	//��������ת����ͼƬ֡���

			if (sunshine[i].status == Sunshine_Down)	//�������������ֱ����
			{
				sunshine[i].time += sunshine[i].speedBezier;
				sunshine[i].pcur = sunshine[i].p1 + sunshine[i].time * (sunshine[i].p4 - sunshine[i].p1);	//pcur=p1+t*(p4-p1)
				if (sunshine[i].time >= 1)		//���������ߵ��������Ϊ���״̬
				{
					sunshine[i].status = Sunshine_Ground;
					sunshine[i].timer = 0;
				}
			}
			else if (sunshine[i].status == Sunshine_Ground)		//��������Ѿ����
			{
				sunshine[i].timer++;
				if (sunshine[i].timer > 100)	//���һ��ʱ�����ʧ
					sunshine[i].used = false;
			}
			else if (sunshine[i].status == Sunshine_Collected)	//����������ڱ��ռ�
			{
				sunshine[i].time += sunshine[i].speedBezier;
				sunshine[i].pcur = sunshine[i].p1 + sunshine[i].time * (sunshine[i].p4 - sunshine[i].p1);	//pcur=p1+t*(p4-p1)
				if (sunshine[i].time >= 1)		//���������ߵ�������ۺ�������ʧ
				{
					sunshine[i].used = false;
					SunshineAmount += 25;		//������������
				}
			}
			else if (sunshine[i].status == Sunshine_Product)		//����������ڱ�ֲ������
			{
				sunshine[i].time += sunshine[i].speedBezier;
				sunshine[i].pcur = calcBezierPoint(sunshine[i].time,
					sunshine[i].p1, sunshine[i].p2, sunshine[i].p3, sunshine[i].p4);	//���Ʊ���������

				if (sunshine[i].time >= 1)		//���������ߵ��������Ϊ���״̬
				{
					sunshine[i].status = Sunshine_Ground;
					sunshine[i].timer = 0;
				}
			}
		}
	}
}



/*
*	���е��������Ϸ���ݸ���
*/

//�����㶹�ӵ�
void Shoot()
{
	static int timer = 0;
	timer++;
	if (timer < 5)	return;		//���������㶹���ֵķ���Ƶ��
	timer = 0;

	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//��ʬ�������ʬ��Ŀ
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//�㶹�ӵ���������㶹�ӵ���Ŀ
	int thresholdX = WIN_WIDTH - imgZombie[0].getwidth() / 2;		//ֲ������ľ�����

	//�㶹���ַ����ӵ�
	for (int i = 0; i < RowofMap; i++)
	{
		for (int j = 0; j < ColumnofMap; j++)
		{
			if (map[i][j].type == Peashooter)
			{
				bool zombieExist=false;		//�жϸ�ֲ�﹥����Χ���Ƿ���ڽ�ʬ
				for (int k = 0; k < ZombieNumsinPool; k++)
				{
					//�ж�ÿһ·��û�н�ʬ�����ң�1.Խ�����ٽ��ߣ�2����ֲ��Ĺ�����Χ�ڣ�3����ֲ����ͬһ��
					if (zombies[k].used && zombies[k].x < thresholdX && zombies[k].x > map[i][j].x && zombies[k].row==i)
					{
						zombieExist = true;
						break;
					}
				}
				if (zombieExist == false)	continue;		//���û�з������������Ľ�ʬ����ֲ�ﲻ����

				map[i][j].shootTimer++;
				if (map[i][j].shootTimer < 10)	continue;	//	���Ƶ����㶹���ֵķ���Ƶ��

				map[i][j].shootTimer = 0;		//ˢ�·����ʱ��
				int k;
				for (k = 0; k < PeaBulletNumsinPool && peabullets[k].used; k++);
				if (k == PeaBulletNumsinPool)		return;		//δ�ҵ���ʹ�õ��㶹�ӵ����˳�

				peabullets[k].used = true;
				peabullets[k].row = i;
				peabullets[k].speedFly = 10;
				peabullets[k].blast = false;
				peabullets[k].frameIndex = 0;

				int PlantX = 256-112 + j * BLOCK_WIDTH;
				int PlantY = 77 + i  * BLOCK_HEIGHT + 14;
				peabullets[k].x = PlantX + imgPlant[Peashooter][0]->getwidth() - 10;
				peabullets[k].y = PlantY;		//�㶹�ӵ��������ʼλ��	

			}
		}
	}
}

//�����㶹�ӵ�״̬
void UpdatePeabullet()
{
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//�㶹�ӵ���������㶹�ӵ���Ŀ
	for (int i = 0; i < PeaBulletNumsinPool; i++)
	{
		if (peabullets[i].used)
		{
			peabullets[i].x += peabullets[i].speedFly;

			if (peabullets[i].x > WIN_WIDTH)	//���㶹�ӵ��ɳ�����
			{
				peabullets[i].used = false;		//�����㶹�ӵ�
			}

			if (peabullets[i].blast)	//���㶹�ӵ�������ը
			{
				peabullets[i].frameIndex++;		//�����㶹��ը��֡���
				if (peabullets[i].frameIndex >= 4)
				{
					peabullets[i].used = false;	//�����㶹�ӵ�
				}
			}
		}
	}
}



/*
*	��ʬ�����Ϸ���ݸ���
*/

//���ɽ�ʬ
void CreateZombie()
{
	static int ZombieTimer = 0;
	static int ZombieFreq = 50;
	ZombieTimer++;
	if (ZombieTimer < ZombieFreq)	//���ƽ�ʬ��Ȼ���ɵ�Ƶ��
		return;
	if (zombieCreated >= ZombieAmount)	//����Ѿ����������еĽ�ʬ����ֹͣ����
		return;

	//ˢ�¼�����
	ZombieFreq = 100 + (rand() % 100);
	ZombieTimer = 0;

	//�ӽ�ʬ����ȡһ������ʹ�õĽ�ʬ
	int i;
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//��ʬ�������ʬ��Ŀ
	for (i = 0; i < ZombieNumsinPool && zombies[i].used; i++);
	if (i == ZombieNumsinPool)		return;		//δ�ҵ���ʹ�õĽ�ʬ���˳�

	zombies[i].used = true;
	zombies[i].x = WIN_WIDTH;
	zombies[i].row = rand() % RowofMap;	//����ʬ��1,2,3,4,5·����
	zombies[i].y = 172 + zombies[i].row * 100;
	zombies[i].speed = 1;		//��ʬ�ƶ��ٶ�����Ϊ1
	zombies[i].HP = 100;		//��ʬ������ֵ����Ϊ100
	zombies[i].damage = 1;		//��ʬ�Ĺ����˺�����Ϊ1
	zombies[i].dead = false;	//���ý�ʬ������״̬
	zombies[i].eating = false;
	zombieCreated++;		
}

//���½�ʬ״̬
void UpdateZombie()
{
	static int timer = 0;
	timer++;
	if (timer < 2)	return;		//���Ƹ��½�ʬ���ݵ�Ƶ��
	timer = 0;

	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//��ʬ�������ʬ��Ŀ
	for (int i = 0; i < ZombieNumsinPool; i++)
	{
		if (zombies[i].used)
		{
			if (zombies[i].dead)
			{
				//���½�ʬ����������ͼƬ֡���
				zombies[i].frameIndex++;
				if (zombies[i].frameIndex >= 20)	//������������������ս�ʬ
					zombies[i].used = false;
			}
			else if (zombies[i].eating)
			{
				//���½�ʬ��ֲ�ﶯ����ͼƬ֡���
				zombies[i].frameIndex = (zombies[i].frameIndex + 1) % 21;
			}
			else
			{
				//���½�ʬ��λ��
				zombies[i].x -= zombies[i].speed;	//��ʬ�ƶ�
				if (zombies[i].x < 170-112)				//�����ʬ���˼��ţ���Ϸʧ��
				{
					gameStatus = FAIL;
				}

				//���½�ʬ�ƶ�������ͼƬ֡���
				zombies[i].frameIndex = (zombies[i].frameIndex + 1) % 22;
			}
		}
	}
}



/*
*	�¼������Ϸ���ݸ���
*/

//����Ƿ����¼�����
void IncidentCheck()
{
	CollisionCheck();	//����Ƿ����㶹�ӵ����н�ʬ
	EatCheck();			//����Ƿ��н�ʬ׼����ֲ��
}

//����Ƿ����㶹�ӵ����н�ʬ
void CollisionCheck()
{
	int PeaBulletNumsinPool = sizeof(peabullets) / sizeof(peabullets[0]);	//�㶹�ӵ���������㶹�ӵ���Ŀ
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//��ʬ�������ʬ��Ŀ
	for (int i = 0; i < PeaBulletNumsinPool; i++)
	{
		//ɸѡ��û��Ҫ�������
		if (peabullets[i].used == false || peabullets[i].blast)
			continue;

		for (int j = 0; j < ZombieNumsinPool; j++)
		{
			//ɸѡ��û��Ҫ�������
			if (zombies[j].used == false || zombies[j].dead || peabullets[i].row != zombies[j].row)
				continue;

			int x1 = zombies[j].x + 70;
			int x2 = zombies[j].x + 110;
			int x = peabullets[i].x;
			if (x > x1 && x < x2)
			{
				PlaySound("res/res/audio/splat1.wav", NULL, SND_FILENAME | SND_ASYNC);	//�����㶹�ӵ����е���Ч
				zombies[j].HP -= 10;		//��ʬ�ܵ��˺�
				peabullets[i].blast = true;
				peabullets[i].speedFly = 0;		//ʹ�㶹�ӵ�ֹͣ
				
				if (zombies[j].HP <= 0)		//�ж���ʬ�Ƿ�����
				{
					zombies[j].dead = true;
					zombies[j].speed = 0;
					zombies[j].frameIndex = 0;	//����ͼƬ֡ʹ֮Ϊ��������
					killcount++;				//�����ɱ�Ľ�ʬ��Ŀ

					if (killcount >= ZombieAmount)		//��ɱ���еĽ�ʬ����Ϸʤ��
						gameStatus = WIN;
				}
				break;	//�㶹�ӵ��ѱ����ģ�ֱ�ӽ�����һ���ӵ����ж�
			}
		}
	}
}

//����Ƿ��н�ʬ׼����ֲ��
void EatCheck()
{
	int ZombieNumsinPool = sizeof(zombies) / sizeof(zombies[0]);	//��ʬ�������ʬ��Ŀ
	for (int i = 0; i < ZombieNumsinPool; i++)
	{
		if (zombies[i].used == false || zombies[i].dead)	continue;	//ɸѡ��û��Ҫ�������
		int row = zombies[i].row;
		for (int j = ColumnofMap - 1; j >= 0; j--)
		{
			int x0 = 256-112 + j * 81;		//ֲ���x����
			int x1 = x0 + 10;
			int x2 = x0 + 60;
			int x = zombies[i].x + 80;		//��ʬ��ֲ�����Чx����

			if (x > x1 && x < x2)		//�����ʬ�ڳ�ֲ��ķ�Χ��
			{
				if (map[row][j].type == -1)		//���ô�û��ֲ��
				{
					if (zombies[i].eating)
					zombies[i].eating = false;		//��Ҫ�����Կ�����
					continue;					//û��ֲ��������
				}

				if (zombies[i].eating == false)		//�����ʬ��׼����
				{
					//ѭ�����Ž�ʬ��ֲ�����Ч
					mciSendString("play res/res/audio/chompsoft.mp3 repeat", 0, 0, 0);

					zombies[i].eating = true;
					zombies[i].speed = 0;
					map[row][j].eaten = true;
				}
				
				map[row][j].HP -= zombies[i].damage;

				if (map[row][j].HP <= 0)	//���ֲ�ﱻ�Ե�
				{
					//�رս�ʬ��ֲ�����Ч
					mciSendString("close res/res/audio/chompsoft.mp3", 0, 0, 0);
					map[row][j].type = -1;
					map[row][j].eaten = false;
					zombies[i].eating = false;
					zombies[i].frameIndex = 0;	//���ý�ʬͼƬ֡���
					zombies[i].speed = 1;
				}
			}
		}
	}
}

//�����Ϸ�Ƿ����
bool OverCheck()
{
	return gameStatus == WIN || gameStatus == FAIL;
}