#pragma once

// SetSingleFrameValueDlg �Ի���
#include "FlowNavigatorDlg.h"

class SetSingleFrameValueDlg : public CDialog
{
	DECLARE_DYNAMIC(SetSingleFrameValueDlg)

public:
	SetSingleFrameValueDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SetSingleFrameValueDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SINGLEFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// �������
	int SingleTimeSpace;
	// ��������
	int SingleCount;
	// ���βɼ�
	int CountOnce;

	afx_msg void OnBnClickedOk();
};
