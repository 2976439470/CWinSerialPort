// CWinSerialCom.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

# include "CWinSerialPort.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CWinSerialPort wincom;
	wincom.OpenPort(NULL,8,9600);

	char ch1[3] ={'a','b','c'};

	wincom.SendData(ch1,3);
	char ch[100];
	int length;
	while(1)
	{
		wincom.RecvData(ch,length);
		if(length !=0)
		{
			printf("length is [%d] string is [%s]",length,ch);
		}
		Sleep(100);
	}

	return 0;
}

