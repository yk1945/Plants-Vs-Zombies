#include <Windows.h>  
#include <stdio.h>  



int main()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	//�򿪽��̷�������
	/*
	��������ɹ�������ֵΪ����ֵ��
	�������ʧ�ܣ�����ֵΪ�㡣 
	*/
	if (!OpenProcessToken(
		GetCurrentProcess(),					//���̾��
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,	//�������ƶ���ķ���Ȩ�ޡ� 
		&hToken))								//ָ������ָ�룬����������ʱ��ʶ�´򿪵ķ������ơ� 
	{
		return 0;
	}

	//�鿴ϵͳȨ�޵���Ȩֵ��������Ϣ��һ��LUID�ṹ����
	//��һ����������ʾ��Ҫ�鿴��ϵͳ������ϵͳֱ����NULL
	//�ڶ���������ָ��һ�������β���ַ�����ָ����Ȩ�����ƣ�
	//�������������������������ص��ƶ���Ȩ���Ƶ���Ϣ��
	//�������óɹ�����Ϣ�������������ΪLUID�Ľṹ���У����Һ������ط�0��
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return 0;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	//����������û��ֹ ָ���������Ƶ���Ȩ
	/*
	HANDLE TokenHandle, //������Ȩ�ľ��
	BOOL DisableAllPrivileges,//��������Ȩ�ޱ�־
	PTOKEN_PRIVILEGES NewState,//����Ȩ��Ϣ��ָ��(�ṹ��)
	DWORD BufferLength, //�������ݴ�С,���ֽ�Ϊ��λ��PreviousState�Ļ�����(sizeof)
	PTOKEN_PRIVILEGES PreviousState,//���ձ��ı���Ȩ��ǰ״̬��Buffer
	PDWORD ReturnLength //����PreviousState������Ҫ��Ĵ�С
	*/
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return 0;
	}

	DWORD  getLastError;
	//�ҵ�����  
	HWND hWinmine = FindWindowW(L"MainWindow", L"ֲ���ս��ʬ���İ�");
	DWORD dwPID = 0;
	//��������ָ�����ڵ��̵߳ı�ʶ�����Լ���ѡ�Ĵ������ڽ��̵ı�ʶ����
	//����һ�����ھ��
	//������������ID
	//����ֵ������ֵ�Ǵ������ڵ��߳�ID�� 
	GetWindowThreadProcessId(hWinmine, &dwPID); //��ȡ���̱�ʶ  
	if (dwPID == 0)
	{
		printf_s("��ȡPIDʧ��\n");
		return -1;
	}


	//��һ������
	//����һ�����̷���Ȩ�ޡ�
	//�������������ֵΪTRUE�����ɴ˽��̴����Ľ��̽��̳о�������򣬽��̲��̳д˾����
	//��������Ҫ�򿪵ı��ؽ��̵Ľ���ID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		printf_s("���̴�ʧ��\n");
		getLastError = GetLastError(); 
		return -1;
	}

	DWORD dwNum = 0, dwSize = 0;

	//��ַ  
	DWORD SunShineBaseAddress = 0x006A9EC0;
	//��ֵַ  
	DWORD SunShineBaseAddressValue = 0;
	//��ָ�����̵��ڴ������ȡ���ݡ�Ҫ��ȡ�������������ɷ��ʻ����ʧ�ܡ� 
	//��һ���������������ڶ�ȡ���ڴ�Ľ��̵ľ����
	//�ڶ���������ָ��ָ��������Ҫ��ȡ�Ļ�ַ��ָ��
	//������������ָ�򻺳�����ָ�룬�û�������ָ�����̵ĵ�ַ�ռ��������
	//���ĸ�������Ҫ��ָ�����̶�ȡ���ֽ����� 
	//�����������ָ��һ��������ָ�룬�ñ������մ��䵽ָ���������е��ֽ���
	//�ɹ������㣻ʧ�ܷ���NULL
	if (NULL == ReadProcessMemory(
			hProcess, 
			(LPVOID)SunShineBaseAddress, 
			&SunShineBaseAddressValue, 
			sizeof(DWORD), 
			&dwSize))
	{
		printf_s("��ַ̬��ȡʧ��\n");
		getLastError = GetLastError();
		return -1;
	}
	//һ��ƫ��  
	DWORD SunShineOffsetFirst = 0x768;
	//һ��ƫ��ֵ  
	DWORD SunShineOffsetFirstValue = 0;

	if (0 == ReadProcessMemory(hProcess, (LPVOID)(SunShineBaseAddressValue + SunShineOffsetFirst), &SunShineOffsetFirstValue, sizeof(DWORD), &dwSize))
	{
		printf_s("һ��ƫ�ƻ�ȡʧ��\n");
		getLastError = GetLastError();
		return -1;
	}

	//����ƫ��  
	DWORD SunShineOffsetSecond = 0x5560;
	//���ֵ  
	DWORD SunShineNum = 0;
	if (0 == ReadProcessMemory(hProcess, (LPVOID)(SunShineOffsetFirstValue + SunShineOffsetSecond), &SunShineNum, sizeof(DWORD), &dwSize))
	{
		printf_s("����ƫ�ƻ�ȡʧ��\n");
		getLastError = GetLastError();
		return -1;
	}
	int modifySunshine;
	printf_s("SunShineNum:%d\n", SunShineNum);
	printf_s("������Ҫ�޸ĺ��ֵ��");
	scanf_s("%d", &modifySunshine);

	//������д��ָ�������е��ڴ�����Ҫд��������������ɷ��ʻ����ʧ�ܡ� 
	//��һ������:Ҫ�޸ĵĽ����ڴ�ľ����
	//�ڶ���������ָ��д�����ݵ�ָ�������еĻ���ַ��ָ�롣
	//������������ָ�򻺳�����ָ��
	//���ĸ�������Ҫд��ָ�����̵��ֽ���
	//�����������ָ��һ��������ָ�룬�ñ������մ��䵽ָ�����̵��ֽ�����
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