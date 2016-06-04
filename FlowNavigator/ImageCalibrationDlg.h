#pragma once
#include "afxcmn.h"
#include "ImageCalibrationZero.h"
#include "ImageCalibrationOne.h"
#include "ImageCalibrationTwo.h"
#include "ImageCalibrationThree.h"
#include "ImageCalibrationFour.h"

#include "KGlobalDefine.h"

// ImageCalibrationDlg �Ի���
typedef struct ImageCalibrationChild
{
}Child;

class ImageCalibrationDlg : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationDlg)

public:
	ImageCalibrationDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImageCalibrationDlg();
	void initial();

private:
	int num;                                     //ѡ�е��������
	//int CheckCamSign[MAX_CAMERAS];               //��ǰѡ�е����ʵ�ʱ��


public:
	CTabCtrl m_tab;
	//Child child;
	ImageCalibrationZero child0;
	ImageCalibrationOne  child1;
	ImageCalibrationTwo  child2;
	ImageCalibrationThree child3;
	ImageCalibrationFour child4;

public:
	void setShowWindow(int sign);
	int  getCLBNum();
	void setCLBNum(int num);


// �Ի�������
	enum { IDD = IDD_CLB_PARENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
