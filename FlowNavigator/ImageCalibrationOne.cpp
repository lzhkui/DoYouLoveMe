// ImageCalibrationOne.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationOne.h"
#include "afxdialogex.h"


// ImageCalibrationOne 对话框

IMPLEMENT_DYNAMIC(ImageCalibrationOne, CDialog)

ImageCalibrationOne::ImageCalibrationOne(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationOne::IDD, pParent)
{

}

ImageCalibrationOne::~ImageCalibrationOne()
{
}

void ImageCalibrationOne::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ImageCalibrationOne, CDialog)
END_MESSAGE_MAP()


// ImageCalibrationOne 消息处理程序
