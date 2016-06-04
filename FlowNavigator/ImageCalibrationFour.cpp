// ImageCalibrationFour.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationFour.h"
#include "afxdialogex.h"


// ImageCalibrationFour 对话框

IMPLEMENT_DYNAMIC(ImageCalibrationFour, CDialog)

ImageCalibrationFour::ImageCalibrationFour(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationFour::IDD, pParent)
{

}

ImageCalibrationFour::~ImageCalibrationFour()
{
}

void ImageCalibrationFour::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ImageCalibrationFour, CDialog)
END_MESSAGE_MAP()


// ImageCalibrationFour 消息处理程序
