#pragma once


// ImageCalibrationThree �Ի���

class ImageCalibrationThree : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationThree)

public:
	ImageCalibrationThree(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationThree();

// �Ի�������
	enum { IDD = IDD_CLB_CHILD3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
