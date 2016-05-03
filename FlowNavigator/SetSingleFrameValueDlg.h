#pragma once

// SetSingleFrameValueDlg 对话框
#include "FlowNavigatorDlg.h"

class SetSingleFrameValueDlg : public CDialog
{
	DECLARE_DYNAMIC(SetSingleFrameValueDlg)

public:
	SetSingleFrameValueDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SetSingleFrameValueDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SINGLEFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 流场间隔
	int SingleTimeSpace;
	// 流场个数
	int SingleCount;
	// 单次采集
	int CountOnce;

	afx_msg void OnBnClickedOk();
};
