// WaitLoading.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "WaitLoading.h"


// CWaitLoading �Ի���

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


// CWaitLoading ��Ϣ�������

BOOL CWaitLoading::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	mPCMove.SetRange(0,100);
	mPCMove.SetStep(20);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	// TODO: �ڴ����ר�ô����/����û���

	DestroyWindow();
}

void CWaitLoading::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::PostNcDestroy();
	delete this;
}

void CWaitLoading::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	DestroyWindow();
}
