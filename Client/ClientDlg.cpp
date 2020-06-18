﻿// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include"ClientPrivateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg dialog




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientDlg::IDD, pParent)
, m_msgString(_T(""))
, IP(_T(""))
, m_userName(_T(""))
, m_Pass(_T(""))
, m_msgSend(_T(""))

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MSGBOX, m_msgString);
	DDX_Text(pDX, IDC_USER, m_userName);
	DDX_Text(pDX, IDC_PASS, m_Pass);
	DDX_Text(pDX, IDC_MESSAGE, m_msgSend);
	DDX_Text(pDX, IDC_IPADDR, IP);
	DDX_Control(pDX, IDC_LIST1, m_listClient);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE (WM_SOCKET, SockMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGIN, &CClientDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_LOGOUT, &CClientDlg::OnBnClickedLogout)
	ON_EN_CHANGE(IDC_MSGBOX, &CClientDlg::OnEnChangeMsgbox)
	ON_BN_CLICKED(IDC_CANCEL, &CClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_SIGNUP, &CClientDlg::OnBnClickedSignup)
	ON_LBN_SELCHANGE(IDC_LIST1, &CClientDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI RunClientDlg(LPVOID lp)
{
	CClientDlg dlg;
	//m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return 0;
}

void CClientDlg::Split(CString src, CString des[3])
{
	int p1, p2, p3;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);
	
	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

	p3 = src.Find(_T("\r\n"), p2 + 1);
	des[2] = src.Mid(p2 + 2, p3 - (p2 + 2));
}

char* CClientDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize+1];
	memset(pAnsiString,0,nSize+1);
	wcstombs(pAnsiString, s, nSize+1);
	return pAnsiString;
}

void CClientDlg::mSend(CString Command)
{
	int Len=Command.GetLength();
	Len+=Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff,0,1000);
	memcpy(sendBuff,(PBYTE)(LPCTSTR)Command, Len);
	send(sClient,(char*)&Len,sizeof(Len),0);
	send(sClient,(char*)sendBuff,Len,0);
	delete sendBuff;
}

int CClientDlg::mRecv(CString &Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer,0, 1000);
	recv(sClient ,(char*)&buffLength,sizeof(int),0);
	recv(sClient,(char*)buffer,buffLength,0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;

	if(Command.GetLength()==0)
		return -1 ;
	return 0;
}

LRESULT CClientDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{

	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		CString temp;
		if (mRecv(temp) < 0)
			break;

		Split(temp, strResult);
		int flag1 = _ttoi(strResult[0]);
		switch (flag1)
		{
			case 1:
			{
				int flag2 = _ttoi(strResult[1]);
				if (flag2 == 1)
				{
					//nếu tên trong thông điệp trùng với tên của CLient thì thông báo là "Đăng nhập thành công"
					if (strResult[2] == m_userName)
					{
						m_msgString = _T("");
						UpdateData(FALSE);
						m_msgString += _T("Dang nhap thanh cong\r\n");
						m_Pass = _T("");
						this->GetDlgItem(IDC_LOGOUT)->EnableWindow(1);
						GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);
						GetDlgItem(IDC_SIGNUP)->EnableWindow(FALSE);
						UpdateData(FALSE);
					}
					//còn ngược lại thì  thông báo "<Tên Client> login"
					else
					{
						m_msgString += strResult[2];
						m_msgString += _T(" logined\r\n");
											
					}			
					CString name(strResult[2]);
					m_listClient.AddString(name);
				}	
				if (flag2 == 0)
				{
					m_msgString += _T("Dang nhap that bai\r\n");
					MessageBox(_T("Dang nhap that bai\r\n"), _T("FAIL"),MB_ICONWARNING);
					GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
				}
				
				UpdateData(FALSE);
				
			}break;
			case 2://Thông báo đăng ký thành công hay không thành công
			{
				int flag2 = _ttoi(strResult[1]);
				if (flag2 == 0)
				{
					MessageBox(_T("UNSUCCESSFUL \r\n"), _T("REGISTER"), MB_ICONERROR);
					
				}
				else
				{
					m_msgString += _T("Dang ky thanh cong\r\n");
					MessageBox(_T("SUCCESSFUL\r\n"), _T("REGISTER"), MB_ICONINFORMATION);
					this->GetDlgItem(IDC_SIGNUP)->EnableWindow(FALSE);
					this->GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);
					this->GetDlgItem(IDC_LOGOUT)->EnableWindow(TRUE);
				}
				UpdateData(FALSE);
			}break;
			case 3://nhận message của các client khác được gửi lại từ sever.
			{
				m_msgString += strResult[1]+_T("-  ");
				m_msgString += strResult[2] + _T("\r\n");
				UpdateData(FALSE);
			}break;
			case 5://nhận thông tin các Client đang online từ Server 
			{
				int flag2 = _ttoi(strResult[1]);
				if (flag2 == 0)
				{
					CString name(strResult[2]);
					m_listClient.AddString(name);
				}
				if (flag2 == 1)
				{
					int index = _ttoi(strResult[2]);
					m_listClient.DeleteString(index);
				}
				UpdateData(FALSE);
			}break;
			
		}

	}break;
	case FD_CLOSE:
		{
			
			closesocket(sClient);
			m_msgString+=_T("Server da dong ket noi\r\n");
			GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOGIN)->EnableWindow(TRUE);
			UpdateData(FALSE);
			break;
		}
	}
	return 0;
}
void CClientDlg::OnBnClickedLogin()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if (IP == "")
	{
		MessageBox(_T("Vui long nhap dia chi IP server"), _T("WARNING"), MB_ICONWARNING);
		return;
	}
	if(m_userName=="")
	{
		MessageBox(_T("Vui long nhap ten user"),_T("WARNING"),MB_ICONWARNING);
		return;
	}
	if (m_Pass == "")
	{
		MessageBox(_T("Vui long nhap mat khau."), _T("WARNING"), MB_ICONWARNING);
		return;
	}

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	hostent* host=NULL;
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(_T("socket() failed"),_T("ERROR"), 0);
		return;
	}

	servAdd.sin_family = AF_INET;
	servAdd.sin_port = htons(PORT);

	char* cIP=ConvertToChar(IP);

	servAdd.sin_addr.s_addr = inet_addr(cIP);

	CStringA cpy_IP(IP); 

	if (servAdd.sin_addr.s_addr==INADDR_NONE)
	{
		host = (gethostbyname(cpy_IP));
		if (host == NULL)
		{
			MessageBox(_T("Khong the ket noi den server."),_T("ERROR"), 0);
		}
		CopyMemory(&servAdd.sin_addr, host->h_addr_list[0],host->h_length);
		return;
	}

	int err=connect(sClient,(struct sockaddr*)&servAdd,sizeof(servAdd));
	if (err==SOCKET_ERROR){
		MessageBox(_T("Ket noi that bai"),_T("ERROR"), MB_ICONERROR);
		return;
	}

	Command=_T("1\r\n");
	Command+= m_userName+_T("\r\n");
	Command += m_Pass + _T("\r\n");
	mSend(Command);

	WSAAsyncSelect(sClient,m_hWnd,WM_SOCKET,FD_READ|FD_CLOSE);
	UpdateData(FALSE);
}

void CClientDlg::OnBnClickedLogout()
{
	// TODO: Add your control notification handler code here
	INT_PTR i = MessageBox(_T("Ban muon logout?"),_T("Confirm"), MB_OKCANCEL);
	if (i == IDCANCEL)
		return;
	Command=_T("4\r\n\r\n\r\n");
	mSend(Command);
	this->GetDlgItem(IDC_SIGNUP)->EnableWindow(1);
	UpdateData(FALSE);
}

void CClientDlg::OnEnChangeMsgbox()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}




void CClientDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	Command = _T("4\r\n\r\n\r\n");
	mSend(Command);
	UpdateData(FALSE);
	CClientDlg::OnCancel();
}


void CClientDlg::OnBnClickedSend()
{
	// TODO: Add your control notification handler code here
	GetDlgItemTextW(IDC_MESSAGE,m_msgSend);
	if (m_msgSend == "")
	{
		MessageBox(_T("Type anything before sent."), _T("WARNING"), MB_ICONINFORMATION);
		return;
	}
	Command = _T("3\r\n");
	Command += m_userName + _T("\r\n");
	Command += m_msgSend + _T("\r\n");
	mSend(Command);
	m_msgSend = _T("");
	UpdateData(FALSE);	
}


void CClientDlg::OnBnClickedSignup()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (IP == "")
	{
		MessageBox(_T("Vui long nhap dia chi IP server"), _T("WARNING"), MB_ICONWARNING);
		return;
	}
	if (m_userName == "")
	{
		MessageBox(_T("Vui long nhap ten user dang ky"), _T("WARNING"), MB_ICONWARNING);
		return;
	}
	if (m_Pass == "")
	{
		MessageBox(_T("Vui long nhap mat khau."), _T("WARNING"), MB_ICONWARNING);
	}
	sClient = socket(AF_INET, SOCK_STREAM, 0);
	hostent* host = NULL;
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(_T("socket() failed"), _T("ERROR"), 0);
		return;
	}

	servAdd.sin_family = AF_INET;
	servAdd.sin_port = htons(PORT);

	char* cIP = ConvertToChar(IP);

	servAdd.sin_addr.s_addr = inet_addr(cIP);

	CStringA cpy_IP(IP);

	if (servAdd.sin_addr.s_addr == INADDR_NONE)
	{
		host = (gethostbyname(cpy_IP));
		if (host == NULL)
		{
			MessageBox(_T("Khong the ket noi den server."), _T("ERROR"), 0);
		}
		CopyMemory(&servAdd.sin_addr, host->h_addr_list[0], host->h_length);
		return;
	}

	int err = connect(sClient, (struct sockaddr*)&servAdd, sizeof(servAdd));
	if (err == SOCKET_ERROR) {
		MessageBox(_T("Ket noi that bai"), _T("ERROR"), MB_ICONERROR);
		return;
	}

	Command = _T("2\r\n");
	Command += m_userName + _T("\r\n");
	Command += m_Pass + _T("\r\n");
	mSend(Command);

	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);
}


void CClientDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
	int sel = m_listClient.GetCurSel();
	CString name,title;
	if (sel != LB_ERR)
	{
		m_listClient.GetText(sel, name);
		//if (name != this->m_userName)
		{
			// TODO: Add your control notification handler code here
				Private * priPr = new Private;
			priPr->priSock = this->sClient;
			priPr->receiverName = name;
			priPr->senderName = m_userName;


			DWORD dwRunClientPrivateDlgId;
			HANDLE hRunClientPrivateDlg;
			hRunClientPrivateDlg = CreateThread(NULL, 0, RunClientPrivateDlg, (LPVOID)priPr, 0, &dwRunClientPrivateDlgId);			
		}
	}

}
