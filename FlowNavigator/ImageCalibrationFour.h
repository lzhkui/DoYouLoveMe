#pragma once


// ImageCalibrationFour �Ի���

class ImageCalibrationFour : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationFour)

public:
	ImageCalibrationFour(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationFour();

// �Ի�������
	enum { IDD = IDD_CLB_CHILD4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
