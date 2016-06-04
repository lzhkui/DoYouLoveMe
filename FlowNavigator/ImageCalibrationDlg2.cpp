// ImageCalibrationDlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationDlg2.h"
#include "afxdialogex.h"


// ImageCalibrationDlg2 对话框

IMPLEMENT_DYNAMIC(ImageCalibrationDlg2, CDialog)

ImageCalibrationDlg2::ImageCalibrationDlg2(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationDlg2::IDD, pParent)
	, m_xMin_s(0)
	, m_xMax_s(0)
	, m_yMin_s(0)
	, m_yMax_s(0)
	, Lx(0)
	, Ly(0)
	, m_CamSign(0)
	, xClb1(0)
	, yClb1(0)
	, xClb2(0)
	, yClb2(0)
	, xClb3(0)
	, yClb3(0)
	, xClb4(0)
	, yClb4(0)
	, xClb5(0)
	, yCLb5(0)
	, xCLb6(0)
	, yCLb6(0)
	, xClb7(0)
	, yClb7(0)
	, xClb8(0)
	, yClb8(0)
	, m_ClbNum(0)
{
	for(int i = 0;i < MAX_CAMERAS; i++)
	{
		CamSign[i]     = FALSE;
		adjustImage[i] = NULL;
	}
	imgClb = NULL;
}

ImageCalibrationDlg2::~ImageCalibrationDlg2()
{
}

void ImageCalibrationDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_xMin_s);
	DDX_Text(pDX, IDC_EDIT2, m_xMax_s);
	DDX_Text(pDX, IDC_EDIT4, m_yMin_s);
	DDX_Text(pDX, IDC_EDIT5, m_yMax_s);
	DDX_Text(pDX, IDC_EDIT3, Lx);
	DDX_Text(pDX, IDC_EDIT6, Ly);
	DDX_Text(pDX, IDC_EDIT7, xClb1);
	DDX_Text(pDX, IDC_EDIT8, yClb1);
	DDX_Text(pDX, IDC_EDIT9, xClb2);
	DDX_Text(pDX, IDC_EDIT10, yClb2);
	DDX_Text(pDX, IDC_EDIT11, xClb3);
	DDX_Text(pDX, IDC_EDIT12, yClb3);
	DDX_Text(pDX, IDC_EDIT13, xClb4);
	DDX_Text(pDX, IDC_EDIT14, yClb4);
	DDX_Text(pDX, IDC_EDIT15, xClb5);
	DDX_Text(pDX, IDC_EDIT16, yCLb5);
	DDX_Text(pDX, IDC_EDIT17, xCLb6);
	DDX_Text(pDX, IDC_EDIT18, yCLb6);
	DDX_Text(pDX, IDC_EDIT19, xClb7);
	DDX_Text(pDX, IDC_EDIT20, yClb7);
	DDX_Text(pDX, IDC_EDIT21, xClb8);
	DDX_Text(pDX, IDC_EDIT22, yClb8);
	DDX_Text(pDX, IDC_EDIT23, m_CamSign);
	DDX_Control(pDX, IDC_EDIT23, m_CamSignCtr);
	DDX_Text(pDX, IDC_EDIT24, m_ClbNum);
	DDV_MinMaxInt(pDX, m_ClbNum, 4, 8);
}


BEGIN_MESSAGE_MAP(ImageCalibrationDlg2, CDialog)
	ON_BN_CLICKED(IDC_BT_BEGING, &ImageCalibrationDlg2::OnBnClickedBtBeging)
END_MESSAGE_MAP()


// ImageCalibrationDlg2 消息处理程序
void ImageCalibrationDlg2::OnBnClickedBtBeging()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		if(i == m_ClbNum)
		{
			CamSign[i] = TRUE;
		}
	}

	
}

void ImageCalibrationDlg2::setSingleRange(st_Range* range, int sign)
{
	m_stRange[sign] = range;
}

st_Range* ImageCalibrationDlg2::getSingleRange(int sign)
{
	return this->m_stRange[sign];
}

void ImageCalibrationDlg2::setL(float* Lx, float* Ly)
{
	this->L[0] = Lx;
	this->L[1] = Ly;
}

float** ImageCalibrationDlg2::getL()
{
	return this->L;
}

void ImageCalibrationDlg2::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	DestroyWindow();
}


void ImageCalibrationDlg2::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;

	CDialog::PostNcDestroy();
}

