#pragma once
#include "afxcmn.h"


// CWaitLoading 对话框

class CWaitLoading : public CDialog
{
	DECLARE_DYNAMIC(CWaitLoading)

public:
	CWaitLoading(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWaitLoading();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };
private:
	int times;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
	//非模态重写一下两个函数
	virtual void OnCancel();
	virtual void PostNcDestroy();

	virtual void OnOK();
};
