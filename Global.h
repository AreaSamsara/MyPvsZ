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
#define ZombieAmount 10		//一局游戏的僵尸总量

struct Plant 
{
	int kind;	//植物的种类
	int cost;	//种植该植物所需阳光
	int HP;
};

struct PlantinBlock
{
	int type;		//-1：表示没有植物；n：表示枚举类型中的第n种植物
	int frameIndex;	//序列帧的序号
	bool eaten;
	int HP;
	int x, y;		//植物所在的位置
	int sunshineTimer;		//阳光生成计时器，用于向日葵
	int shootTimer;		//子弹发射计时器，用于豌豆射手
};

struct SunshineBall
{
	int frameIndex;			//当前显示的图片帧的序号
	int destinationY;		//飘落目标位置的y坐标
	bool used;				//是否正在被使用
	int timer;				//阳光消失计时器（落地后开始计时）
	float vector_x;			//指向阳光槽的单位向量的水平分量
	float vector_y;			//指向阳光槽的单位向量的竖直分量

	float speedBezier;		//阳光球的贝塞尔速度
	float time;		//贝塞尔曲线的时间点，∈(0,1)
	vector2 p1, p2, p3, p4;	//贝塞尔曲线的四个控制向量
	vector2 pcur;	//当前时刻的位移向量
	int status;		//表示阳光球当前状态
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
	int speedFly;		//子弹被发射后飞行的速度
	bool blast;		//子弹是否发生了爆炸
	int frameIndex;		//豌豆子弹爆炸状态下的帧序号
};
