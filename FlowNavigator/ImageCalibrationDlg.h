#pragma once
#include "afxcmn.h"
#include "ImageCalibrationZero.h"
#include "ImageCalibrationOne.h"
#include "ImageCalibrationTwo.h"
#include "ImageCalibrationThree.h"
#include "ImageCalibrationFour.h"

#include "KGlobalDefine.h"

// ImageCalibrationDlg 对话框
typedef struct ImageCalibrationChild
{
}Child;

class ImageCalibrationDlg : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationDlg)

public:
	ImageCalibrationDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ImageCalibrationDlg();
	void initial();

private:
	int num;                                     //选中的相机总数
	//int CheckCamSign[MAX_CAMERAS];               //当前选中的相机实际编号


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


// 对话框数据
	enum { IDD = IDD_CLB_PARENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
