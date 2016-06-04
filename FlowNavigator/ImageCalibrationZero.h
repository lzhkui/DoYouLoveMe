#pragma once


// ImageCalibrationZero 对话框

class ImageCalibrationZero : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationZero)

public:
	ImageCalibrationZero(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ImageCalibrationZero();

// 对话框数据
	enum { IDD = IDD_CLB_CHILD0 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
