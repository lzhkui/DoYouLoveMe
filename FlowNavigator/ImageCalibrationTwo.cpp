// ImageCalibrationTwo.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationTwo.h"
#include "afxdialogex.h"


// ImageCalibrationTwo 对话框

IMPLEMENT_DYNAMIC(ImageCalibrationTwo, CDialog)

ImageCalibrationTwo::ImageCalibrationTwo(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationTwo::IDD, pParent)
{

}

ImageCalibrationTwo::~ImageCalibrationTwo()
{
}

void ImageCalibrationTwo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ImageCalibrationTwo, CDialog)
END_MESSAGE_MAP()


// ImageCalibrationTwo 消息处理程序
