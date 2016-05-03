// SetSingleFrameValueDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "SetSingleFrameValueDlg.h"

#define MAX_CAMERAS 8

// SetSingleFrameValueDlg 对话框

IMPLEMENT_DYNAMIC(SetSingleFrameValueDlg, CDialog)

SetSingleFrameValueDlg::SetSingleFrameValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetSingleFrameValueDlg::IDD, pParent)
	, SingleTimeSpace(0)
	, SingleCount(0)
	, CountOnce(0)
{

}

SetSingleFrameValueDlg::~SetSingleFrameValueDlg()
{
}

void SetSingleFrameValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, SingleTimeSpace);
	DDV_MinMaxInt(pDX, SingleTimeSpace, 1, 36000);
	DDX_Text(pDX, IDC_EDIT2, SingleCount);
	DDV_MinMaxInt(pDX, SingleCount, 2, 36000);
	DDX_Text(pDX, IDC_EDIT3, CountOnce);
}


BEGIN_MESSAGE_MAP(SetSingleFrameValueDlg, CDialog)
	ON_BN_CLICKED(IDOK, &SetSingleFrameValueDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SetSingleFrameValueDlg 消息处理程序

void SetSingleFrameValueDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		Count_Single[i] = SingleCount;
		Count_Space[i]  = SingleTimeSpace;
		Count_Once[i]   = CountOnce;
	}
	OnOK();
}
