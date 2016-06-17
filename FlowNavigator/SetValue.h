#pragma once

#include <Jai_Factory.h>
#include "afxcmn.h"  //控件CSliderCtrl
#include "BuildDir.h"
using namespace BuildDir;
#include "afxwin.h"

#include "KGlobalDefine.h"

// SetValue 对话框

#define Min_EXPOSURE 10
#define Max_EXPOSURE 44730

#define Min_GAIN 100
#define MAX_GAIN 1600

#define  MIN_FRAME 1
#define  MAX_FRAME 20

class CSetValue : public CDialog
{
	DECLARE_DYNAMIC(CSetValue)

public:
	CSetValue(CWnd* pParent, CString curProPath);   // 标准构造函数
	virtual ~CSetValue();

public:
	CAM_HANDLE m_hCam[8];

	NODE_HANDLE m_hExposureNode;
// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnStnClickedMinexposure();
	afx_msg void OnEnChangeCurexposure();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSaveSetup();

protected:
	virtual void OnOK();
	virtual void OnCancel();

protected:
	BOOL b_splitCamCheck[MAX_CAMERAS + 1];
	int  checkNum;
	int  checkCamSign[MAX_CAMERAS];
	CMenu menu;
public:
	CSliderCtrl mExpSliderCtrl;
	int m_MinExposure;
	int m_MaxExposure;
	int m_CurExposure;
	int m_CameraCount;

	int m_CurGain;
	int m_MinGain;
	int m_MaxGain;
	double m_CurFrame;
	int m_MinFrame;
	int m_MaxFrame;
	CSliderCtrl mGainSliderCtrl;
	CSliderCtrl mFrameSliderCtrl;

    CString m_CurrentProPath;
	CSplitButton m_splitBt;

	void Start();
	void SetRangeMM(void);
	int CSetValue::SetValueInt_CWJ(CAM_HANDLE hCam[], int8_t* sNodeName, int64_t Value, int m_CameraCount);
	int SetFromEditToSlider(int m_CurrentValue, CSliderCtrl& mSlider, int Min, int Max);
	int CSetValue::SetValueMM(CAM_HANDLE hCam[], int8_t* sNodeName,
		CSliderCtrl& mSlider, int* Min, int* Max);
	void setInitValueMM(CSliderCtrl& mSlider, int& mMin, int& mMax,int Min, int Max);

	afx_msg void OnBnClickedLoadlastsetup();
	virtual BOOL OnInitDialog();
	afx_msg void OnCamsetupAllUse();
	afx_msg void OnCamsetupCam0();
	afx_msg void OnCamsetupCam1();
	afx_msg void OnCamsetupCam2();
	afx_msg void OnCamsetupCam3();
	afx_msg void OnCamsetupCam4();
	afx_msg void OnCamsetupCam5();
	afx_msg void OnCamsetupCam6();
	afx_msg void OnCamsetupCam7();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnClose();
};
