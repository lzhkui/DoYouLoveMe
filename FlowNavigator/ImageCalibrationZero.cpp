// ImageCalibrationZero.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationZero.h"
#include "afxdialogex.h"


// ImageCalibrationZero 对话框

IMPLEMENT_DYNAMIC(ImageCalibrationZero, CDialog)

ImageCalibrationZero::ImageCalibrationZero(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationZero::IDD, pParent)
{

}

ImageCalibrationZero::~ImageCalibrationZero()
{
}

void ImageCalibrationZero::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ImageCalibrationZero, CDialog)
END_MESSAGE_MAP()


// ImageCalibrationZero 消息处理程序
