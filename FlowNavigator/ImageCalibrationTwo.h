#pragma once


// ImageCalibrationTwo �Ի���

class ImageCalibrationTwo : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationTwo)

public:
	ImageCalibrationTwo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationTwo();

// �Ի�������
	enum { IDD = IDD_CLB_CHILD2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
