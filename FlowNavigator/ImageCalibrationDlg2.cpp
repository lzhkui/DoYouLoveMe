// ImageCalibrationDlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationDlg2.h"
#include "afxdialogex.h"

int startClb = -1;
int What_NumOfClb = 0;
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
	, m_ClbNum(4)
	, m_leftSplit(0)
	, m_RightSplit(0)
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
	DDX_Text(pDX, IDC_EDIT25, m_leftSplit);
	DDX_Text(pDX, IDC_EDIT26, m_RightSplit);
}


BEGIN_MESSAGE_MAP(ImageCalibrationDlg2, CDialog)
	ON_BN_CLICKED(IDC_BT_BEGING, &ImageCalibrationDlg2::OnBnClickedBtBeging)
	ON_BN_CLICKED(IDC_BT_RECLB, &ImageCalibrationDlg2::OnBnClickedBtReClb)

	ON_BN_CLICKED(IDC_LOAD_CLBSETTING, &ImageCalibrationDlg2::OnBnClickedLoadClbSetting)
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

	CalibrationPoint CblPoint[MAX_CALIBRATION] = {0};

	CblPoint[0].x_p = xClb1;
	CblPoint[0].y_p = yClb1;
	CblPoint[1].x_p = xClb2;
	CblPoint[1].y_p = yClb2;
	CblPoint[2].x_p = xClb3;
	CblPoint[2].y_p = yClb3;
	CblPoint[3].x_p = xClb4;
	CblPoint[3].y_p = yClb4;
	CblPoint[4].x_p = xClb5;
	CblPoint[4].y_p = yCLb5;
	CblPoint[5].x_p = xCLb6;
	CblPoint[5].y_p = yCLb6;
	CblPoint[6].x_p = xClb7;
	CblPoint[6].y_p = yClb7;
	CblPoint[7].x_p = xClb8;
	CblPoint[7].y_p = yClb8;
	imgClb->setCBPoint(CblPoint, m_ClbNum);
	imgClb->setCurrentCamSign(m_CamSign);

	adjustImage[m_CamSign]->setSingleRange(m_xMin_s, m_xMax_s, m_yMin_s, m_yMax_s);
	adjustImage[m_CamSign]->setL(Lx, Ly);
	adjustImage[m_CamSign]->setLeftSplitLine(m_leftSplit);
	adjustImage[m_CamSign]->setSplitLine(m_RightSplit);
	adjustImage[m_CamSign]->setAdjustImageSize(); // 这句要在setSingleRange()之后调用

	FILE* fp;
	char pathTemp[MAX_PATH] = {0};
	char path[MAX_PATH]     = {0};
	wcstombs(pathTemp, CurrentPath, CurrentPath.GetLength());  
	sprintf(path, "%s\\配置文件\\Position_Cam%d.txt", pathTemp, m_CamSign);
	fp = fopen(path, "w");
	fprintf(fp, "%f ", m_xMin_s);
	fprintf(fp, "%f ", m_xMax_s);
	fprintf(fp, "%f ", m_yMin_s);
	fprintf(fp, "%f ", m_yMax_s);
	fprintf(fp, "%f ", Lx);
	fprintf(fp, "%f ", Ly);
	fprintf(fp, "%f ", m_leftSplit);
	fprintf(fp, "%f ", m_RightSplit);
	fprintf(fp, "%f %f %f %f %f %f %f %f", xClb1,yClb1,xClb2,yClb2,xClb3,yClb3,xClb4,yClb4);
	fclose(fp);

	What_NumOfClb = 0;
	startClb = 1;
}

void ImageCalibrationDlg2::OnBnClickedBtReClb()
{
	What_NumOfClb = 0;
}

void ImageCalibrationDlg2::OnBnClickedLoadClbSetting()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE); // 更新m_CamSign
	FILE* fp;
	char pathTemp[MAX_PATH] = {0};
	char path[MAX_PATH]     = {0};

	wcstombs(pathTemp, CurrentPath, CurrentPath.GetLength());  
	sprintf(path, "%s\\配置文件\\Position_Cam%d.txt", pathTemp, m_CamSign);
	fp = fopen(path, "r");

	fscanf(fp, "%f %f %f %f %f %f %f %f ", &m_xMin_s, &m_xMax_s, &m_yMin_s,
		&m_yMax_s, &Lx, &Ly, &m_leftSplit, &m_RightSplit);

	fscanf(fp, "%f %f %f %f %f %f %f %f", &xClb1,&yClb1,&xClb2,&yClb2,&xClb3,&yClb3,&xClb4,&yClb4);
	UpdateData(FALSE);
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
	startClb = -1;

	DestroyWindow();
}


void ImageCalibrationDlg2::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	startClb = -1;
	delete this;

	CDialog::PostNcDestroy();
}

