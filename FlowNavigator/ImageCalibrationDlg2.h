#pragma once
#include "afxwin.h"
#include "KGloabalVar.h"
#include "KGlobalDefine.h"
#include "AdjustImage.h"
#include "ImageCalibration.h"

// ImageCalibrationDlg2 对话框

extern int startClb;        // 1:开始标定
extern int What_NumOfClb;   //标记当前指定的是第几个标定点

class ImageCalibrationDlg2 : public CDialog
{
	DECLARE_DYNAMIC(ImageCalibrationDlg2)

public:
	ImageCalibrationDlg2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ImageCalibrationDlg2();

	AdjustImage *adjustImage[MAX_CAMERAS];
	ImageCalibration* imgClb;

private:
	st_Range* m_stRange[MAX_CAMERAS];
	float*    L[2];                         //L[0]: x方向分辨率   L[1]: y方向分辨率
	BOOL      CamSign[MAX_CAMERAS];         //TRUE: 相机已标定过

public:
	//单个相机的原始实际范围
	void      setSingleRange(st_Range* range, int sign);
	st_Range* getSingleRange(int sign);

	//所谓的“分辨率”，即1像素对应的实际物理距离
	void      setL(float* Lx, float* Ly);
	float**   getL();



// 对话框数据
	enum { IDD = IDD_CLB_CHILD8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	float m_xMin_s;
	float m_xMax_s;
	float m_yMin_s;
	float m_yMax_s;
	float Lx;
	float Ly;
	// 左分割线
	float m_leftSplit;
	// 右分割线
	float m_RightSplit;
	int   m_CamSign;
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
	CEdit m_CamSignCtr;  //相机编号
	int m_ClbNum;        //输入的标定点个数

	CString CurrentPath;

	afx_msg void OnBnClickedBtBeging();
	afx_msg void OnBnClickedBtReClb();

	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedLoadClbSetting();
};
