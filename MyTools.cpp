# include "MyTools.h"

//�ж��ļ��Ƿ����
bool fileExist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp == NULL)	return false;
	fclose(fp);
	return true;
}