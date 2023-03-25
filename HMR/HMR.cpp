#include"HMR.h"
#include<iostream>
#include<Windows.h>
using namespace std;

LPVOID g_lpBaseAddress = NULL;	//��̬���ص�ģ�鼰��ַ
int g_curVersion = 1;
HMRInfo hmrInfo; 

//��̬����Dll
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
	
	// ���붯̬�ڴ沢��ȡDLL���ڴ���
	BYTE* lpData = new BYTE[dwFileSize];
	if (NULL == lpData)
	{
		ShowError((CHAR*)"new");
		return 2;
	}
	
	DWORD dwRet = 0;
	ReadFile(hFile, lpData, dwFileSize, &dwRet, NULL);

	// ���ڴ�DLL���ص�������
	g_lpBaseAddress = MmLoadLibrary(lpData, dwFileSize);
	if (NULL == g_lpBaseAddress)
	{
		ShowError((CHAR*)"MmLoadLibrary");
		return 3;
	}
	
	CloseHandle(hFile);
	delete[] lpData;
}


//�汾����̻߳ص�����
DWORD checkVersionProc(LPVOID Param)
{
	
	while (1)
	{
		//��ȡdll�汾
		FILE* fp = NULL;
		fopen_s(&fp,hmrInfo.versionFile, "r");
		char tips[20];
		int version;
		fscanf(fp, "%s %d", tips, &version);

		if (version > g_curVersion)//����汾�и���
		{
			printf("find new version. version code:%d\n", version);
			g_curVersion = version;
			
			/*ж��ԭ��dll*/
			if (g_lpBaseAddress)
			{
				FARPROC UnLoadDLL = (FARPROC)MmGetProcAddress(g_lpBaseAddress, (PCHAR)"UnloadDLL");
				UnLoadDLL();					//�ͷ�dll����ռ�õ���Դ
				MmFreeLibrary(g_lpBaseAddress); //�ͷ�dll�ڽ��̿ռ���ʹ�õ��ڴ�
				g_lpBaseAddress = NULL;
			}

			/*�����°汾��dll*/
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


//�ȸ��½ӿ�
void HMR(int initVersion, char* modulePath, char* versionFile)
{
	g_curVersion = initVersion;
	strcpy(hmrInfo.moudlePath, modulePath);
	strcpy(hmrInfo.versionFile, versionFile);

	LoadDll(modulePath);
	
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)checkVersionProc, &hmrInfo, 0, NULL);
	
	Sleep(1000);//ȷ���̱߳���������������
}
