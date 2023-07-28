#pragma once
#include "Global.h"

//更新草坪上的植物状态
void UpdatePlantinBlock();

//自然生成阳光
void NatureCreateSunshine();

//植物生成阳光
void PlantCreateSunshine();	

//更新阳光状态
void UpdateSunshine();

//发射豌豆子弹
void Shoot();

//更新豌豆子弹状态
void UpdatePeabullet();

//生成僵尸
void CreateZombie();

//更新僵尸状态
void UpdateZombie();

//检测是否有事件发生
void IncidentCheck();

//检测是否有豌豆子弹命中僵尸
void CollisionCheck();	

//检测是否有僵尸准备吃植物
void EatCheck();		

//检测游戏是否结束
bool OverCheck();