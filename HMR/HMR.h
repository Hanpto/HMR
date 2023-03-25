#pragma once
#include"MmLoadDll.h"

//�ȸ���Ϣ���ȸ�ģ��·�����汾�ļ�·��
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


//�汾���¼��
DWORD checkVersionProc(LPVOID Param);

//�ڴ涯̬����dll
int  LoadDll(char* dllPath);

//�ȸ�ģ��
void HMR(int initVersion, char* modulePath, char* versionFile);
