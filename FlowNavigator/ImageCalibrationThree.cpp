// ImageCalibrationThree.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibrationThree.h"
#include "afxdialogex.h"


// ImageCalibrationThree 对话框

IMPLEMENT_DYNAMIC(ImageCalibrationThree, CDialog)

ImageCalibrationThree::ImageCalibrationThree(CWnd* pParent /*=NULL*/)
	: CDialog(ImageCalibrationThree::IDD, pParent)
{

}

ImageCalibrationThree::~ImageCalibrationThree()
{
}

void ImageCalibrationThree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ImageCalibrationThree, CDialog)
END_MESSAGE_MAP()


// ImageCalibrationThree 消息处理程序
