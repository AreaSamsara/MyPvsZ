# include "MyTools.h"

//判断文件是否存在
bool fileExist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp == NULL)	return false;
	fclose(fp);
	return true;
}