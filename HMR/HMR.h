#pragma once
#include"MmLoadDll.h"

//热更信息：热更模块路径，版本文件路径
struct HMRInfo
{
	char moudlePath[MAX_PATH];
	char versionFile[MAX_PATH];
	
	HMRInfo(){}
	HMRInfo(char* _modulePath,char* _versionFile)
	{
		memcpy(moudlePath,_modulePath, MAX_PATH);
		memcpy(versionFile, _versionFile, MAX_PATH);
	}
};


//版本更新检查
DWORD checkVersionProc(LPVOID Param);

//内存动态加载dll
int  LoadDll(char* dllPath);

//热更模块
void HMR(int initVersion, char* modulePath, char* versionFile);
