#pragma once
#include "Global.h"

//���²�ƺ�ϵ�ֲ��״̬
void UpdatePlantinBlock();

//��Ȼ��������
void NatureCreateSunshine();

//ֲ����������
void PlantCreateSunshine();	

//��������״̬
void UpdateSunshine();

//�����㶹�ӵ�
void Shoot();

//�����㶹�ӵ�״̬
void UpdatePeabullet();

//���ɽ�ʬ
void CreateZombie();

//���½�ʬ״̬
void UpdateZombie();

//����Ƿ����¼�����
void IncidentCheck();

//����Ƿ����㶹�ӵ����н�ʬ
void CollisionCheck();	

//����Ƿ��н�ʬ׼����ֲ��
void EatCheck();		

//�����Ϸ�Ƿ����
bool OverCheck();