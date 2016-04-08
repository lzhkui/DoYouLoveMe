// WaitLoading.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "WaitLoading.h"


// CWaitLoading 对话框

IMPLEMENT_DYNAMIC(CWaitLoading, CDialog)

CWaitLoading::CWaitLoading(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitLoading::IDD, pParent)
{
	times = 5;
}

CWaitLoading::~CWaitLoading()
{
}

void CWaitLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, mPCMove);
}


BEGIN_MESSAGE_MAP(CWaitLoading, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaitLoading 消息处理程序

BOOL CWaitLoading::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	mPCMove.SetRange(0,100);
	mPCMove.SetStep(20);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CWaitLoading::StartTimes(void)
{
	SetTimer(1,10,NULL);
}


void CWaitLoading::KillTimes(void)
{
	KillTimer(1);
}

void CWaitLoading::setTime(int times)
{
	this->times = times;
}
int CWaitLoading::getTime()
{
	return this->times;
}
void CWaitLoading::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
	if (times > 0)
	{
		times--;
		mPCMove.StepIt();
		Sleep(50);
	}
}

void CWaitLoading::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	DestroyWindow();
}

void CWaitLoading::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::PostNcDestroy();
	delete this;
}

void CWaitLoading::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	DestroyWindow();
}
