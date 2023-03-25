#include"HMR.h"
#include<iostream>
#include<Windows.h>
using namespace std;

LPVOID g_lpBaseAddress = NULL;	//动态加载的模块及地址
int g_curVersion = 1;
HMRInfo hmrInfo; 

//动态加载Dll
int  LoadDll(char* dllPath)	
{
	HANDLE hFile = CreateFileA(dllPath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ShowError((CHAR*)"CreateFile");
		return 1;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	
	// 申请动态内存并读取DLL到内存中
	BYTE* lpData = new BYTE[dwFileSize];
	if (NULL == lpData)
	{
		ShowError((CHAR*)"new");
		return 2;
	}
	
	DWORD dwRet = 0;
	ReadFile(hFile, lpData, dwFileSize, &dwRet, NULL);

	// 将内存DLL加载到程序中
	g_lpBaseAddress = MmLoadLibrary(lpData, dwFileSize);
	if (NULL == g_lpBaseAddress)
	{
		ShowError((CHAR*)"MmLoadLibrary");
		return 3;
	}
	
	CloseHandle(hFile);
	delete[] lpData;
}


//版本检测线程回调函数
DWORD checkVersionProc(LPVOID Param)
{
	
	while (1)
	{
		//获取dll版本
		FILE* fp = NULL;
		fopen_s(&fp,hmrInfo.versionFile, "r");
		char tips[20];
		int version;
		fscanf(fp, "%s %d", tips, &version);

		if (version > g_curVersion)//如果版本有更新
		{
			printf("find new version. version code:%d\n", version);
			g_curVersion = version;
			
			/*卸载原有dll*/
			if (g_lpBaseAddress)
			{
				FARPROC UnLoadDLL = (FARPROC)MmGetProcAddress(g_lpBaseAddress, (PCHAR)"UnloadDLL");
				UnLoadDLL();					//释放dll中所占用的资源
				MmFreeLibrary(g_lpBaseAddress); //释放dll在进程空间中使用的内存
				g_lpBaseAddress = NULL;
			}

			/*加载新版本的dll*/
			LoadDll(hmrInfo.moudlePath);
		}
		else
		{
			printf("%s %d\n", tips, version);
		}
		fclose(fp);
		Sleep(10000);
	}

}


//热更新接口
void HMR(int initVersion, char* modulePath, char* versionFile)
{
	g_curVersion = initVersion;
	strcpy(hmrInfo.moudlePath, modulePath);
	strcpy(hmrInfo.versionFile, versionFile);

	LoadDll(modulePath);
	
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)checkVersionProc, &hmrInfo, 0, NULL);
	
	Sleep(1000);//确保线程被完整创建并运行
}
