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
#include "test.h"
#include "AdjustImage.h"
#include "ShowView.h"
#include "AdjustThreadFunc.h"
#include "LinkImage.h"
#include  <afxpriv.h> //T2A()可能用到
#include "ErrorCodeList.h"
#include "SetSingleFrameValueDlg.h"
#include "CheckToShow.h"
#include "KGloabalVar.h"
#include <afxext.h>
#include "ZoomImage.h"
#include "SetPivArg.h"
#include "ImageCalibrationDlg.h"
#include "ImageCalibrationDlg2.h"
#include "ImageCalibration.h"
#include "MyStatusBar.h"

#define MAX_CAMERAS 8
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

#define WM_MY_MESSAGE_INIT  (WM_USER + 100)   //打开系统进度
#define WM_MESSAGE_INITPATH (WM_USER + 101)   //采集时在项目路径下建立工况目录

#define SYSTEM_WILL_OPEN     0               //系统即将打开
#define SYSTEM_OPEN_ING      1               //系统正在打开
#define SYSTEM_IS_OPEN       2               //系统已就绪
#define SYSTEM_NOT_OPEN      3               //系统未打开
#define COLLECTION_NOT_START 4               //采集状态 重置
#define CAM_IS_CLOSED        5               //已打开相机编号 重置

 //定时器ID 
#define SETTIMER_ID_SYSTEM_TIME 0    //状态栏系统时间
#define SETTIMER_ID_COLLECTION  1    //状态栏采集状态

extern int Count_Single[MAX_CAMERAS];//流场个数
extern int Count_Once[MAX_CAMERAS];  //单次采集张数
extern int Count_Space[MAX_CAMERAS]; //流场间隔

//打开相机进度
typedef struct Wait_inParam 
{
	MyStatusBar* statusBar;
	int         CameraCount;
}st_Wait;

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

	//图像校正线程传入参数
	pAdajustCLs m_pAdjustCls;
	unsigned int*relateArr[MAX_CAMERAS];

	ShowView *showView;     //用于实时显示
	ShowView *SV_AdjustArg; //校正线程传入参数
	CButton* checkBt[MAX_CAMERAS];

	CheckToShow* checkShow; //双击后的显示问题
	ZoomImage* zoomImage;   //缩放
	ImageCalibrationDlg*  CLBdlg;
	ImageCalibrationDlg2* CLBdlg2;
	ImageCalibration* imgClb;

	HICON m_hIcon;
	int   m_CameraCount;       //实际相机个数
	BOOL bt_on;                //true:打开状态 让菜单按钮变灰

	//用于GDI+初始化函数
private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	HWND g_hwndTrackingTT;

	CPoint PointStart;
	CPoint PointEnd;
	//CRectTracker m_RectTracker;  //橡皮圈
	BOOL b_LBDown;                 //左击按下
	BOOL b_LBUp;                   //左击抬起
	int CamSign;                   //鼠标起点选中的camera标号
	int CamSignNow;                //鼠标起点处当前的顺序编号
	HINSTANCE hinst;               // piv dll 

	unsigned int test_count_getImage[8];

	CSetValue *setDlg;         //参数设置对话框
	ImageInfo myImageInfo;
	AdjustImage *adjustImage_C[MAX_CAMERAS];
	SetPivArg* setPivArg;      //piv算法参数设置
	CWaitLoading *dlgWait;     //进度条等待对话框

	MyStatusBar m_statusBar;

	pImageNode head[MAX_CAMERAS];
	pImageNode pImage[MAX_CAMERAS];

	st_Wait* st_wait;
public:
	FACTORY_HANDLE  m_hFactory;                                      // Factory Handle
	CAM_HANDLE      m_hCam[MAX_CAMERAS];                             // Camera Handle
	VIEW_HANDLE     m_hView[MAX_CAMERAS];                            // View Window handle
	THRD_HANDLE     m_hThread[MAX_CAMERAS];                          // Stream Channel handle
	int8_t          m_sCameraId[MAX_CAMERAS][J_CAMERA_ID_SIZE];      // Camera ID
	NODE_HANDLE     m_hGainNode;                                     // Handle to "GainRaw" node
	NODE_HANDLE     m_hExposureNode;                                 // Handle to "ExposureTimeRaw" node
	PIXELVALUE		m_PixelValue;
	uint64_t		m_PixelType;
	//CRITICAL_SECTION    m_CriticalSection;
	//J_tIMAGE_INFO		m_CnvImageInfo;                               // Image info structure
	
	BITMAPINFO *bmpinfo;
	int RecvFrame[MAX_CAMERAS];       //当前接收到的帧数
	CWinThread* hThread[MAX_CAMERAS]; //存图片线程

	//LOGPALETTE *pLogPal;   
	//CPalette m_hPalete;             //调色板

	unsigned int CtrLine;             //控制显示的模式2行4列 还是1行8列
 
	BOOL OpenFactoryAndCamera();                    
	void CloseFactoryAndCamera();
	void StreamCBFunc (J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc3(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc4(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc5(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc6(J_tIMAGE_INFO * pAqImageInfo);
	void StreamCBFunc7(J_tIMAGE_INFO * pAqImageInfo);

	void LiveViewWnd(CWnd* pWnd, CheckToShow* checkShow, int sign,
		J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,int nStretchMode);

	void LiveView_Cwj(J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,
		int x,int y,int scaleX,int scaleY,
		CheckToShow* checkShow,
		int nStretchMode = COLORONCOLOR);
	void LiveView_Cwj(J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,
		int x,int y,int scaleX,int scaleY,
		int nStretchMode = COLORONCOLOR);

	void InitializeControls();
	void EnableControls();

	void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	//设置b_LBDown 状态
	BOOL getLBDownState(CPoint point);


	static int SaveImage_CWJ(J_tIMAGE_INFO* pAqImageInfo, J_tIMAGE_INFO& m_CnvImageInfo,int Count,
		const char* numCam,
		const char* path = NULL, const char* fileName = NULL);

	//static UINT Write2File(LPVOID param);//类中不能加static

	int InsertHead(pImageNode pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls, 
		pImageNode pHead);

	// 		/*内部使用了imageInfo类的静态函数,使用时要包含imageInfo.h   用了Count   CCriticalSection全局变量 */
	// 	static int SaveAndDelTail(pImageNode pHead,
	// 			J_tIMAGE_INFO& m_CnvImageInfo,/* int Count,*/
	// 			const char* numCam,
	// 			const char* path = NULL, const char* fileName = NULL);

	void  insertSingle(int* Count_Single, int* Count_Once, int* Space_Frame,
		const int Copy_Space_Frame, const int CountOnce,
		pImageNode* pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls,  
		pImageNode* pHead);
	void insertBySign(BOOL bContinuous, BOOL bSingleFrame, CCriticalSection *pCs, int* Count,
		int* Count_Single, int* Count_Once, int* Space_Frame,
		const int Copy_Space_Frame, const int Copy_CountOnce,
		pImageNode* pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls,  
		pImageNode* pHead);

	void EndControl(void);
	void InitPathDir(CString dstDir, CString SubDir[], UINT_K length);//创建项目工程

private:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenCamera();
	afx_msg void OnCloseCamera();
	afx_msg void OnQuit();
	afx_msg void OnStartstream();

	afx_msg void OnSetValues();
	afx_msg void OnDestroy();
	afx_msg void On4Col2Line();
	afx_msg void On8col1line();
	afx_msg void OnContinuous();
	afx_msg void OnSingleFrame();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();

	afx_msg void OnAdjust();
	afx_msg void OnLink();
	afx_msg void OnTest();
	afx_msg void OnCreateproj();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCloseStream();
	afx_msg void OnReboot();
	afx_msg void OnLinkFlow();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnOpenproj();
	afx_msg void OnMouseLeave();
	afx_msg void OnImageCalibrate();
	afx_msg void OnImageCalibrate2();

	afx_msg LRESULT OnMyMessegeInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageInitPath(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCheckBtClick(UINT uID);
};
