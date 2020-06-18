#pragma once
#define WM_SOCKET WM_USER+2

struct Private
{
	SOCKET priSock;
	CString senderName;
	CString receiverName;
};

// ClientPrivateDlg dialog

class ClientPrivateDlg : public CDialog
{
	DECLARE_DYNAMIC(ClientPrivateDlg)

public:
	ClientPrivateDlg(SOCKET sk, CString username, CString partner,CWnd* pParent = nullptr);   // standard constructor
	ClientPrivateDlg(CWnd* pParent =nullptr);
	virtual ~ClientPrivateDlg();
	SOCKET Client;
	CString username;
	CString partnername;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENTPRIVATE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString& s);
	void Split(CString src, CString des[3]);
	void mSend(CString Command);
	int mRecv(CString& Command);
	CString Command;
	int	buffLength;
	CString strResult[3];
	CString title;
	CString m_msgStringPr;
	CString m_msgSendPr;


	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedSendprivate();
};
DWORD WINAPI RunClientPrivateDlg(LPVOID lp);
