// FlowNavigatorDlg.h : ͷ�ļ�
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


// CFlowNavigatorDlg �Ի���
class CFlowNavigatorDlg : public CDialog
{
// ����
public:
	CFlowNavigatorDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CFlowNavigatorDlg();
// �Ի�������
	enum { IDD = IDD_FLOWNAVIGATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

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
	int RecvFrame,RecvFrame1;//��ǰ���յ���֡��
	CWinThread* hThread[MAX_CAMERAS]; //��ͼƬ�߳�

	//LOGPALETTE *pLogPal;   
	//CPalette m_hPalete;   //��ɫ��

	unsigned int CtrLine; //������ʾ��ģʽ2��4�� ����1��8��
 
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


// ʵ��
protected:
	HICON m_hIcon;
	int   m_CameraCount;//ʵ���������
	BOOL bt_on;//true:��״̬ �ò˵���ť���

private:
	unsigned int test_count_getImage;
	
	CSetValue *setDlg;//�������öԻ���
	ImageInfo myImageInfo;
	pImageNode head[MAX_CAMERAS];
	pImageNode pImage[MAX_CAMERAS];

	// ���ɵ���Ϣӳ�亯��
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
	//static UINT Write2File(LPVOID param);//���в��ܼ�static

	int InsertHead(pImageNode pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls, 
			pImageNode pHead);

// 		/*�ڲ�ʹ����imageInfo��ľ�̬����,ʹ��ʱҪ����imageInfo.h   ����Count   CCriticalSectionȫ�ֱ��� */
// 	static int SaveAndDelTail(pImageNode pHead,
// 			J_tIMAGE_INFO& m_CnvImageInfo,/* int Count,*/
// 			const char* numCam,
// 			const char* path = NULL, const char* fileName = NULL);

};
