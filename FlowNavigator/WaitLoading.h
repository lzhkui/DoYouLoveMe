#pragma once
#include "afxcmn.h"


// CWaitLoading �Ի���

class CWaitLoading : public CDialog
{
	DECLARE_DYNAMIC(CWaitLoading)

public:
	CWaitLoading(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWaitLoading();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };
private:
	int times;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CProgressCtrl mPCMove;
	virtual BOOL OnInitDialog();
	void StartTimes(void);
	void KillTimes(void);
	int getTime();
	void setTime(int times);

protected:
	//��ģ̬��дһ����������
	virtual void OnCancel();
	virtual void PostNcDestroy();

	virtual void OnOK();
};
