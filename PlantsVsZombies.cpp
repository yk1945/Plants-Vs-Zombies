#include <Windows.h>  
#include <stdio.h>  



int main()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	//打开进程访问令牌
	/*
	如果函数成功，返回值为非零值。
	如果函数失败，返回值为零。 
	*/
	if (!OpenProcessToken(
		GetCurrentProcess(),					//进程句柄
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,	//访问令牌对象的访问权限。 
		&hToken))								//指向句柄的指针，当函数返回时标识新打开的访问令牌。 
	{
		return 0;
	}

	//查看系统权限的特权值，返回信息到一个LUID结构体里
	//第一个参数：表示所要查看的系统，本地系统直接用NULL
	//第二个参数：指向一个以零结尾的字符串，指定特权的名称，
	//第三个参数：用来接收所返回的制定特权名称的信息。
	//函数调用成功后，信息存入第三个类型为LUID的结构体中，并且函数返回非0。
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return 0;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	//这个函数启用或禁止 指定访问令牌的特权
	/*
	HANDLE TokenHandle, //包含特权的句柄
	BOOL DisableAllPrivileges,//禁用所有权限标志
	PTOKEN_PRIVILEGES NewState,//新特权信息的指针(结构体)
	DWORD BufferLength, //缓冲数据大小,以字节为单位的PreviousState的缓存区(sizeof)
	PTOKEN_PRIVILEGES PreviousState,//接收被改变特权当前状态的Buffer
	PDWORD ReturnLength //接收PreviousState缓存区要求的大小
	*/
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return 0;
	}

	DWORD  getLastError;
	//找到窗口  
	HWND hWinmine = FindWindowW(L"MainWindow", L"植物大战僵尸中文版");
	DWORD dwPID = 0;
	//检索创建指定窗口的线程的标识符，以及可选的创建窗口进程的标识符。
	//参数一：窗口句柄
	//参数二：进程ID
	//返回值：返回值是创建窗口的线程ID。 
	GetWindowThreadProcessId(hWinmine, &dwPID); //获取进程标识  
	if (dwPID == 0)
	{
		printf_s("获取PID失败\n");
		return -1;
	}


	//打开一个进程
	//参数一：进程访问权限。
	//参数二：如果此值为TRUE，则由此进程创建的进程将继承句柄。否则，进程不继承此句柄。
	//参数三：要打开的本地进程的进程ID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		printf_s("进程打开失败\n");
		getLastError = GetLastError(); 
		return -1;
	}

	DWORD dwNum = 0, dwSize = 0;

	//基址  
	DWORD SunShineBaseAddress = 0x006A9EC0;
	//基址值  
	DWORD SunShineBaseAddressValue = 0;
	//从指定进程的内存区域读取数据。要读取的整个区域必须可访问或操作失败。 
	//第一个参数：带有正在读取的内存的进程的句柄。
	//第二个参数：指向指定进程中要读取的基址的指针
	//第三个参数：指向缓冲区的指针，该缓冲区从指定进程的地址空间接收内容
	//第四个参数：要从指定进程读取的字节数。 
	//第五个参数：指向一个变量的指针，该变量接收传输到指定缓冲区中的字节数
	//成功：非零；失败返回NULL
	if (NULL == ReadProcessMemory(
			hProcess, 
			(LPVOID)SunShineBaseAddress, 
			&SunShineBaseAddressValue, 
			sizeof(DWORD), 
			&dwSize))
	{
		printf_s("静态址获取失败\n");
		getLastError = GetLastError();
		return -1;
	}
	//一级偏移  
	DWORD SunShineOffsetFirst = 0x768;
	//一级偏移值  
	DWORD SunShineOffsetFirstValue = 0;

	if (0 == ReadProcessMemory(hProcess, (LPVOID)(SunShineBaseAddressValue + SunShineOffsetFirst), &SunShineOffsetFirstValue, sizeof(DWORD), &dwSize))
	{
		printf_s("一级偏移获取失败\n");
		getLastError = GetLastError();
		return -1;
	}

	//二级偏移  
	DWORD SunShineOffsetSecond = 0x5560;
	//最后值  
	DWORD SunShineNum = 0;
	if (0 == ReadProcessMemory(hProcess, (LPVOID)(SunShineOffsetFirstValue + SunShineOffsetSecond), &SunShineNum, sizeof(DWORD), &dwSize))
	{
		printf_s("二级偏移获取失败\n");
		getLastError = GetLastError();
		return -1;
	}
	int modifySunshine;
	printf_s("SunShineNum:%d\n", SunShineNum);
	printf_s("输入你要修改后的值：");
	scanf_s("%d", &modifySunshine);

	//将数据写入指定进程中的内存区域。要写入的整个区域必须可访问或操作失败。 
	//第一个参数:要修改的进程内存的句柄。
	//第二个参数：指向写入数据的指定进程中的基地址的指针。
	//第三个参数：指向缓冲区的指针
	//第四个参数：要写入指定进程的字节数
	//第五个参数：指向一个变量的指针，该变量接收传输到指定进程的字节数。
	WriteProcessMemory(
					hProcess, 
					(LPVOID)(SunShineOffsetFirstValue + SunShineOffsetSecond),
					&modifySunshine, 
					sizeof(DWORD),
					&dwSize);


	CloseHandle(hProcess);
	system("pause");
	return 0;
}