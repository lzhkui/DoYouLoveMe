#pragma once


// ImageCalibrationOne �Ի���

class ImageCalibrationOne : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationOne)

public:
	ImageCalibrationOne(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationOne();

// �Ի�������
	enum { IDD = IDD_CLB_CHILD1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
