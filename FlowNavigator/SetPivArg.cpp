// SetPivArg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "SetPivArg.h"


SetPivArg* SetPivArg::setPivArg = NULL;
int SetPivArg::num = 0;
int mLinkFlow = -1;
// SetPivArg 对话框

IMPLEMENT_DYNAMIC(SetPivArg, CDialog)

SetPivArg* SetPivArg::GetInstance()
{
	if(setPivArg == NULL)
	{
		setPivArg = new SetPivArg();
	}

	num++;
	TRACE(_T("++SetPivArg has %d\n"), num);

	return setPivArg;
}

SetPivArg::SetPivArg(CWnd* pParent /*=NULL*/)
    :CDialog(SetPivArg::IDD, pParent)
	, xSmall(0)
	, xLarge(0)
	, ySmall(0)
	, yLarge(0)
	, step(0)
	, m_Row(0)
	, copy_solveFlow(NULL)
{
	showView = NULL;
	int arg[8] = {32,32,1,30,-10,10,-10,10};
	for (int i = 0; i < 8; i++)
	{
		paras[i] = arg[i];
	}

	int range[4] = {0, 2560, 0, 2048};
	for (int j = 0; j < 4; j++)
	{
		XYRange[j] = range[j];
	}

	//((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	/*((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);*/
}
SetPivArg::~SetPivArg()
{
	num--;
	TRACE(_T("--SetPivArg has %d\n"), num);
}

void SetPivArg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_xSmall, xSmall);
	DDX_Text(pDX, IDC_xLarge, xLarge);
	DDX_Text(pDX, IDC_ySmall, ySmall);
	DDX_Text(pDX, IDC_yLarge, yLarge);
	DDX_Text(pDX, IDC_STEP, step);
	DDX_Text(pDX, IDC_ROW, m_Row);
}


BEGIN_MESSAGE_MAP(SetPivArg, CDialog)
	ON_BN_CLICKED(IDC_DEFAULTARG_PIV, &SetPivArg::OnBnClickedDefaultArgPiv)
	ON_BN_CLICKED(IDC_USECURRENT, &SetPivArg::OnBnClickedUseCurrent)
	ON_BN_CLICKED(IDC_STARTCALC, &SetPivArg::OnBnClickedStartCalc)
	ON_BN_CLICKED(IDC_STOPCALC, &SetPivArg::OnBnClickedStopCalc)
END_MESSAGE_MAP()


void SetPivArg::setShowView(ShowView* showView)
{
	this->showView = showView;
}

ShowView* SetPivArg::getShowView()
{
	return this->showView;
}

void SetPivArg::setShowViewArgs(BOOL default)
{
	/*int paras[8] = {32,32,1,30,-10,10,-10,10};*/
	if(default)
	{
		showView->setCalcRange(paras);
	}
	else
	{
		UpdateData(TRUE);
		paras[0] = m_Row;
		paras[1] = m_Row;
		showView->setCalcRange(paras, xSmall, xLarge, ySmall, yLarge, step);
	}
}

void SetPivArg::OnBnClickedDefaultArgPiv()
{
	// TODO: 在此添加控件通知处理程序代码
	xSmall = XYRange[0];
	xLarge = XYRange[1];
	ySmall = XYRange[2];
	yLarge = XYRange[3];
	step   = 30;
	m_Row  = 32;
	UpdateData(FALSE);

	setShowViewArgs(TRUE);
}

void SetPivArg::OnBnClickedUseCurrent()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);
	setShowViewArgs(FALSE);
}

void SetPivArg::OnBnClickedStartCalc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	BOOL m_MQD_MP    = ((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck();
	BOOL m_Cross_Cor = ((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck();
	if (m_MQD_MP)
	{
		showView->setChooseAlgorithm(0);
	}
	else if(m_Cross_Cor)
	{
		showView->setChooseAlgorithm(1);
	}
	*copy_solveFlow = 1;
}

void SetPivArg::OnBnClickedStopCalc()
{
	// TODO: 在此添加控件通知处理程序代码
	*copy_solveFlow = 0;
}


// SetPivArg 消息处理程序

void SetPivArg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	delete this;
	setPivArg = NULL;
	CDialog::PostNcDestroy();
}

void SetPivArg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	DestroyWindow();
}
