// FlowNavigatorDlg.h : 头文件
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"
#include "SetValue.h"
#include "WaitLoading.h"
#include "ImageInfo.h"
#include <afxmt.h>// CCriticalSection
#include "CSetSampleDlg.h"

#define  MAX_CAMERAS 8
#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"
#define NODE_NAME_EXPOSURE      (int8_t*)"ExposureTimeRaw"
#define NODE_NAME_EXPOSURE_MODE (int8_t*)"ExposureMode"

#define K_INIT_WIDTH 2560
#define K_INIT_HEIGH 2048

#define MAX_IMG_COUNT 100


// CFlowNavigatorDlg 对话框
class CFlowNavigatorDlg : public CDialog
{
// 构造
public:
	CFlowNavigatorDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CFlowNavigatorDlg();
// 对话框数据
	enum { IDD = IDD_FLOWNAVIGATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	FACTORY_HANDLE  m_hFactory;                         // Factory Handle
	CAM_HANDLE      m_hCam[MAX_CAMERAS];                             // Camera Handle
	VIEW_HANDLE     m_hView[MAX_CAMERAS];                            // View Window handle
	THRD_HANDLE     m_hThread[MAX_CAMERAS];                          // Stream Channel handle
	int8_t          m_sCameraId[MAX_CAMERAS][J_CAMERA_ID_SIZE];      // Camera ID
	NODE_HANDLE     m_hGainNode;                        // Handle to "GainRaw" node
	NODE_HANDLE     m_hExposureNode;                    // Handle to "ExposureTimeRaw" node
	PIXELVALUE			m_PixelValue;
	uint64_t			m_PixelType;
	//CRITICAL_SECTION    m_CriticalSection;
	//J_tIMAGE_INFO		m_CnvImageInfo;    // Image info structure
	
	BITMAPINFO *bmpinfo;
	int RecvFrame,RecvFrame1;//当前接收到的帧数
	CWinThread* hThread[MAX_CAMERAS]; //存图片线程

	//LOGPALETTE *pLogPal;   
	//CPalette m_hPalete;   //调色板

	unsigned int CtrLine; //控制显示的模式2行4列 还是1行8列
 
	BOOL OpenFactoryAndCamera();                    
	void CloseFactoryAndCamera();
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc3(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc4(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc5(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc6(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc7(J_tIMAGE_INFO * pAqImageInfo);

	void LiveView_Cwj(J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,
		int x,int y,int scaleX,int scaleY,
		int nStretchMode = COLORONCOLOR);

	void InitializeControls();
	void EnableControls();

	void ShowErrorMsg(CString message, J_STATUS_TYPE error);


// 实现
protected:
	HICON m_hIcon;
	int   m_CameraCount;//实际相机个数
	BOOL bt_on;//true:打开状态 让菜单按钮变灰

private:
	unsigned int test_count_getImage;
	
	CSetValue *setDlg;//参数设置对话框
	ImageInfo myImageInfo;
	pImageNode head[MAX_CAMERAS];
	pImageNode pImage[MAX_CAMERAS];

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnOpenCamera();
	afx_msg void OnCloseCamera();
	afx_msg void OnQuit();
	afx_msg void OnSetValues();
	afx_msg void OnDestroy();
	afx_msg void On4Col2Line();
	afx_msg void On8col1line();
	afx_msg void OnContinuous();
	afx_msg void OnSingleFrame();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();

	static int SaveImage_CWJ(J_tIMAGE_INFO* pAqImageInfo, J_tIMAGE_INFO& m_CnvImageInfo,int Count,
		 const char* numCam,
		 const char* path = NULL, const char* fileName = NULL);

	public:
	//static UINT Write2File(LPVOID param);//类中不能加static

	int InsertHead(pImageNode pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls, 
			pImageNode pHead);

// 		/*内部使用了imageInfo类的静态函数,使用时要包含imageInfo.h   用了Count   CCriticalSection全局变量 */
// 	static int SaveAndDelTail(pImageNode pHead,
// 			J_tIMAGE_INFO& m_CnvImageInfo,/* int Count,*/
// 			const char* numCam,
// 			const char* path = NULL, const char* fileName = NULL);

};
