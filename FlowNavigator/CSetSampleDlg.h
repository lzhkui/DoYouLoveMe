#pragma once
#include <Jai_Factory.h>

#define  MAX_CAMERAS 8

// SetSampleDlg 对话框

class CSetSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetSampleDlg)

public:
	CSetSampleDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetSampleDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_SampleTime;
	int m_CameraCount;
	double m_CurFrame;
	int Count;
	CAM_HANDLE m_hCam[MAX_CAMERAS];

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
