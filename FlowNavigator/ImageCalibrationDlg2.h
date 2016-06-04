#pragma once
#include "afxwin.h"
#include "KGloabalVar.h"
#include "KGlobalDefine.h"
#include "AdjustImage.h"
#include "ImageCalibration.h"

// ImageCalibrationDlg2 �Ի���

class ImageCalibrationDlg2 : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationDlg2)

public:
	ImageCalibrationDlg2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationDlg2();

	AdjustImage *adjustImage[MAX_CAMERAS];
	ImageCalibration* imgClb;

private:
	st_Range* m_stRange[MAX_CAMERAS];
	float*    L[2];                         //L[0]: x����ֱ���   L[1]: y����ֱ���
	BOOL      CamSign[MAX_CAMERAS];

public:
	//���������ԭʼʵ�ʷ�Χ
	void      setSingleRange(st_Range* range, int sign);
	st_Range* getSingleRange(int sign);

	//��ν�ġ��ֱ��ʡ�����1���ض�Ӧ��ʵ���������
	void      setL(float* Lx, float* Ly);
	float**   getL();



// �Ի�������
	enum { IDD = IDD_CLB_CHILD8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	float m_xMin_s;
	float m_xMax_s;
	float m_yMin_s;
	float m_yMax_s;
	float Lx;
	float Ly;
	float m_CamSign;
	float xClb1;
	float yClb1;
	float xClb2;
	float yClb2;
	float xClb3;
	float yClb3;
	float xClb4;
	float yClb4;
	float xClb5;
	float yCLb5;
	float xCLb6;
	float yCLb6;
	float xClb7;
	float yClb7;
	float xClb8;
	float yClb8;
	afx_msg void OnBnClickedBtBeging();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	CEdit m_CamSignCtr;
	int m_ClbNum;
};
