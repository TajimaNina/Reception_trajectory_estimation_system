#pragma once


// CLanSetupDlg �_�C�A���O

class CLanSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CLanSetupDlg)

public:
	CLanSetupDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CLanSetupDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG_LANSETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CString m_strHandle;
public:
	CString m_strServer;
public:
	UINT m_nChannel;
public:
	UINT m_nLevel;
public:
	BOOL m_bCheckLog;
};
