#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)

#include "Global.h"

#include <stdio.h>
#include <graphics.h>
#include <time.h>
#include <math.h>
#include "tools.h"
#include "vector2.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 900
#define WIN_HEIGHT 600
#define BLOCK_HEIGHT 102
#define BLOCK_WIDTH 81
#define RowofMap 5
#define ColumnofMap 9
#define ZombieAmount 10		//һ����Ϸ�Ľ�ʬ����

struct Plant 
{
	int kind;	//ֲ�������
	int cost;	//��ֲ��ֲ����������
	int HP;
};

struct PlantinBlock
{
	int type;		//-1����ʾû��ֲ�n����ʾö�������еĵ�n��ֲ��
	int frameIndex;	//����֡�����
	bool eaten;
	int HP;
	int x, y;		//ֲ�����ڵ�λ��
	int sunshineTimer;		//�������ɼ�ʱ�����������տ�
	int shootTimer;		//�ӵ������ʱ���������㶹����
};

struct SunshineBall
{
	int frameIndex;			//��ǰ��ʾ��ͼƬ֡�����
	int destinationY;		//Ʈ��Ŀ��λ�õ�y����
	bool used;				//�Ƿ����ڱ�ʹ��
	int timer;				//������ʧ��ʱ������غ�ʼ��ʱ��
	float vector_x;			//ָ������۵ĵ�λ������ˮƽ����
	float vector_y;			//ָ������۵ĵ�λ��������ֱ����

	float speedBezier;		//������ı������ٶ�
	float time;		//���������ߵ�ʱ��㣬��(0,1)
	vector2 p1, p2, p3, p4;	//���������ߵ��ĸ���������
	vector2 pcur;	//��ǰʱ�̵�λ������
	int status;		//��ʾ������ǰ״̬
};

struct Zombie
{
	int x, y;
	int frameIndex;
	bool used;
	int speed;
	int row,col;
	int HP;
	int damage;
	bool dead;
	bool eating;
};

struct PeaBullet
{
	int x, y;
	int row;
	bool used;
	int speedFly;		//�ӵ����������е��ٶ�
	bool blast;		//�ӵ��Ƿ����˱�ը
	int frameIndex;		//�㶹�ӵ���ը״̬�µ�֡���
};
