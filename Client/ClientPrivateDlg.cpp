// ClientPrivateDlg.cpp : implementation file
//


#include "stdafx.h"
#include "Client.h"
#include "ClientPrivateDlg.h"


// ClientPrivateDlg dialog

IMPLEMENT_DYNAMIC(ClientPrivateDlg, CDialog)

ClientPrivateDlg::ClientPrivateDlg(SOCKET sk, CString username, CString partner, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CLIENTPRIVATE, pParent)
	, m_msgStringPr(_T(""))
	, m_msgSendPr(_T(""))
{
	this->username = username;
	Client = sk;
	this->partnername = partner;
	title = _T("Private Chat with ");
	title += partnername;
	//this->SetWindowTextW(title);
	//WSAAsyncSelect(Client, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	//UpdateData(FALSE);
}
ClientPrivateDlg::ClientPrivateDlg( CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CLIENTPRIVATE, pParent)
	, m_msgStringPr(_T(""))
	, m_msgSendPr(_T(""))
{
}

ClientPrivateDlg::~ClientPrivateDlg()
{
}

void ClientPrivateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PRBOXCHAT, m_msgStringPr);
	DDX_Text(pDX, IDC_PRMESS, m_msgSendPr);
}


BEGIN_MESSAGE_MAP(ClientPrivateDlg, CDialog)
	ON_MESSAGE(WM_SOCKET, SockMsg)
	ON_BN_CLICKED(IDC_SENDPRIVATE, &ClientPrivateDlg::OnBnClickedSendprivate)
END_MESSAGE_MAP()

BOOL ClientPrivateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowTextW(title);
	//WSAAsyncSelect(Client, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
DWORD WINAPI RunClientPrivateDlg(LPVOID lp)
{
	// TODO: Add your control notification handler code here

	Private* priPr = NULL;
	priPr = (Private*)lp;

	ClientPrivateDlg Pdlg(priPr->priSock,priPr->senderName,priPr->receiverName);

	INT_PTR nResponseP = Pdlg.DoModal();

	if (nResponseP == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponseP == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	return 0;
}





// ClientPrivateDlg message handlers
void ClientPrivateDlg::Split(CString src, CString des[3])
{
	int p1, p2, p3;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

	p3 = src.Find(_T("\r\n"), p2 + 1);
	des[2] = src.Mid(p2 + 2, p3 - (p2 + 2));
}

char* ClientPrivateDlg::ConvertToChar(const CString& s)
{
	int nSize = s.GetLength();
	char* pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void ClientPrivateDlg::mSend(CString Command)
{
	int Len = Command.GetLength();
	Len += Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff, 0, 1000);
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len);
	send(Client, (char*)&Len, sizeof(Len), 0);
	send(Client, (char*)sendBuff, Len, 0);
	delete sendBuff;
}

int ClientPrivateDlg::mRecv(CString& Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer, 0, 1000);
	recv(Client, (char*)&buffLength, sizeof(int), 0);
	recv(Client, (char*)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;

	if (Command.GetLength() == 0)
		return -1;
	return 0;
}

LRESULT ClientPrivateDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{

	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch (WSAGETSELECTEVENT(lParam))
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
		case 6:
		{

		}break;
		default:
			break;
		}
	}break;
	case FD_CLOSE:
	{

	}break;
	}
	return 0;
}




void ClientPrivateDlg::OnBnClickedSendprivate()
{
	// TODO: Add your control notification handler code here
	this->GetDlgItemTextW(IDC_PRMESS, m_msgSendPr);
	if (m_msgSendPr == "")
	{
		MessageBox(_T("Message is empty. Enter anychar to send."), _T("ERROR"), MB_ICONERROR);
		return;
	}
	Command = _T("6\r\n");
	Command += this->partnername;
	Command += _T("\r\n");
	Command += m_msgSendPr;
	Command += _T("\r\n");
	mSend(Command);
	m_msgSendPr = _T("");
	UpdateData(FALSE);
}
