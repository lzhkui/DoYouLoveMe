#pragma once


// ImageCalibrationTwo 对话框

class ImageCalibrationTwo : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationTwo)

public:
	ImageCalibrationTwo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ImageCalibrationTwo();

// 对话框数据
	enum { IDD = IDD_CLB_CHILD2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
