#pragma once


// ImageCalibrationZero �Ի���

class ImageCalibrationZero : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationZero)

public:
	ImageCalibrationZero(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationZero();

// �Ի�������
	enum { IDD = IDD_CLB_CHILD0 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
