#pragma once


// ImageCalibrationFour 对话框

class ImageCalibrationFour : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationFour)

public:
	ImageCalibrationFour(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ImageCalibrationFour();

// 对话框数据
	enum { IDD = IDD_CLB_CHILD4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
