#pragma once

#include <Jai_Factory.h>
#include "afxcmn.h"  //控件CSliderCtrl

// SetValue 对话框

#define MAX_CAMERAS 8
#define NODE_NAME_EXPOSURE      (int8_t*)"ExposureTimeRaw"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"


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
	CSetValue(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetValue();

	void Start();

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

	CSliderCtrl mExpSliderCtrl;
	int m_MinExposure;
	int m_MaxExposure;
	afx_msg void OnStnClickedMinexposure();
	int m_CurExposure;
	afx_msg void OnEnChangeCurexposure();
	int m_CameraCount;
protected:
	virtual void OnOK();
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	virtual void OnCancel();
public:
	int m_CurGain;
	int m_MinGain;
	int m_MaxGain;
	double m_CurFrame;
	int m_MinFrame;
	int m_MaxFrame;
	CSliderCtrl mGainSliderCtrl;
	CSliderCtrl mFrameSliderCtrl;

	void SetRangeMM(void);
	int CSetValue::SetValueInt_CWJ(CAM_HANDLE hCam[], int8_t* sNodeName, int64_t Value, int m_CameraCount);
	int SetFromEditToSlider(int m_CurrentValue, CSliderCtrl& mSlider, int Min, int Max);
	int CSetValue::SetValueMM(CAM_HANDLE hCam[], int8_t* sNodeName,
		CSliderCtrl& mSlider, int* Min, int* Max);
	void setInitValueMM(CSliderCtrl& mSlider, int& mMin, int& mMax,int Min, int Max);
};
