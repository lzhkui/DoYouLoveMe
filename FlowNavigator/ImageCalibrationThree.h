#pragma once


// ImageCalibrationThree 对话框

class ImageCalibrationThree : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationThree)

public:
	ImageCalibrationThree(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ImageCalibrationThree();

// 对话框数据
	enum { IDD = IDD_CLB_CHILD3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
