// ImageCalibrationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationDlg.h"
#include "afxdialogex.h"


// ImageCalibrationDlg �Ի���

IMPLEMENT_DYNAMIC(ImageCalibrationDlg, CDialog)

ImageCalibrationDlg::ImageCalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationDlg::IDD, pParent)
{
	
}

ImageCalibrationDlg::~ImageCalibrationDlg()
{
}


void ImageCalibrationDlg::initial()
{
// 	child0 = NULL;
// 	child1 = NULL;
// 	child2 = NULL;
}

void ImageCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(ImageCalibrationDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &ImageCalibrationDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// ImageCalibrationDlg ��Ϣ�������


BOOL ImageCalibrationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_tab.InsertItem(0, _T("Cam0"));
	m_tab.InsertItem(1, _T("Cam1"));
	m_tab.InsertItem(2, _T("Cam2"));
	m_tab.InsertItem(3, _T("Cam3"));

	child0.Create(IDD_CLB_CHILD0, &m_tab);
	child1.Create(IDD_CLB_CHILD1, &m_tab);
	child2.Create(IDD_CLB_CHILD2, &m_tab);
	child3.Create(IDD_CLB_CHILD3, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);

	rect.top += 21;
	rect.bottom -= 1;
	rect.left += 1;
	rect.right -= 1;

	child0.MoveWindow(&rect);
	child1.MoveWindow(&rect);
	child2.MoveWindow(&rect);
	child3.MoveWindow(&rect);

	child0.ShowWindow(TRUE);
	child1.ShowWindow(FALSE);
	child2.ShowWindow(FALSE);
	child3.ShowWindow(FALSE);

	m_tab.SetCurSel(0);


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void ImageCalibrationDlg::setShowWindow(int sign)
{
	int Num = getCLBNum();
	for (int i = 0; i < Num; i++)
	{
// 		if(i != sign)
// 		{
// 			child.ShowWindow(FALSE);
// 		}
// 		else
// 		{
// 			m_tab
// 		}
	}
}

int ImageCalibrationDlg::getCLBNum()
{
	return MAX_CAMERAS;
}

void ImageCalibrationDlg::setCLBNum(int num)
{
	this->num = num;
}

void ImageCalibrationDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int curSel = m_tab.GetCurSel();
	switch(curSel)
	{
	case 0:
		child0.ShowWindow(TRUE);
		child1.ShowWindow(FALSE);
		child2.ShowWindow(FALSE);
		child3.ShowWindow(FALSE);

		break;
	case 1:
		child0.ShowWindow(FALSE);
		child1.ShowWindow(TRUE);
		child2.ShowWindow(FALSE);
		child3.ShowWindow(FALSE);

	case 2:
		child0.ShowWindow(FALSE);
		child1.ShowWindow(FALSE);
		child2.ShowWindow(TRUE);
		child3.ShowWindow(FALSE);
	case 3:
		child0.ShowWindow(FALSE);
		child1.ShowWindow(FALSE);
		child2.ShowWindow(FALSE);
		child3.ShowWindow(TRUE);

	default:
		;
	}

		*pResult = 0;
}

void ImageCalibrationDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();

	delete this;
}


void ImageCalibrationDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	DestroyWindow();
}


