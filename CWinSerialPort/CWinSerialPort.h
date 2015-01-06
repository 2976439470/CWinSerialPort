/*
**	FILENAME			CWinSerialPort.h
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**
***************************************************************************************
**  author: mrlong date:2007-12-25
**
**  改进
**	1) 增加 ClosePort
**	2) 增加 WriteToPort 两个方法
**	3) 增加 SendData 与 RecvData 方法
***************************************************************************************
***************************************************************************************
**  author：liquanhai date:2011-11-06
**
**  改进
**	1) 增加 ClosePort 中交出控制权，防止死锁问题
**	2) 增加 ReceiveChar 中防止线程死锁
***************************************************************************************
***************************************************************************************
**  author：viruscamp date:2013-12-04
**
**  改进
**	1) 增加 IsOpen 判断是否打开
**	2) 修正 InitPort 中 parity Odd Even 参数取值错误
**	3) 修改 InitPort 中 portnr 取值范围，portnr>9 时特殊处理
**	4) 取消对 MFC 的依赖，使用 HWND 替代 CWnd，使用 win32 thread 函数而不是 MFC 的
**	5) 增加用户消息编号自定义，方法来自 CnComm
***************************************************************************************
***************************************************************************************
**  author：ACOSSATD date:20150106
**
**  改进
**	1) 增加windows.h文件，可以在win32控制台直接调用
**	2) 增加openport函数，可以通过单个函数打开串口，不必再  init+startmonitor
**	3) 增加receivebuf，可以存储thread监听到的数据
**	4) 添加m_nReadBufferSize
**	5) 在ReceiveChar函数中，将接收的字符存放在receivebuf中，同时向上发送消息，以可以进行消息响应
**	6) 修改了RecvData，直接将消息放到参数对应的数组中，长度放到参数length中
**	7) 添加GetReadBufferSize函数
**	8) Re
**	9)

***************************************************************************************
*/

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

# include <windows.h>

#ifndef WM_COMM_MSG_BASE 
	#define WM_COMM_MSG_BASE		WM_USER + 617		//!< 消息编号的基点  
#endif

#define WM_COMM_BREAK_DETECTED		WM_COMM_MSG_BASE + 1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_COMM_MSG_BASE + 2	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_COMM_MSG_BASE + 3	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_COMM_MSG_BASE + 4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_COMM_MSG_BASE + 5	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_COMM_MSG_BASE + 6	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_COMM_MSG_BASE + 7	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_COMM_MSG_BASE + 8	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_COMM_MSG_BASE + 9	// The last character in the output buffer was sent.  

class CWinSerialPort
{														 
public:
	// contruction and destruction
	CWinSerialPort();
	virtual		~CWinSerialPort();

	// add by ACOSSATD 2015-01-06        open the port with one function
	BOOL		OpenPort(HWND pPortOwner, UINT portnr = 1, UINT baud = 19200, 
						char parity = 'N', UINT databits = 8, UINT stopsbits = 1, 
						DWORD dwCommEvents = EV_RXCHAR | EV_CTS,
						UINT nWriteBufferSize = 512, UINT nReadBufferSize = 512,

						DWORD ReadIntervalTimeout = 1000,
						DWORD ReadTotalTimeoutMultiplier = 1000,
						DWORD ReadTotalTimeoutConstant = 1000,
						DWORD WriteTotalTimeoutMultiplier = 1000,
						DWORD WriteTotalTimeoutConstant = 1000);
	void		ClosePort();					 // add by mrlong 2007-12-2  
	BOOL		IsOpen();


	// port initialisation											
	BOOL		InitPort(HWND pPortOwner, UINT portnr = 1, UINT baud = 19200, 
				char parity = 'N', UINT databits = 8, UINT stopsbits = 1, 
				DWORD dwCommEvents = EV_RXCHAR | EV_CTS, 
				UINT nWriteBufferSize = 512, UINT nReadBufferSize = 512,

				DWORD ReadIntervalTimeout = 1000,
				DWORD ReadTotalTimeoutMultiplier = 1000,
				DWORD ReadTotalTimeoutConstant = 1000,
				DWORD WriteTotalTimeoutMultiplier = 1000,
				DWORD WriteTotalTimeoutConstant = 1000);

	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetReadBufferSize();

	DWORD		GetCommEvents();
	DCB			GetDCB();

	void		WriteToPort(char* string);
	void		WriteToPort(char* string,int n); // add by mrlong 2007-12-25
	void		WriteToPort(LPCTSTR string);	 // add by mrlong 2007-12-25
	void		WriteToPort(BYTE* Buffer, int n);// add by mrlong


	void SendData(LPCTSTR lpszData, const int nLength);   //串口发送函数 by mrlong 2008-2-15
//	BOOL RecvData(LPTSTR lpszData, const int nSize);	  //串口接收函数 by mrlong 2008-2-15
	BOOL RecvData(LPTSTR lpszData, int &nSize);			  //串口接收函数 by ACOSSATD 2015-01-06

protected:
	// protected memberfunctions
	void		ProcessErrorMessage(char* ErrorText);
	static DWORD WINAPI CommThread(LPVOID pParam);
	static void	ReceiveChar(CWinSerialPort* port);
	static void	WriteChar(CWinSerialPort* port);

	// thread
	//CWinThread*			m_Thread;
	HANDLE			  m_Thread;

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	BOOL				m_bThreadAlive;

	// handles
	HANDLE				m_hShutdownEvent;  //stop发生的事件
	HANDLE				m_hComm;		   // read  
	HANDLE				m_hWriteEvent;	 // write

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;

	// owner window
	//CWnd*				m_pOwner;
	HWND				m_pOwner;


	// misc
	UINT				m_nPortNr;		//com port number
	char*				m_szWriteBuffer;
	char*				m_szReadBuffer;

	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;					// 写缓冲区大小
	DWORD				m_nReadBufferSize;					// 读缓冲区大小

	int				 m_nWriteSize; //add by mrlong 2007-12-25		实际需要写入字节数量
	int				 m_nReadSize; //add by ACOSSATD 2015-01-06		实际读取字节数量

};

#endif __SERIALPORT_H__
