 // FlowNavigatorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "FlowNavigatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "BuildDir.h"
using namespace BuildDir;     //������ĿĿ¼

//PathIsDirectory()
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
//����Image
J_tIMAGE_INFO m_CnvImageInfo[MAX_CAMERAS];

//����ģʽ
// int /*Count,*/Count1,Count2,Count3,
// Count4,Count5,Count6,Count7;
int Count[MAX_CAMERAS];       //���Ʋɼ��̣߳��ɼ�����
int Count_[MAX_CAMERAS];      //����д���̣߳�д���ͼƬ���ƴ�Count_��1

//����ģʽ
int Count_Single[MAX_CAMERAS];//��������
int Count_Space[MAX_CAMERAS]; //�������

int Count_Once[MAX_CAMERAS];  //���βɼ�����
int Copy_Count_Once[MAX_CAMERAS];

int Space_Frame[MAX_CAMERAS]; //�������*��ǰ֡��
int Copy_Space_Frame[MAX_CAMERAS];

int Total_Frame[MAX_CAMERAS]; //һ������ɼ�����

BOOL b_isContinuous = FALSE;  //true:����ģʽ
BOOL b_isSingleFrame = FALSE; //true:����
CString m_CurrentProPath;     //���浱ǰ��Ŀ·��

CCriticalSection /*g_cs,*/g_cs1,g_cs2,g_cs3,
g_cs4,g_cs5,g_cs6,g_cs7;

CCriticalSection g_cs[MAX_CAMERAS];

UINT CreateWaitDlg(LPVOID pParam);
//UINT Write2File(LPVOID param);

UINT Check_ID[8] = {IDC_CheckBT1,IDC_CheckBT2,IDC_CheckBT3,IDC_CheckBT4,
IDC_CheckBT5,IDC_CheckBT6,IDC_CheckBT7,IDC_CheckBT8};

#define DOUBLECLK_IN 0
#define DOUBLECLK_OUT 1
int DoubleClk = DOUBLECLK_IN;

static UINT indicators[] = 
{
	ID_INDICATOR_INIT,
	ID_INDICATOR_SAVE_IMG,
	ID_INDICATOR_CURRENT_CAMSIGN,
	ID_INDICATOR_TIME,
};

#define INDEX_INDICATOR_INIT            0
#define INDEX_INDICATOR_SAVE_IMG        1
#define INDEX_INDICATOR_CURRENT_CAMSIGN 2
#define INDEX_INDICATOR_TIME            3
#define PRE_ID_INDICATOR_TIME     ID_INDICATOR_CURRENT_CAMSIGN

static int openCamCount = 0;       // �Ѵ����������
int        CamCount_NotNULL = -1;  //-1��δ���� 0���յ� 1���ǿ�

static int initCount = 0;          //��Ҫ�ɼ���ͼƬ����

TOOLINFO g_toolItem;
BOOL g_TrackingMouse = FALSE;

HWND CreateTrackingToolTip(int toolID, HWND hDlg, WCHAR *pText) 
{ 
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, 
		TOOLTIPS_CLASS, 
		NULL, 
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		hDlg, 
		NULL, 
		AfxGetInstanceHandle(), 
		NULL); 

	if (!hwndTT) 
	{ 
		return NULL; 
	} 

	g_toolItem.cbSize = sizeof(TOOLINFO); 
	g_toolItem.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE | TTF_SUBCLASS; 
	g_toolItem.hwnd = hDlg; 
	g_toolItem.hinst = AfxGetInstanceHandle(); 
	g_toolItem.lpszText = pText; 
	g_toolItem.uId = (UINT_PTR)hDlg; 

	GetClientRect(hDlg, &g_toolItem.rect); 

	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&g_toolItem); 

	return hwndTT; 
} 

void CreateCheckCamBt(CButton* bt[], CRect rect, CWnd *pWnd, UINT ID[])
{
	CString m_Caption;
	int xLeft;

	int yLeft =  rect.Height() / 2 - rect.Width()/8*2048/2560 / 2 - 20;
	for (int i = 0; i < 8; i++)
	{
		m_Caption.LoadString(ID[i]);
		xLeft = rect.Width()/8/2 + (rect.Width() / 8) * i - 20;
		(bt[i])->Create(m_Caption, WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX, 
			CRect(xLeft, yLeft, xLeft + 120, yLeft + 20), pWnd, ID[i]);
	}
}

void DestroyCheckCamBt(CButton *bt[], UINT ID[])
{
	for (int i = 0; i < 8; i++)
	{
		bt[i]->DestroyWindow();
	}
}

void ShowCheckCamBt(CWnd * pWnd, UINT ID[], BOOL enable)
{
	for (int i = 0; i < 8; i++)
	{
		if(pWnd->GetDlgItem(ID[i]) != NULL)
		{
			pWnd->GetDlgItem(ID[i])->ShowWindow(enable);
		}
	}
}

void EnableCheckCamBt(CButton *bt[], int maxCam, int validCam, BOOL enable)
{
	for (int i = validCam; i < maxCam; i++)
	{
		bt[i]->EnableWindow(enable);
	}
}

BOOL isCamBtCheck(UINT ID, CWnd * pWnd)
{
	CWnd * btWnd = pWnd->GetDlgItem(ID);//���ǿ�
	CButton *bt = (CButton*)btWnd;
	if(TRUE == bt->GetCheck())
	{
		return TRUE;
	}

	return FALSE;
}

void DrawLine(CWnd *pWnd, int Xstart, int Ystart, int Xend, int Yend)
{
	CClientDC dc(pWnd);
	dc.MoveTo(Xstart, Ystart);
	dc.LineTo(Xend, Yend);
}

/*void DrawGrid*/
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


// CFlowNavigatorDlg �Ի���
CFlowNavigatorDlg::CFlowNavigatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlowNavigatorDlg::IDD, pParent)
	//,b_isContinuous(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#ifdef MYTEST_LOAD_MATLAB_DLL
	//��ʼ��matlab������Ҫ�Ŀ�
	if( !mclInitializeApplication(NULL, 0) )  
	{          
		AfxMessageBox(_T("�޷���ʼ��mt��!"), MB_ICONERROR);
		exit(1);  
	}  

	if (!hFigInitialize() )  
	{  
		AfxMessageBox(_T("�޷���ʼ��mt��!"), MB_ICONERROR);
		exit(1);  
	}  
#endif
	m_hFactory       = NULL;
	m_CameraCount    = 0;
	CtrLine          = 0;
	setDlg           = NULL;
	m_CurrentProPath = DEFAULT_PATH;
	b_LBDown         = FALSE;
	b_LBUp           = FALSE;
	CamSign          = -1;
	CamSignNow       = -1;
	//Count=Count1=Count2=Count3=Count4=Count5=Count6=Count7=5;
	m_pAdjustCls     = (pAdajustCLs)malloc(sizeof(AdajustCLs));//У���̴߳������
	
	checkShow        = new CheckToShow;
	SV_AdjustArg     = NULL;
	CLBdlg           = NULL;
	CLBdlg2          = NULL;

	st_wait          = (st_Wait*)malloc(sizeof(st_Wait) + sizeof(MyStatusBar));
	st_wait->CameraCount = 0;

	// Initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	for (int i=0; i< MAX_CAMERAS; i++)
	{
		m_hCam[i]    = NULL;
		m_hView[i]   = NULL;
		m_hThread[i] = NULL;
		head[i]      = myImageInfo.CreateLink();//������������ʼ��ͷ�ڵ�
		Count[i]     = 0;
		Count_[i]    = 0;
		test_count_getImage[i] = 20;
		RecvFrame[i] = 0;
		(m_CnvImageInfo[i]).pImageBuffer = NULL;// �����ÿգ�������治����ռ�

		relateArr[i] = NULL;

		checkBt[i]   = new CButton;

		adjustImage_C[i] = new AdjustImage(CAMERA_PIXEL_ROW*CAMERA_PIXEL_COL);
	}
	imgClb = new ImageCalibration(adjustImage_C, MAX_CAMERAS);

	bmpinfo = (BITMAPINFO*)new char[256*sizeof(RGBQUAD)+ sizeof(BITMAPINFOHEADER)];
	memset(bmpinfo,0,sizeof(BITMAPINFO)); 
	bmpinfo->bmiHeader.biPlanes = 1;    
	bmpinfo->bmiHeader.biWidth  = 2560;
	bmpinfo->bmiHeader.biHeight = 2048;
	//bmpinfo->bmiHeader.biSizeImage=2560*2048*8;
	bmpinfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);


	bmpinfo->bmiHeader.biCompression = BI_RGB;      
	bmpinfo->bmiHeader.biBitCount    = 8;
	bmpinfo->bmiHeader.biClrUsed     = 0;//Ϊ0����2^biBitCount
	//bmpinfo->bmiHeader.biClrImportant=256;


	for( int j = 0; j < 256; j++ )
	{
		bmpinfo->bmiColors[j].rgbBlue     = (BYTE) (0xff & j);
		bmpinfo->bmiColors[j].rgbGreen    = (BYTE)j;  // bmpinfo->bmiColors[j].rgbBlue;
		bmpinfo->bmiColors[j].rgbRed      = (BYTE)j;   // bmpinfo->bmiColors[j].rgbBlue;
		bmpinfo->bmiColors[j].rgbReserved = 0;
	}

//     pLogPal = (LOGPALETTE *)new char[2*sizeof(WORD) + 256*sizeof(PALETTEENTRY)];
// 	pLogPal->palVersion = 0x300;
// 	pLogPal->palNumEntries = 256;
// 	for (int j = 0; j < 256; j++)
// 	{
// 		pLogPal->palPalEntry[j].peRed = (BYTE)j;
// 		pLogPal->palPalEntry[j].peGreen = (BYTE)j;
// 		pLogPal->palPalEntry[j].peBlue = (BYTE)j;
// 		pLogPal->palPalEntry[j].peFlags = 0;
// 	}
// 	m_hPalete.CreatePalette(pLogPal);
	// We use a critical section to protect the update of the pixel value display
	//InitializeCriticalSection(&m_CriticalSection);


	hinst = LoadLibraryEx(DEFAULT_PIV_DLL_LINK_PATH, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);//��һ��������׺���Բ���...
	int result = GetLastError();

	if(result != 0)
	{
		CString txt;
		txt.Format(_T("load lib GetLastError=%d"), result);
		AfxMessageBox(txt);
	}
}

CFlowNavigatorDlg::~CFlowNavigatorDlg()
{
	delete bmpinfo;
	delete showView;
	delete checkShow;
	delete zoomImage;
	delete imgClb;
	delete setDlg;
	if (m_pAdjustCls != NULL)
	{
		free(m_pAdjustCls);
		m_pAdjustCls = NULL;
	}
	
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		free(head[i]);
		head[i] = NULL;

		if (checkBt[i] != NULL)
		{
			delete checkBt[i];
		}

		delete adjustImage_C[i];
		adjustImage_C[i] = NULL;
	}
	FreeLibrary(hinst);

#ifdef MYTEST_LOAD_MATLAB_DLL
	hFigTerminate();  
	mclTerminateApplication();  
#endif

}

void CFlowNavigatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFlowNavigatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	//}}AFX_MSG_MAP
	ON_COMMAND(ID_OpenCamera, &CFlowNavigatorDlg::OnOpenCamera)
	ON_COMMAND(ID_CloseCamera, &CFlowNavigatorDlg::OnCloseCamera)
	ON_COMMAND(ID_Quit, &CFlowNavigatorDlg::OnQuit)
	ON_COMMAND(ID_32775, &CFlowNavigatorDlg::OnSetValues)
	ON_COMMAND(ID_4Col2Line, &CFlowNavigatorDlg::On4Col2Line)
	ON_COMMAND(ID_8Col1Line, &CFlowNavigatorDlg::On8col1line)
	ON_COMMAND(ID_Continuous, &CFlowNavigatorDlg::OnContinuous)
	ON_COMMAND(ID_SingleFrame, &CFlowNavigatorDlg::OnSingleFrame)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_Adjust, &CFlowNavigatorDlg::OnAdjust)
	ON_COMMAND(ID_Link, &CFlowNavigatorDlg::OnLink)
	ON_COMMAND(ID_Test, &CFlowNavigatorDlg::OnTest)
	ON_COMMAND(ID_CreateProj, &CFlowNavigatorDlg::OnCreateproj)
	ON_COMMAND(ID_StartStream, &CFlowNavigatorDlg::OnStartstream)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_CloseStream, &CFlowNavigatorDlg::OnCloseStream)
	ON_COMMAND(ID_Reboot, &CFlowNavigatorDlg::OnReboot)
	ON_COMMAND(ID_LinkFlow, &CFlowNavigatorDlg::OnLinkFlow)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDBLCLK()
	ON_COMMAND(ID_OpenProj, &CFlowNavigatorDlg::OnOpenproj)
	ON_WM_MOUSELEAVE()
	ON_COMMAND(ID_ImageCalibrate, &CFlowNavigatorDlg::OnImageCalibrate)
	ON_COMMAND(ID_ImageCalibrate2, &CFlowNavigatorDlg::OnImageCalibrate2)

	ON_MESSAGE(WM_MY_MESSAGE_INIT, OnMyMessegeInit)     //�Զ�����Ϣ ϵͳ�򿪽���
	ON_COMMAND_RANGE(IDC_CheckBT1, IDC_CheckBT8, OnCheckBtClick)
END_MESSAGE_MAP()


// CFlowNavigatorDlg ��Ϣ�������

BOOL CFlowNavigatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	InitializeControls();

	m_PixelValue.PixelValueUnion.Mono16Type.Value = 0;
	m_PixelType = J_GVSP_PIX_MONO8; 

	ShowWindow(SW_MAXIMIZE);

	zoomImage        = new ZoomImage(this);
	showView         = new ShowView(this); 

	g_hwndTrackingTT = CreateTrackingToolTip(IDOK, m_hWnd, _T("ToolTip")); 

	if(!m_statusBar.CreateEx(this, SBARS_SIZEGRIP) || 
		!m_statusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
	{
		TRACE(_T("fail to create status bar!\n"));

		return -1;
	}
	//����״̬���߶�
	m_statusBar.GetStatusBarCtrl().SetMinHeight(30);
	CRect rcClientOld;
	CRect rcClientNew;
	GetClientRect(&rcClientOld);
	//m_statusBar.MoveWindow(0,rcClientOld.bottom-20,rcClientOld.right,20);// ����״̬����λ�úʹ�С
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery,rcClientNew);

	CRect rcWindow;
	GetWindowRect(rcWindow);
	// �޸ĶԻ����Ⱥ͸߶�,���öԻ�������� CStatusBar �ĸ߶Ⱥ���Ӧ CStatusBar �Ŀ��
	rcWindow.right  += rcClientOld.Width() - rcClientNew.Width();   
    rcWindow.bottom += rcClientOld.Height() - rcClientNew.Height();

// 	rcWindow.right  += 20;
// 	rcWindow.bottom += 100;
	// �ƶ��Ի���
	MoveWindow(rcWindow,TRUE); 
	m_statusBar.SetPaneInfo(INDEX_INDICATOR_TIME - 1,  PRE_ID_INDICATOR_TIME, SBPS_STRETCH, 20);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	SetTimer(SETTIMER_ID_SYSTEM_TIME, 1000, NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFlowNavigatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFlowNavigatorDlg::OnPaint()
{
	CRect rect;
	GetClientRect(&rect);

	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();//����ԶҲ�벻������ע�͵�����ʱ��ֻ������С��ʱ�����аɣ���ôʹ������wtf��   _kui
	}
// 	CRect k_rect;
// 	GetClientRect(k_rect);
// 	CPaintDC k_dc(this);
// 	k_dc.SetMapMode(MM_ANISOTROPIC);
// 	k_dc.SetWindowExt(500,500);
// 	k_dc.SetViewportExt(k_rect.Width(),k_rect.Height());
// 	k_dc.Ellipse(0,0,500,500);

	int showWidth = rect.Width() / 8;
	int showHeight = showWidth * 2048 / 2560; // ��ʾ�߶�
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if((CtrLine == 81) && (DoubleClk == DOUBLECLK_IN) && (CloseSystem == FALSE) && (mAdjust != TRUE))
	{
		DrawLine(this, rect.left, showStartHeight, rect.right, showStartHeight);
		DrawLine(this, rect.left, showStartHeight + showHeight, rect.right, showStartHeight + showHeight);
		for (int i = 0; i < MAX_CAMERAS; i++)
		{
			DrawLine(this, i * showWidth, showStartHeight, i * showWidth, showStartHeight + showHeight);
		}
	}
}
 
//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFlowNavigatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CFlowNavigatorDlg::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;
	uint32_t        iNumDev;
	bool8_t         bHasChange;

	// Open factory
	retval = J_Factory_Open((int8_t*)"" , &m_hFactory);
	if (retval != J_ST_SUCCESS)
	{
		AfxMessageBox(CString("Could not open factory!"));
		return FALSE;
	}
	TRACE("Opening factory succeeded\n");

	// Update camera list
	retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
	if (retval != J_ST_SUCCESS)
	{
		AfxMessageBox(CString("Could not update camera list!"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	TRACE("Updating camera list succeeded\n");

	// Get the number of Cameras
	retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
	if (retval != J_ST_SUCCESS)
	{
		AfxMessageBox(CString("Could not get the number of cameras!"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (iNumDev == 0)
	{
		AfxMessageBox(CString("There is no camera!"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	TRACE("%d cameras were found\n", iNumDev);

	// We only want to get MAX_CAMERAS cameras connected at a time
	// and we assume that iNumDev is the actual camera count*2 because we assume
	// that we have 2 drivers active (SockerDriver+FilerDriver)

	bool bFdUse = false;
	int	iValidCamera = 0;

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	int64_t int64Val;
	SIZE	ViewSize;

	for (int i=0; i< (int)iNumDev; i++)
	{
		// Get camera IDs
		iSize = J_CAMERA_ID_SIZE;
		m_sCameraId[iValidCamera][0] = 0;
		retval = J_Factory_GetCameraIDByIndex(m_hFactory, i, m_sCameraId[iValidCamera], &iSize);
		if (retval != J_ST_SUCCESS)
		{
			AfxMessageBox(CString("Could not get the camera ID!"), MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
		TRACE("Camera ID[%d]: %s\n", i, m_sCameraId[iValidCamera]);

		if(0 == strncmp("TL=>GevTL , INT=>FD", (char*)m_sCameraId[iValidCamera], 19))//�����ַ����м䶺�����඼�пո�\0��Ҳ����Ƚ�
		{ // FD
			bFdUse = true;
			// Open camera
			retval = J_Camera_Open(m_hFactory, m_sCameraId[iValidCamera], &m_hCam[iValidCamera]);
			if (retval != J_ST_SUCCESS)
			{
				AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}
			iValidCamera++;
			TRACE("Opening camera %d succeeded\n", iValidCamera);			
		}
		else
		{ // SD
			if(bFdUse == false)
			{
				// Open camera
				retval = J_Camera_Open(m_hFactory, m_sCameraId[iValidCamera], &m_hCam[iValidCamera]);
				if (retval != J_ST_SUCCESS)
				{
					AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}
				iValidCamera++;
				TRACE("Opening camera %d succeeded\n", iValidCamera);
			}
			else
			{
				;
			}
		}
		if(iValidCamera >= MAX_CAMERAS)
			break;
	}

	m_CameraCount = iValidCamera > MAX_CAMERAS ? MAX_CAMERAS : iValidCamera;
	TRACE("iValidCamera = %d MAX_CAMERAS = %d m_CameraCount = %d\n",iValidCamera,MAX_CAMERAS,m_CameraCount);

	st_wait->CameraCount = m_CameraCount;//�򿪽����߳�
	CamCount_NotNULL     = 1;
	NODE_HANDLE hNode;
	int I = 0;
	CString paneText;

	for(int j = 0; j < m_CameraCount; j++)
	{
		// Get the pixelformat from the camera
		retval = J_Camera_GetValueInt64(m_hCam[j], NODE_NAME_PIXELFORMAT, &pixelFormat);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
			return FALSE;
		}
		J_Image_Get_PixelFormat(m_hCam[j], pixelFormat, &jaiPixelFormat);

		//Set frame grabber pixel format, if applicable
		//SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

		// Calculate number of bits (not bytes) per pixel using macro
		int bpp = J_BitsPerPixel(jaiPixelFormat);

		TRACE("kui bpp : %d\n",bpp);

		//���ó�ʼ֡��ֵ������Ĭ��22֡��8̨ͬʱ��������
	    J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[j],(int8_t *)"AcquisitionFrameRate",&hNode);
		J_Node_SetValueDouble(hNode,TRUE,3.0);
		
        
		// Open stream
		switch(j)
		{
			case 0:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 1:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc1),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 2:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc2),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 3:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc3),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 4:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc4),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 5:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc5),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 6:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc6),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			case 7:
				retval = J_Image_OpenStream(m_hCam[j], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
					reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CFlowNavigatorDlg::StreamCBFunc7),
					&m_hThread[j], (K_INIT_WIDTH*K_INIT_HEIGH*bpp)/8);
				break;
			default:
				;
		}
		// Open stream
		if (retval != J_ST_SUCCESS) {
			ShowErrorMsg(CString("Could not open stream!"), retval);
			return FALSE;
		}
		TRACE("Opening stream succeeded\n");

		openCamCount++;
		I = openCamCount * 100 / (st_wait->CameraCount);

	    paneText.Format(_T("ϵͳ״̬:�����%d%%"), I);
	    m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, paneText, TRUE);

	}//	for(int j = 0; j < m_CameraCount; j++)
	for (int k = 0; k < m_CameraCount;k++)
	{
		// Start Acquisition
		retval = J_Camera_ExecuteCommand(m_hCam[k], NODE_NAME_ACQSTART);
		if (retval != J_ST_SUCCESS) {
			ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
			return FALSE;
		}
	}

	return TRUE;
}

void CFlowNavigatorDlg::CloseFactoryAndCamera()
{
	J_STATUS_TYPE retval;
	for (int i = 0; i < m_CameraCount; i++)
	{
		if (m_hThread[i] != NULL)
		{
			retval = J_Image_CloseStream(m_hThread[i]);
			if (retval != J_ST_SUCCESS) {
				ShowErrorMsg(CString("Could not close Stream!"), retval);
			}
			m_hThread[i]= NULL;
			TRACE("Closed stream %d\n",i);

			
		}

		if (m_hCam[i] != NULL)
		{
			retval = J_Camera_ExecuteCommand(m_hCam[i],NODE_NAME_ACQSTOP);
			if (retval != J_ST_SUCCESS) {
				ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
			}

			retval = J_Camera_Close(m_hCam[i]);
			if (retval != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("Could not close camera!"), retval);
			}			
			m_hCam[i] = NULL;
			TRACE("Closed camera %d\n",i);

			openCamCount--;
		}
	}
	if(m_hFactory != NULL)
	{
		retval = J_Factory_Close(m_hFactory);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not close factory!"), retval);
		}
		m_hFactory = NULL;
		TRACE("Close factory\n");
	}
}


/*�����ʵ��һ������Ĵ��룬���ҳ�������ͼ����Ŀ���
void CFlowNavigatorDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{

	TRACE("Line%d:get image%d\n",__LINE__,test_count_getImage++);
#if KDebug

	//J_Image_ShowImage(::GetDC(this->m_hWnd),pAqImageInfo);

#endif
	
	//CClientDC dcScreen(this);

// 	BYTE *pData;
// 	pData = new BYTE[2560*2048];
    
	//�����Ǹ�ֵ��λͼ���飬��ô��û��Ҫ
// 	int k = 0;
// 	int lineByte=(2560+3)/4*4;          //int lineByte=(width*biBitCount/8+3)/4*4;
// 	for (int i = 0; i < 2048; i++)
// 	{
// 		for (int j = 0; j < 2560; j++)
// 		{
// 			pData[k] = *(pAqImageInfo->pImageBuffer + i*lineByte + j);
// 			k++;
// 		}
// 	}
// 	for(int i = 0; i < 5242880; i++)
// 	{
// 		pData[i] = *(pAqImageInfo->pImageBuffer + i);
// 	}
	
	//bmpinfo Ҫ����ռ�
// 	BITMAPINFO bmpinfo={0};
// 	//ZeroMemory(&bmpinfo,sizeof(BITMAPINFO)); 
// 	bmpinfo->bmiHeader.biPlanes = 1;    
// 	bmpinfo->bmiHeader.biWidth=2560;
// 	bmpinfo->bmiHeader.biHeight=2048;
// 	//bmpinfo->bmiHeader.biSizeImage=2560*2048*8;
// 	bmpinfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
// 
// 
// 	bmpinfo->bmiHeader.biCompression =BI_RGB;      
// 	bmpinfo->bmiHeader.biBitCount=8;
// 	bmpinfo->bmiHeader.biClrUsed=256;
// 	//bmpinfo->bmiHeader.biClrImportant=256;
// 	for( int j = 0; j < 256; j++ )
// 	{
// 	//  		bmpinfo->bmiColors[j].rgbBlue =  (BYTE) (0xff & j);
// 	//  		bmpinfo->bmiColors[j].rgbGreen = bmpinfo->bmiColors[j].rgbBlue;
// 	//  		bmpinfo->bmiColors[j].rgbRed = bmpinfo->bmiColors[j].rgbBlue;
// 	//  		bmpinfo->bmiColors[j].rgbReserved = 0;
// 	}
	


	CDC * pDC = GetDC();
// 	int nc = pDC->GetDeviceCaps(SIZEPALETTE);
// 	BOOL btext = (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) <= 256;
	CDC MemDC;

	if(!MemDC.CreateCompatibleDC(pDC))
	{
		ReleaseDC(pDC);
		return;
	}

//  	CPalette *pOldPalette = pDC->SelectPalette(&m_hPalete,FALSE);
//  	pDC->RealizePalette();
	

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight);
	CBitmap* pOldBmp = MemDC.SelectObject(&bmp);
	::SetDIBitsToDevice(MemDC.GetSafeHdc(),
		0,0,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight,
		0,0,0,
		bmpinfo->bmiHeader.biHeight,
		(pAqImageInfo->pImageBuffer),
		bmpinfo,
		DIB_RGB_COLORS);
	//pDC->BitBlt(1,1,2560,2048,&MemDC,0,0,SRCCOPY);

	CRect rect;
	GetClientRect(&rect);

	//��䲻��ͼƬʧ��
 	pDC->SetStretchBltMode(HALFTONE);//COLORONCOLOR��ֱ�ӌ���Ҫ�ĵ�ɾ��.��
	                                  //HALFTONE��Ҫɾ�������ظ���Χ���к�����,�ɵõ��ϺõĻ��� ����Ч��Ҫ������
	pDC->StretchBlt(0,0,rect.Width()/4,rect.Height()/2,&MemDC,0,0,
		bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight,
		SRCCOPY);
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
	bmp.DeleteObject();
	ReleaseDC(pDC);
 
// 	delete []pData;
// 	pData = NULL;

// // 	    CDC * cdc = GetDC();
// // 		HDC g_hDC=cdc->GetSafeHdc();
// 
// 		HDC g_hDC = ::GetDC(this->m_hWnd);
// 
// 		SetDIBitsToDevice(
// 			g_hDC,
// 			0,0,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight,
// 			0,0,0,
// 			bmpinfo->bmiHeader.biHeight,
// 			(LPVOID)(pData),
// 			&bmpinfo,
// 			DIB_RGB_COLORS);
// 		::ReleaseDC(this->m_hWnd,g_hDC);
// 		delete pData;
// 		pData = NULL;	
}
*/
int SaveImage(CString CurrentProPath, 
			  J_tIMAGE_INFO* pAqImageInfo, J_tIMAGE_INFO& m_CnvImageInfo,
			  int* Count,const char* numCam, 
			  const char* path = NULL, const char* fileName = NULL)
{
	if(*Count > 0)
	{
		char path[MAX_PATH]={0};
		if(m_CurrentProPath == DEFAULT_PATH)
		{
			CFlowNavigatorDlg::SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo,*Count,numCam);		
		}
		else
		{
			CString tempPath = m_CurrentProPath + _T("\\ԭʼ����");
			int nLength = WideCharToMultiByte(CP_ACP, 0, tempPath, tempPath.GetLength(),
				NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, tempPath, tempPath.GetLength(), path, nLength, NULL, NULL);
			//wcstombs(path, m_CurrentProPath, m_CurrentProPath.GetLength());//���ֱ��������漸�仰
			CFlowNavigatorDlg::SaveImage_CWJ(pAqImageInfo, m_CnvImageInfo, *Count, numCam , path);
		}

		(*Count)--;
	}

	return 0;
}
int SaveImageBySign(BOOL bContinuous, BOOL bSingleFrame, 
					int* Count_Continuous, int* Count_Single,
					const char* numCamConti, const char* numCamSingle, 
					CString CurrentProPath, J_tIMAGE_INFO* pAqImageInfo, J_tIMAGE_INFO& m_CnvImageInfo,
					const char* path = NULL, const char* fileName = NULL)
{
	if (bContinuous)
	{
		SaveImage(CurrentProPath, pAqImageInfo, m_CnvImageInfo, Count_Continuous, numCamConti);

		if (*Count_Continuous == 0)
		{
			CString camSign(numCamConti);
			CString text;
			text.Format(_T("���%s�ɼ����"), camSign);
			AfxMessageBox(text, MB_ICONINFORMATION);
			return -1;
		}
	}
	else if (bSingleFrame)
	{
		SaveImage(m_CurrentProPath, pAqImageInfo, m_CnvImageInfo, Count_Single, numCamSingle);

		if (*Count_Single == 0)
		{
			CString camSign(numCamConti);
			CString text;
			text.Format(_T("���%s�ɼ����"), camSign);
			AfxMessageBox(text, MB_ICONINFORMATION);

			return -1;
		}
	}

	return 0;
}

int CFlowNavigatorDlg::SaveImage_CWJ(J_tIMAGE_INFO* pAqImageInfo, J_tIMAGE_INFO& m_CnvImageInfo, int Count,
									 const char* numCam, 
									 const char* path, const char* fileName)
{
	//���ƶ��̷߳��ʣ�����������ͼ�ᴮ���������е�ͼ�������Ǽ���ͼƬ����һ��������//���ڹ���ȫ�ֱ���
	//EnterCriticalSection(&m_CriticalSection);

	CString filePath;
	TCHAR str[MAX_PATH];
	//J_tIMAGE_INFO **ppAqImageInfo = new J_tIMAGE_INFO *[MAX_IMG_COUNT];
	/*
	for (int i = 0; i < Count; i++)
	{
//		J_tIMAGE_INFO *ppAqImageInfo[i] = 
	}
	*/

	/*����ע���ĵ�����ͬLiveView_CWJ��ʹ��ʱ������*/
	//J_Image_Flip(pAqImageInfo,J_FLIP_VERTICAL);
	// Allocate the buffer to hold converted the image. (We only want to do this once for performance reasons)
	if (m_CnvImageInfo.pImageBuffer == NULL)
	{
		if (J_Image_Malloc(pAqImageInfo, &m_CnvImageInfo) != J_ST_SUCCESS)
			return -1;
	}

	if (m_CnvImageInfo.pImageBuffer != NULL)
	{
		// Convert the raw image to "normalized" image format
		if (J_Image_FromRawToImage(pAqImageInfo, &m_CnvImageInfo) == J_ST_SUCCESS)
		{
			if(path == NULL)
			{
				//�õ�exe������·��������exe���ļ���
				GetModuleFileName(NULL,str,MAX_PATH); //ԭ�������� LPWCH(str) ���·����ÿ���ַ�����˸� 0 ������
				CString pathDir(str);
				int pos = pathDir.ReverseFind('\\');
				pathDir = pathDir.Left(pos);

				CString strNumCam(numCam);
				pathDir = pathDir + _T("\\")+ strNumCam;
				if(!PathIsDirectory(pathDir))
				{
					::CreateDirectory(pathDir,NULL);
				}

				filePath.Format(_T("%s\\%d.tiff"),pathDir,Count);
				//filePath.Replace(_T("\\"),_T("\\\\"));

				//��ΪLPCSTR��ANSI��׼�µģ������ַ�����Unicode
                USES_CONVERSION;
				J_Image_SaveFileA(&m_CnvImageInfo, (LPCSTR)T2A(filePath)); // Stores RGB image to a TIFF file.
			}
			else
			{
				char tempPath[MAX_PATH] = {0};
				strcpy(tempPath, path);
				sprintf(tempPath, "%s\\%s", tempPath, numCam);
				CString s_tempPath(tempPath);
				if(!PathIsDirectory(s_tempPath))
				{
					::CreateDirectory(s_tempPath,NULL);
				}

				sprintf(tempPath, "%s\\%d.tiff", tempPath, Count);
				
				J_Image_SaveFileA(&m_CnvImageInfo, tempPath);
			}
			//J_Image_SaveFileA(&m_CnvImageInfo, (LPCSTR)"D:\\2.tiff"); // Stores RGB image to a TIFF file.
		}
	}
	//LeaveCriticalSection(&m_CriticalSection);

	return 0;
	
}

int CFlowNavigatorDlg::InsertHead(pImageNode pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls, 
								  pImageNode pHead)
{
	pImage = (pImageNode)malloc(sizeof(ImageNode) + pAqImageInfo->iImageSize);
	//����ʧ��
	if(-1 == ImageInfo::IsMallocOk(pImage))
	{
		return -1;
	}

	(pImage)->AqImageInfo = *pAqImageInfo;
	(pImage)->pImageBuffer = pAqImageInfo->pImageBuffer;

	//g_cs.Lock();
	ImageInfo::InsertHead(pHead,pImage);
	//g_cs.Unlock();
	return 0;
}
void DrawTextByCamSign(CWnd* pWnd, ShowView* pShowView, int recv, int lost, int X, int Y)
{
	CDC *pDc = pWnd->GetDC();
	HDC hdc = pDc->GetSafeHdc();
	st_DrawText pDrawText;
	pDrawText.graph = Graphics::FromHDC(hdc)/*::new Graphics(hdc)*/;//������new delete�ͱ�����
	pDrawText.Mode = TextRenderingHintClearTypeGridFit;// Set the text rendering for Cleartype
	pDrawText.a = 255;
	pDrawText.r = 255;
	pDrawText.g = 0;
	pDrawText.b = 0;
	pDrawText.pWnd = pWnd;
	pDrawText.X = X;
	pDrawText.Y = Y;
	pDrawText.showStr.Format(_T("����%d ��%lu"), recv, lost);

	pShowView->DrawText_k(&pDrawText);

	delete pDrawText.graph;
	pDrawText.graph = NULL;
	pWnd->ReleaseDC(pDc);
}
void DrawTextLocate(CheckToShow* checkShow, int sign, CWnd* pWnd, ShowView* pShowView, int recv, int lost)
{
	CRect rect;
	pWnd->GetClientRect(&rect);
	int Weight      = checkShow->ReturnWeight(sign);
	int SrcWidth    = checkShow->getWidth(rect);
	int startHeight = checkShow->getStartHeight(rect);
	DrawTextByCamSign(pWnd,pShowView,recv,lost,Weight*SrcWidth,startHeight);
}
void  CFlowNavigatorDlg::insertSingle(int* Count_Single, int* Count_Once, int* Space_Frame,
									  const int Copy_Space_Frame, const int Copy_CountOnce,
									  pImageNode* pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls,  
									  pImageNode* pHead)
{
	if (*Count_Single > 0)
	{
		if (*Count_Once > 0)
		{
			if(-1 == InsertHead(*pImage,pAqImageInfo,cls,*pHead))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}

			(*Count_Once)--;
			if (*Count_Once == 0)
			{
				(*Count_Single)--;
			}
		}
		else if (*Space_Frame > 0)
		{
			(*Space_Frame)--;
			if (*(Space_Frame) == 0)
			{
				*Count_Once = Copy_CountOnce;
				*(Space_Frame) = Copy_Space_Frame;
			}
		}
	}
}
void CFlowNavigatorDlg::insertBySign(BOOL bContinuous, BOOL bSingleFrame, CCriticalSection *pCs, int* Count,
				  int* Count_Single, int* Count_Once, int* Space_Frame,
				  const int Copy_Space_Frame, const int Copy_CountOnce,
				  pImageNode* pImage, J_tIMAGE_INFO* pAqImageInfo, ImageInfo cls,  
				  pImageNode* pHead)
{
	if(bContinuous)
	{
		if (*Count > 0)
		{
			(*pCs).Lock();
			if(-1 == InsertHead(*pImage,pAqImageInfo,cls,*pHead))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			(*Count)--;
			(*pCs).Unlock();
		}
	}
	else if (bSingleFrame)
	{
		(*pCs).Lock();
		insertSingle(Count_Single,Count_Once,Space_Frame,
			Copy_Space_Frame,Copy_CountOnce,
			pImage, pAqImageInfo, cls, pHead);
		(*pCs).Unlock();
	}
}

void CFlowNavigatorDlg::StreamCBFunc(J_tIMAGE_INFO *pAqImageInfo)
{
	int CameraSign = 0;

	float begin = clock();

	if(GetDlgItem(Check_ID[0]) != NULL)
	{
		if((FALSE == isCamBtCheck(Check_ID[0], this)) || CtrLine == 0)
		{
			goto End;
		}
	}

	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[0])++;

	CRect rect;
	GetClientRect(&rect);

	int showHeight = rect.Width() / 8 *2048/2560; // ��ʾ�߶�
	int showStartHeight = rect.Height() / 2 - showHeight / 2;
	if (mAdjust)
	{ 
		if (!AdjustING)
		{
			adjustImage_C[0]->setImageInfo(pAqImageInfo);
		}
		if (Pair[0] > 0)
		{
			adjustImage_C[0]->setPairImageInfo(&Pair[0], pAqImageInfo);
			(Pair[0])--;
		}
		goto End;
	} 
	else 
	{
		//End:
		if(CtrLine == 42)
		{
			LiveView_Cwj(pAqImageInfo,bmpinfo,0,0,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,0,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,0,pAqImageInfo,bmpinfo,HALFTONE);
			}
		}	
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[0], &Count[0],
			&(Count_Single[0]),&(Count_Once[0]), &(Space_Frame[0]),
			Copy_Space_Frame[0],Copy_Count_Once[0],
			&(pImage[0]), pAqImageInfo, myImageInfo, &(head[0]));
		// 		if(b_isContinuous)
		// 		{
		// 			if (Count > 0)
		// 			{
		// 				float begin = clock();
		// 				g_cs.Lock();
		// 				//���յ����ݲ�����ͷ��������β�������̶߳�������ɾ���ڵ�
		// 				if(-1 == InsertHead(pImage[0],pAqImageInfo,myImageInfo,head[0]))
		// 				{
		// 					AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 					OnClose();
		// 				}
		// 				Count--;
		// 				g_cs.Unlock();
		// 				float end = clock();
		// 				TRACE("InsertHead:%f\n",end - begin);//2����
		// 			}
		// 		}
		// 		else if (b_isSingleFrame)
		// 		{
		// 			g_cs.Lock();
		// 			insertSingle(&(Count_Single[0]),&(Count_Once[0]), &(Space_Frame[0]),
		// 				Copy_Space_Frame[0],Copy_Count_Once[0],
		// 				&(pImage[0]), pAqImageInfo, myImageInfo, &(head[0]));
		// 			g_cs.Unlock();
		// 		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[0], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);
	iSize = sizeof(uint64_t);
	// get the number of frames which has been corrupted 
	J_Image_GetStreamInfo(m_hThread[0], STREAM_INFO_CMD_NUMBER_OF_FRAMES_CORRUPT_ON_DELIEVRY, &iCurruptedFrames, &iSize);
	TRACE("���0��%d,LostFrames=%d,CurruptedFrames=%d,RecvFrame=%d,iTimeStamp=%llu,iAwaitDelivery=%d\n",Count,iLostFrames,
		iCurruptedFrames,RecvFrame[0],pAqImageInfo->iTimeStamp,pAqImageInfo->iAwaitDelivery);

	if (CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[0], iLostFrames, 0, 0);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[0], iLostFrames, 0, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,0,this,showView,RecvFrame[0],iLostFrames);
		}
	}
	//Graphics g(this->GetDC()->GetSafeHdc());
	// 	CDC *pDc = this->GetDC();
	// 	HDC hdc = pDc->GetSafeHdc();
	// 	st_DrawText pDrawText;
	// 	pDrawText.graph = Graphics::FromHDC(hdc)/*::new Graphics(hdc)*/;//������new delete�ͱ�����
	// 	pDrawText.Mode = TextRenderingHintClearTypeGridFit;// Set the text rendering for Cleartype
	// 	pDrawText.a = 255;
	// 	pDrawText.r = 255;
	// 	pDrawText.g = 0;
	// 	pDrawText.b = 0;
	// 	pDrawText.pWnd = this;
	// 	pDrawText.X = 0;
	// 	pDrawText.Y = 0;
	// 	pDrawText.showStr.Format(_T("����%d ��%lu"), RecvFrame[0],iLostFrames);
	// 
	// 	showView->DrawText_k(&pDrawText);
	// 
	// 	delete pDrawText.graph;
	// 	pDrawText.graph = NULL;
	// 	ReleaseDC(pDc);
End:
	float end = clock();
	//TRACE("LiveView coast time = %f\n",end - begin);
}

void CFlowNavigatorDlg::StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo)
{
	int CameraSign = 1;

	if(GetDlgItem(Check_ID[1]) != NULL)
	{
		if((FALSE == isCamBtCheck(Check_ID[1], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[1])++;

	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[1]->setImageInfo(pAqImageInfo);
		}
		if (Pair[1] > 0)
		{
			adjustImage_C[1]->setPairImageInfo(&Pair[1], pAqImageInfo);
			(Pair[1])--;
		}
		goto End;
	} 
	else
	{
		//End1:
		if(CtrLine == 42)
		{
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4,0,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,1,pAqImageInfo,bmpinfo,HALFTONE);
			}

			//LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[1], &Count[1],
			&(Count_Single[1]),&(Count_Once[1]), &(Space_Frame[1]),
			Copy_Space_Frame[1],Copy_Count_Once[1],
			&(pImage[1]), pAqImageInfo, myImageInfo, &(head[1]));
// 		if(b_isContinuous)
// 		{
// 			if(Count1 > 0){
// 				g_cs1.Lock();
// 				if(-1 == InsertHead(pImage[1],pAqImageInfo,myImageInfo,head[1]))
// 				{
// 					AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
// 					OnClose();
// 				}
// 				Count1--;
// 				g_cs1.Unlock();
// 				//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo1,Count1,"1");
// 			}
// 		}
	}

	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[1], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if(CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[1], iLostFrames, rect.Width() / 4, 0);
	}
	else if (CtrLine == 81)
	{	
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[1], iLostFrames, rect.Width() / 8 * 1, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,1,this,showView,RecvFrame[1],iLostFrames);
		}
	}

End:
	;
}

void CFlowNavigatorDlg::StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo)
{	
	int CameraSign = 2;

	if(GetDlgItem(Check_ID[CameraSign]) != NULL)
	{
		if((FALSE == isCamBtCheck(Check_ID[CameraSign], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[CameraSign])++;
	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[CameraSign]->setImageInfo(pAqImageInfo);
		}
		if (Pair[CameraSign] > 0)
		{
			adjustImage_C[CameraSign]->setPairImageInfo(&Pair[CameraSign], pAqImageInfo);
			(Pair[CameraSign])--;
		}
		goto End;
	} 
	else
	{

		if (CtrLine == 42)
		{
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/2,0,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*2,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,2,pAqImageInfo,bmpinfo,HALFTONE);
			}
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[CameraSign], &Count[CameraSign],
			&(Count_Single[CameraSign]),&(Count_Once[CameraSign]), &(Space_Frame[CameraSign]),
			Copy_Space_Frame[CameraSign],Copy_Count_Once[CameraSign],
			&(pImage[CameraSign]), pAqImageInfo, myImageInfo, &(head[CameraSign]));
		// 	if(b_isContinuous)
		// 	{
		// 		if(Count2 > 0){
		// 			g_cs2.Lock();
		// 			if(-1 == InsertHead(pImage[2],pAqImageInfo,myImageInfo,head[2]))
		// 			{
		// 				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 				OnClose();
		// 			}
		// 			Count2--;
		// 			g_cs2.Unlock();
		// 			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo2,Count2,"2");
		// 			TRACE("���2��%d\n",Count2);
		// 		}
		// 	}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[2], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if(CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[2], iLostFrames, rect.Width()/ 2, 0);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[2], iLostFrames, rect.Width() / 8 * 2, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,2,this,showView,RecvFrame[2],iLostFrames);
		}	}
End:
	;
}
void CFlowNavigatorDlg::StreamCBFunc3(J_tIMAGE_INFO * pAqImageInfo)
{
	int CameraSign = 3;
	if(GetDlgItem(Check_ID[3]) != NULL){
		if((FALSE == isCamBtCheck(Check_ID[3], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[3])++;
	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[CameraSign]->setImageInfo(pAqImageInfo);
		}
		if (Pair[CameraSign] > 0)
		{
			adjustImage_C[CameraSign]->setPairImageInfo(&Pair[CameraSign], pAqImageInfo);
			(Pair[CameraSign])--;
		}
		goto End;
	} 
	else
	{

		if(CtrLine == 42)
		{
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4*3,0,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*3,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,3,pAqImageInfo,bmpinfo,HALFTONE);
			}	
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[CameraSign], &Count[CameraSign],
			&(Count_Single[CameraSign]),&(Count_Once[CameraSign]), &(Space_Frame[CameraSign]),
			Copy_Space_Frame[CameraSign],Copy_Count_Once[CameraSign],
			&(pImage[CameraSign]), pAqImageInfo, myImageInfo, &(head[CameraSign]));
		// 	if(b_isContinuous)
		// 	{
		// 		if(Count3 > 0){
		// 			g_cs3.Lock();
		// 			if(-1 == InsertHead(pImage[3],pAqImageInfo,myImageInfo,head[3]))
		// 			{
		// 				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 				OnClose();
		// 			}
		// 			Count3--;
		// 			g_cs3.Unlock();
		// 			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo3,Count3,"3");
		// 			TRACE("���3��%d\n",Count3);
		// 		}
		// 	}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[3], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if (CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[3], iLostFrames, rect.Width() / 4 * 3, 0);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[3], iLostFrames, rect.Width() / 8 * 3, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,3,this,showView,RecvFrame[3],iLostFrames);
		}	}
End:
	;
}
void CFlowNavigatorDlg::StreamCBFunc4(J_tIMAGE_INFO * pAqImageInfo)
{
	int CameraSign = 4;
	if(GetDlgItem(Check_ID[4]) != NULL){
		if((FALSE == isCamBtCheck(Check_ID[4], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[4])++;
	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[CameraSign]->setImageInfo(pAqImageInfo);
		}
		if (Pair[CameraSign] > 0)
		{
			adjustImage_C[CameraSign]->setPairImageInfo(&Pair[CameraSign], pAqImageInfo);
			(Pair[CameraSign])--;
		}
		goto End;
	} 
	else
	{

		if(CtrLine ==42)
		{
			LiveView_Cwj(pAqImageInfo,bmpinfo,0,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*4,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,4,pAqImageInfo,bmpinfo,HALFTONE);
			}	
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[CameraSign], &Count[CameraSign],
			&(Count_Single[CameraSign]),&(Count_Once[CameraSign]), &(Space_Frame[CameraSign]),
			Copy_Space_Frame[CameraSign],Copy_Count_Once[CameraSign],
			&(pImage[CameraSign]), pAqImageInfo, myImageInfo, &(head[CameraSign]));
		// 	if(b_isContinuous)
		// 	{
		// 		if(Count4 > 0){
		// 			g_cs4.Lock();
		// 			if(-1 == InsertHead(pImage[4],pAqImageInfo,myImageInfo,head[4]))
		// 			{
		// 				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 				OnClose();
		// 			}
		// 			Count4--;
		// 			g_cs4.Unlock();
		// 			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo4,Count4,"4");
		// 			TRACE("���4��%d\n",Count4);
		// 		}
		// 	}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[4], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if(CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[4], iLostFrames, 0, rect.Height() / 2);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[5], iLostFrames, rect.Width() / 8 * 5, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,5,this,showView,RecvFrame[5],iLostFrames);
		}	}

End:
	;
}
void CFlowNavigatorDlg::StreamCBFunc5(J_tIMAGE_INFO * pAqImageInfo)
{
	int CameraSign = 5;
	if(GetDlgItem(Check_ID[5]) != NULL){
		if((FALSE == isCamBtCheck(Check_ID[5], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[5])++;
	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[CameraSign]->setImageInfo(pAqImageInfo);
		}
		if (Pair[CameraSign] > 0)
		{
			adjustImage_C[CameraSign]->setPairImageInfo(&Pair[CameraSign], pAqImageInfo);
			(Pair[CameraSign])--;
		}
		goto End;
	} 
	else
	{
		if (CtrLine == 42)
		{	
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*5,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,5,pAqImageInfo,bmpinfo,HALFTONE);
			}	
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[CameraSign], &Count[CameraSign],
			&(Count_Single[CameraSign]),&(Count_Once[CameraSign]), &(Space_Frame[CameraSign]),
			Copy_Space_Frame[CameraSign],Copy_Count_Once[CameraSign],
			&(pImage[CameraSign]), pAqImageInfo, myImageInfo, &(head[CameraSign]));
		// 	if(b_isContinuous)
		// 	{
		// 		if(Count5 > 0){
		// 			g_cs5.Lock();
		// 			if(-1 == InsertHead(pImage[5],pAqImageInfo,myImageInfo,head[5]))
		// 			{
		// 				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 				OnClose();
		// 			}
		// 			Count5--;
		// 			g_cs5.Unlock();
		// 			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo5,Count5,"5");
		// 			TRACE("���5��%d\n",Count5);
		// 		}
		// 	}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[5], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if(CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[5], iLostFrames, rect.Width() / 4, rect.Height() / 2);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[5], iLostFrames, rect.Width() / 8 * 5, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,5,this,showView,RecvFrame[5],iLostFrames);
		}	}
End:
	;
}
void CFlowNavigatorDlg::StreamCBFunc6(J_tIMAGE_INFO * pAqImageInfo)
{
	int CameraSign = 6;
	if(GetDlgItem(Check_ID[6]) != NULL){
		if((FALSE == isCamBtCheck(Check_ID[6], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[6])++;
	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[CameraSign]->setImageInfo(pAqImageInfo);
		}
		if (Pair[CameraSign] > 0)
		{
			adjustImage_C[CameraSign]->setPairImageInfo(&Pair[CameraSign], pAqImageInfo);
			(Pair[CameraSign])--;
		}
		goto End;
	} 
	else
	{

		if (CtrLine == 42)
		{
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/2,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81) 
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*6,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,6,pAqImageInfo,bmpinfo,HALFTONE);
			}	
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[CameraSign], &Count[CameraSign],
			&(Count_Single[CameraSign]),&(Count_Once[CameraSign]), &(Space_Frame[CameraSign]),
			Copy_Space_Frame[CameraSign],Copy_Count_Once[CameraSign],
			&(pImage[CameraSign]), pAqImageInfo, myImageInfo, &(head[CameraSign]));
		// 	if(b_isContinuous)
		// 	{
		// 		if(Count6 > 0){
		// 			g_cs6.Lock();
		// 			if(-1 == InsertHead(pImage[6],pAqImageInfo,myImageInfo,head[6]))
		// 			{
		// 				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 				OnClose();
		// 			}
		// 			Count6--;
		// 			g_cs6.Unlock();
		// 			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo6,Count6,"6");
		// 			TRACE("���6��%d\n",Count6);
		// 		}
		// 	}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	J_Image_GetStreamInfo(m_hThread[6], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if(CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[6], iLostFrames, rect.Width() / 2, rect.Height() / 2);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[6], iLostFrames, rect.Width() / 8 * 6, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,6,this,showView,RecvFrame[6],iLostFrames);
		}	
	}
End:
	;
}
void CFlowNavigatorDlg::StreamCBFunc7(J_tIMAGE_INFO * pAqImageInfo)
{
	int CameraSign = 7;
	if(GetDlgItem(Check_ID[7]) != NULL){
		if((FALSE == isCamBtCheck(Check_ID[7], this)) || CtrLine == 0)
		{
			goto End;
		}
	}
	if(CtrLine == 0)
	{
		goto End;
	}
	(RecvFrame[7])++;
	CRect rect;
	GetClientRect(&rect);
	int showHeight = rect.Width() / 8 *2048/2560;
	int showStartHeight = rect.Height() / 2 - showHeight / 2;

	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[CameraSign]->setImageInfo(pAqImageInfo);
		}
		if (Pair[CameraSign] > 0)
		{
			adjustImage_C[CameraSign]->setPairImageInfo(&Pair[CameraSign], pAqImageInfo);
			(Pair[CameraSign])--;
		}
		goto End;
	} 
	else
	{

		if (CtrLine ==42)
		{	
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4*3,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
		else if (CtrLine == 81)
		{
			if(DoubleClk == DOUBLECLK_IN){ 
				LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*7,showStartHeight,rect.Width()/8,showHeight,HALFTONE);
			}else if(DoubleClk == DOUBLECLK_OUT){
				LiveViewWnd(this,checkShow,7,pAqImageInfo,bmpinfo,HALFTONE);
			}	
		}
		insertBySign(b_isContinuous, b_isSingleFrame, &g_cs[CameraSign], &Count[CameraSign],
			&(Count_Single[CameraSign]),&(Count_Once[CameraSign]), &(Space_Frame[CameraSign]),
			Copy_Space_Frame[CameraSign],Copy_Count_Once[CameraSign],
			&(pImage[CameraSign]), pAqImageInfo, myImageInfo, &(head[CameraSign]));
		// 	if(b_isContinuous)
		// 	{
		// 		if(Count7 > 0){
		// 			g_cs7.Lock();
		// 			if(-1 == InsertHead(pImage[7],pAqImageInfo,myImageInfo,head[7]))
		// 			{
		// 				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
		// 				OnClose();
		// 			}
		// 			Count7--;
		// 			g_cs7.Unlock();
		// 			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo7,Count7,"7");
		// 			TRACE("���7��%d\n",Count7);
		// 		}
		// 	}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	J_Image_GetStreamInfo(m_hThread[7], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	if(CtrLine == 42)
	{
		DrawTextByCamSign(this, showView, RecvFrame[7], iLostFrames, rect.Width() / 4 * 3, rect.Height() / 2);
	}
	else if(CtrLine == 81)
	{
		if(DoubleClk == DOUBLECLK_IN){
			DrawTextByCamSign(this, showView, RecvFrame[7], iLostFrames, rect.Width() / 8 * 7, showStartHeight);
		}else if(DoubleClk == DOUBLECLK_OUT){
			DrawTextLocate(checkShow,7,this,showView,RecvFrame[7],iLostFrames);
		}		}
End:
	;
}

void CFlowNavigatorDlg::LiveViewWnd(CWnd* pWnd, CheckToShow* checkShow, int sign,
									J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,int nStretchMode)
{
	CRect rect;
	pWnd->GetClientRect(&rect);

	int Weight      = checkShow->ReturnWeight(sign);
	int startHeight = checkShow->getStartHeight(rect);
	int SrcWidth    = checkShow->getWidth(rect);
	int SrcHeight   = checkShow->getHeight(rect, 2048 , 2560); // ���ﲻ��2560������Ĭ�ϲ���Ī������ĳ���2056_Bug_20160517

	if(sign == CamSign)
	{
		LiveView_Cwj(pAqImageInfo, bmpinfo, Weight*SrcWidth, startHeight, SrcWidth, SrcHeight, checkShow,
			HALFTONE);
	}
	else
	{
		LiveView_Cwj(pAqImageInfo, bmpinfo, Weight*SrcWidth, startHeight, SrcWidth, SrcHeight);
	}
}

void CFlowNavigatorDlg::LiveView_Cwj(J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,
									 int x,int y,int scaleX,int scaleY,
									 CheckToShow* checkShow,
									 int nStretchMode)
{
	CDC * pDC = GetDC();
	CDC MemDC;

	if(!MemDC.CreateCompatibleDC(pDC))
	{
		ReleaseDC(pDC);
		return;
	}
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight);
	CBitmap* pOldBmp = MemDC.SelectObject(&bmp);
	::SetDIBitsToDevice(MemDC.GetSafeHdc(),
		0,0,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight,
		0,0,
		0,(bmpinfo->bmiHeader.biHeight),
		(pAqImageInfo->pImageBuffer),
		bmpinfo,
		DIB_RGB_COLORS);

	pDC->SetStretchBltMode(nStretchMode);//COLORONCOLOR   HALFTONE

	pDC->StretchBlt(x,y,scaleX,scaleY,&MemDC,checkShow->showStartAndSize[0],checkShow->showStartAndSize[1],/*first*/
		checkShow->showStartAndSize[2],-checkShow->showStartAndSize[3],/*second */
		SRCCOPY);
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
	bmp.DeleteObject();
	ReleaseDC(pDC);
}

void CFlowNavigatorDlg::LiveView_Cwj(J_tIMAGE_INFO * pAqImageInfo,BITMAPINFO *bmpinfo,
									 int x,int y,int scaleX,int scaleY,int nStretchMode)
{
	CDC * pDC = GetDC();
	CDC MemDC;

	if(!MemDC.CreateCompatibleDC(pDC))
	{
		ReleaseDC(pDC);
		return;
	}
	//float begin = clock();
	/*֮��������ע�����Ҳ�����ǿ���һ���߳�������ת ������תpAaImageInfo��ֵ���и�ֵ��
	pAaImageInfo�������ֵӦ�������߳���ɣ������£�
	�����߳���Ҫ���ٵ������������ʹ��pAImageInfo����*/
	//J_Image_Flip(pAqImageInfo,J_FLIP_VERTICAL);//SetDIBitsToDevice�����⣬��������Ҳ�ܸ�����·�תЧ����//��ʵ��
	//float end = clock();
	//TRACE("flip time: %f\n",end-begin);   //5��7����֮��

	/*
	int SetDIBitsToDevice(HDC hdc,
		int xDest, int Ydest,
		DWORD dwWidth, DWORD dwHeight,
		intXSrc, int Ysrc,
		UINT uStartScan, UINT cScanLines,
		CONST VOID *lpvBits, CONST BITMAPINFO *lpbmi, UINT fuColorUse);
	hdc���豸���������
	XDest��ָ��Ŀ��������Ͻǵ�X�����꣬���߼���λ��ʾ���ꡣ
	YDest��ָ��Ŀ��������Ͻǵ�Y�����꣬���߼���λ��ʾ���ꡣ
	dwWidth��ָ��DIB�Ŀ�ȣ����߼���λ��ʾ��ȡ�
	dwHeight��ָ��DIB�ĸ߶ȣ����߼���λ��ʾ�߶ȡ�
	XSrc��ָ��DIBλͼ���½ǵ�X�����꣬���߼���λ��ʾ���ꡣ
	YSrc��ָ��DIBλͼ���½ǵ�Y�����꣬���߼���λ��ʾ���ꡣ
	uStartScan��ָ��DIB�е���ʼɨ���ߡ�
	cScanLInes��ָ������lpvBitsָ��������а�����DIBɨ������Ŀ��
	lpvBits��ָ��洢DIB��ɫ���ݵ��ֽ����������ָ�롣���ڸ������Ϣ����ο�����ı�עһ�ڡ�
	lpbmi��ָ��BITMAPINFO�ṹ��ָ�룬�ýṹ�����й�DIB����Ϣ��
	fuColorUse��ָ��BITMAPINFO�ṹ�еĳ�ԱbmiColors�Ƿ������ȷ��RGBֵ��Ե�ɫ�����������ֵ���йظ������Ϣ����ο�����ı�ע���֡�
	����fuColorUse����������ֵ֮һ����Щֵ�ĺ������£�
	DIB_PAL_COLORS����ʾ��ɫ����16λ������ֵ������ɣ�������Щֵ�ɶԵ�ǰѡ�е��߼���ɫ�����������
	DIB_RGB_COLORS����ʾ��ɫ�����ԭ���RGBֵ��
	*/
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight);
	CBitmap* pOldBmp = MemDC.SelectObject(&bmp);
	::SetDIBitsToDevice(MemDC.GetSafeHdc(),
		0,0,bmpinfo->bmiHeader.biWidth,bmpinfo->bmiHeader.biHeight,
		0,0,
		0,(bmpinfo->bmiHeader.biHeight),
		(pAqImageInfo->pImageBuffer),
		bmpinfo,
		DIB_RGB_COLORS);
	//pDC->BitBlt(1,1,2560,2048,&MemDC,0,0,SRCCOPY);
/*BOOL StretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeighDest,
                           HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, 
						   int nWidthSrc, int nHeightSrc, 
						   DWORD dwRop)
 
����hdcDest��ָ��Ŀ���豸�����ľ���� 
����nXDest�� ָ��Ŀ������������Ͻǵ�X���߼����ꡣ 
����nYDest�� ָ��Ŀ������������Ͻǵ�Y���߼����ꡣ 
����nWidth�� ָ��Դ��Ŀ�����������߼���ȡ� 
����nHeight��ָ��Դ��Ŀ�����������߼��߶ȡ� 
����hdcSrc�� ָ��Դ�豸�����ľ���� 
����nXSrc��  ָ��Դ�����������Ͻǵ�X���߼����ꡣ 
����nYSrc��  ָ��Դ�����������Ͻǵ�Y���߼����ꡣ 
����nWidthSrc�� ָ��Դ���εĿ�ȣ����߼���λ��ʾ��ȡ�  
����nHeightSrc��ָ��Դ���εĸ߶ȣ����߼���λ��ʾ�߶ȡ�
  */

	pDC->SetStretchBltMode(COLORONCOLOR);//COLORONCOLOR   HALFTONE
	pDC->StretchBlt(x,y,scaleX,scaleY,&MemDC,0,bmpinfo->bmiHeader.biHeight,/*first*/
		bmpinfo->bmiHeader.biWidth,-(bmpinfo->bmiHeader.biHeight),/*second */
		SRCCOPY);
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
	bmp.DeleteObject();
	ReleaseDC(pDC);
}
void CFlowNavigatorDlg::InitializeControls()
{
	BOOL bt_on = FALSE;
}

void CFlowNavigatorDlg::EndControl(void)
{
	b_isContinuous  = FALSE;
	b_isSingleFrame = FALSE;
	mAdjust = FALSE;
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		if (GetDlgItem(Check_ID[i]) != NULL)
		{
			checkBt[i]->DestroyWindow(); // �ֶ������Ŀؼ���ɾ����CloseCamera ������
		}
	}

	DoubleClk = DOUBLECLK_IN;
	CloseSystem = TRUE;
}

void CFlowNavigatorDlg::EnableControls()
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		RecvFrame[i] = 0;
	}
	CtrLine = 0;
	DoubleClk =DOUBLECLK_IN;
	CloseSystem = TRUE;

}

void CFlowNavigatorDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
{
	CString errorMsg;
	errorMsg.Format(_T("%s: Error = %d: "), message, error);

	switch(error)
	{
	case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	                break;
	case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		                break;
	case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			                break;
	case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		                break;
	case J_ST_NO_DATA:				errorMsg += "No data ";				                break;
	case J_ST_ERROR:				errorMsg += "Generic error ";		                break;
	case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	                break;
	case J_ST_TIMEOUT:				errorMsg += "Timeout ";				                break;
	case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	                break;
	case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		                break;
	case J_ST_FILE_IO:				errorMsg += "File IO error ";		                break;
	case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		                break;
	case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
	case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
	}

	AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}
/*
UINT CreateWaitDlg(LPVOID pParam)
{
	st_Wait* st_wait = (st_Wait*)pParam;
	CString paneText;
	int I = 0;
	HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();

	WPARAM wParam;
	LPARAM lParam;
	int system_state = -1;
	int content      = -1;
	lParam           = (LPARAM)content;

	while (st_wait->CameraCount == 0)
	{
		system_state = SYSTEM_WILL_OPEN;
		wParam       = (WPARAM)system_state;
		::SendMessage(hwnd, WM_MY_MESSAGE_INIT, wParam, lParam);

		Sleep(10);
	}
	while(openCamCount < st_wait->CameraCount)
	{
		system_state = SYSTEM_OPEN_ING;
		wParam       = (WPARAM)system_state;
		::SendMessage(hwnd, WM_MY_MESSAGE_INIT, wParam, lParam);
	
		Sleep(50);
	}

	if(openCamCount == st_wait->CameraCount)
	{
		system_state = SYSTEM_IS_OPEN;
		wParam       = (WPARAM)system_state;

		::SendMessage(hwnd, WM_MY_MESSAGE_INIT, wParam, lParam);
	}

	return 0;
}
*/

UINT CreateWaitDlg(LPVOID pParam)
{
	st_Wait* st_wait = (st_Wait*)pParam;
	CString paneText;
	int I = 0;
	HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();


	if (st_wait->CameraCount == 0)
	{
		st_wait->statusBar->SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬: ���ڴ�"), TRUE);
	}
	while(openCamCount <= st_wait->CameraCount)
	{
		if((openCamCount == st_wait->CameraCount) && (st_wait->CameraCount != 0))
		{
			st_wait->statusBar->SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬: �Ѿ���"), TRUE);
			break;
		}
		if(st_wait->CameraCount == 0)
		{
			I = 0;
		}
		else
		{
			I = openCamCount * 100 / (st_wait->CameraCount);
		}

		paneText.Format(_T("ϵͳ״̬:�����%d%%"), I);
		st_wait->statusBar->SetPaneText(INDEX_INDICATOR_INIT, paneText, TRUE);

		Sleep(100);
	}


	return 0;
}


void CFlowNavigatorDlg::OnOpenCamera()
{
	// TODO: �ڴ���������������
	TRACE(_T("OpenCamera\n"));

	//st_wait->statusBar   = &m_statusBar;
	//::AfxBeginThread(CreateWaitDlg,st_wait,THREAD_BASE_PRIORITY_MAX);

//	dlgWait = new CWaitLoading;
// 	dlgWait->Create(IDD_DIALOG2,AfxGetMainWnd());
// 	dlgWait->ShowWindow(SW_SHOW);
// 	dlgWait->UpdateWindow();
// 	dlgWait->StartTimes();

	m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬: ���ڴ�"), TRUE);

	OpenFactoryAndCamera();

	m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬: �Ѿ���"), TRUE);

	bt_on = TRUE;
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_OpenCamera,bt_on);
	//dlgWait->KillTimes();
	CloseSystem = FALSE;
}

void CFlowNavigatorDlg::OnCloseCamera()
{
	// TODO: �ڴ���������������
	EnableControls();
	OnCloseStream();
	if(GetDlgItem(Check_ID[0]) != NULL)
	{
		DestroyCheckCamBt(checkBt, Check_ID);
	}
	CloseFactoryAndCamera();
	m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬��     δ��"));
	Invalidate(TRUE);

	bt_on = FALSE;
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_OpenCamera,bt_on);
	CtrLine = 0;
	openCamCount = 0;
	st_wait->CameraCount = 0;

	WPARAM wParam;
	LPARAM lParam;
	int system_state = -1;
	int content      = -1;
	lParam           = (LPARAM)content;

	for (int i = SYSTEM_NOT_OPEN; i < CAM_IS_CLOSED+1; i++)
	{
		system_state = i;
		wParam       = (WPARAM)system_state;
		SendMessage(WM_MY_MESSAGE_INIT, wParam, lParam);
	}
}

void CFlowNavigatorDlg::OnQuit()
{
	// TODO: �ڴ���������������
	EndControl();
	EnableControls();

	OnOK();
	CloseFactoryAndCamera();
	GdiplusShutdown(gdiplusToken);

}

void CFlowNavigatorDlg::OnDestroy()
{

	// Stop acquisition
	// Close factory & camera
	CloseFactoryAndCamera();
	if (setDlg != NULL)
	{	
		delete setDlg;
		setDlg = NULL;
	}
	GdiplusShutdown(gdiplusToken);

	CDialog::OnDestroy();
}

void CFlowNavigatorDlg::OnSetValues()
{
	// TODO: �ڴ���������������
	TRACE("SetValues\n");
	int retval;
	int64_t int64Val;
	CSliderCtrl *pSCtrl;
	//SetTimer(1,1000,NULL);
	if(setDlg == NULL )
	{
		setDlg = new CSetValue(NULL, m_CurrentProPath);
	}
	setDlg->m_CameraCount = m_CameraCount;

	setDlg->m_CurrentProPath = m_CurrentProPath;//����ѡ����Ŀ

	for (int i = 0; i<m_CameraCount; i++)
	{
		setDlg->m_hCam[i]= m_hCam[i];
	}

	setDlg->Create(IDD_DIALOG1,this);
	setDlg->ShowWindow(SW_SHOW);
	//setDlg->Start();
	setDlg->SetRangeMM();//˳�����Ҫ���봰�崴���ˣ�����ʹ�ã�����Assert����

	setDlg->UpdateWindow();//�������ᵼ�� ������ģ̬�Ի���ʱ����߿ؼ�һ��ʼ����ʾ���϶�һ�¶Ի������ʾ��


/*
	retval = J_Camera_GetNodeByName(m_hCam[0], NODE_NAME_EXPOSURE, &m_hExposureNode);

	retval = J_Node_GetValueInt64(m_hExposureNode, TRUE, &int64Val);
    retval = J_Camera_GetNodeByName(m_hCam[1], NODE_NAME_EXPOSURE, &m_hExposureNode);

	//retval = J_Node_SetValueInt64(m_hExposureNode, TRUE, (int64_t)100);
	retval = J_Node_GetValueInt64(m_hExposureNode, TRUE, &int64Val);
	//retval = J_Node_SetValueInt64(m_hExposureNode, TRUE, (int64_t)29000);
*/
}

void CFlowNavigatorDlg::On4Col2Line()
{
	// TODO: �ڴ���������������
	CtrLine = 42;
	if (GetDlgItem(Check_ID[0]) != NULL)
	{
		DestroyCheckCamBt(checkBt, Check_ID);
	}
	Invalidate(TRUE);
}

void CFlowNavigatorDlg::On8col1line()
{
	// TODO: �ڴ���������������
	CRect rect;
	GetClientRect(&rect);

	if (GetDlgItem(Check_ID[0]) == NULL)
	{
		CreateCheckCamBt(checkBt, rect, this, Check_ID);
		EnableCheckCamBt(checkBt, MAX_CAMERAS, m_CameraCount, FALSE);
	}
	else
	{
		DestroyCheckCamBt(checkBt, Check_ID);
		CreateCheckCamBt(checkBt, rect, this, Check_ID);
		EnableCheckCamBt(checkBt, MAX_CAMERAS, m_CameraCount, FALSE);
	}
	
	int showWidth = rect.Width() / 8;
	int showHeight = showWidth * 2048 / 2560; // ��ʾ�߶�
	int showStartHeight = rect.Height() / 2 - showHeight / 2;
	CtrLine = 81;
	Invalidate(TRUE);
	DrawLine(this, rect.left, showStartHeight, rect.right, showStartHeight);
	DrawLine(this, rect.left, showStartHeight + showHeight, rect.right, showStartHeight + showHeight);
}

/*
int CFlowNavigatorDlg::SaveAndDelTail(pImageNode pHead,
									J_tIMAGE_INFO& m_CnvImageInfo,
									 const char* numCam,
									 const char* path, const char* fileName)
{
	g_cs.Lock();
	pImageNode pTail = ImageInfo::getTailNode(pHead);
	g_cs.Unlock();

	if (pTail == NULL)
	{
		Sleep(10);
	}
	else
	{
		(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

		if(Count > 0){
			CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo,Count,"0");

			g_cs.Lock();
			ImageInfo::DelTail(pHead);
			g_cs.Unlock();

 			Count--;
		}
	}
	return 0;
}
*/
UINT Write2File(LPVOID param)
{
	TRACE("Write2File!\n");

	while(b_isContinuous || b_isSingleFrame)
	{
		float begin = clock();

		pImageNode pHead = (pImageNode)param;
		(g_cs[0]).Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		(g_cs[0]).Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[0]), &(Total_Frame[0]),
				"0", "0_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[0]))
			{
				(g_cs[0]).Lock();
				ImageInfo::DelTail(pHead);
				(g_cs[0]).Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}
// 			if (b_isContinuous)
// 			{
// 				if (Count_[0] == 0)
// 				{
// 					AfxMessageBox(_T("����ģʽ�ɼ����"), MB_ICONINFORMATION);
// 					goto End;
// 				}
// 				SaveImage(m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[0], &(Count_[0]),"0");
// 			}
// 			else if (b_isSingleFrame)
// 			{
// 				SaveImage(m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[0], &(Total_Frame[0]), "0_");
// 			}

// 			if(Count_[0] > 0)
// 			{
// 				char path[MAX_PATH]={0};
// 				if(m_CurrentProPath == DEFAULT_PATH)
// 				{
// 					CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo,Count_[0],"0");
// 				}
// 				else
// 				{
// 					CString tempPath = m_CurrentProPath + _T("\\ԭʼ����");
// 					int nLength = WideCharToMultiByte(CP_ACP, 0, tempPath, tempPath.GetLength(),
// 						NULL, 0, NULL, NULL);
// 					WideCharToMultiByte(CP_ACP, 0, tempPath, tempPath.GetLength(), path, nLength, NULL, NULL);
// 					//wcstombs(path, m_CurrentProPath, m_CurrentProPath.GetLength());//���ֱ��������漸�仰
// 				}
// 
// 				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo,Count_[0],"0", path);

				(g_cs[0]).Lock();
				ImageInfo::DelTail(pHead);
				(g_cs[0]).Unlock();
			//}
		}

		float end = clock();
		TRACE("Write2File time %d %d: %lf\n",Count_[0],Total_Frame[0],end-begin);
	}
End:
	return 0;
}

UINT Write2File1(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs1.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs1.Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[1]), &(Total_Frame[1]),
				"1", "1_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[1]))
			{
				g_cs[1].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[1].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[1].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[1].Unlock();
			
		}
	}
End:
    return 0;
} 
UINT Write2File2(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs[2].Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs[2].Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[2]), &(Total_Frame[2]),
				"2", "2_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[2]))
			{
				g_cs[2].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[2].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[2].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[2].Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File3(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs[3].Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs[3].Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[3]), &(Total_Frame[3]),
				"3", "3_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[3]))
			{
				g_cs[3].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[3].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[3].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[3].Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File4(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs[4].Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs[4].Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[4]), &(Total_Frame[4]),
				"4", "4_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[4]))
			{
				g_cs[4].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[4].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[4].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[4].Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File5(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs[5].Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs[5].Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[5]), &(Total_Frame[5]),
				"5", "5_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[5]))
			{
				g_cs[5].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[5].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[5].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[5].Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File6(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs[6].Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs[6].Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[6]), &(Total_Frame[6]),
				"6", "6_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[6]))
			{
				g_cs[6].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[6].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[6].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[6].Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File7(LPVOID param)
{
	while(b_isSingleFrame || b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs[7].Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs[7].Unlock();

		if (pTail == NULL)
		{
			Sleep(80);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[7]), &(Total_Frame[7]),
				"7", "7_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[7]))
			{
				g_cs[7].Lock();
				ImageInfo::DelTail(pHead);
				g_cs[7].Unlock();

				//b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs[7].Lock();
			ImageInfo::DelTail(pHead);
			g_cs[7].Unlock();
		}
	}
End:
    return 0;
} 
void CFlowNavigatorDlg::OnContinuous()
{
	// TODO: �ڴ���������������
	//���ǵ��� �� ���δ��״̬���Լ�����һ�ε��ǹر�״̬�£��ܾ�����ɼ�ģʽ
	if(m_CameraCount <= 0)
	{
		AfxMessageBox(_T("������!"),MB_ICONINFORMATION);
		return;
	}
	if(m_hCam[0] == NULL)
	{
		AfxMessageBox(_T("������!"),MB_ICONINFORMATION);
		return;
	}

	b_isSingleFrame = FALSE;
	CSetSampleDlg *samDlg;
	samDlg = new CSetSampleDlg;
	samDlg->m_CameraCount = m_CameraCount;
	for (int i = 0; i < m_CameraCount; i++)
	{
		samDlg->m_hCam[i] = m_hCam[i];
	}
	samDlg->DoModal();
	b_isContinuous = TRUE;
// 	Count1=Count2=Count3=Count4=Count5=Count6=Count7=samDlg->Count;

	initCount = samDlg->Count;
	//��ȡ��ǰѡ�е�������λ�õ�
	checkShow->setCheckCamSign(checkBt, MAX_CAMERAS);
	checkShow->getCheckNum(checkBt, MAX_CAMERAS, TRUE);

	int* checkCamSign = checkShow->getCheckCamSign();
	int num = checkShow->getNum();
	for (int i = 0; i < num; i++)
	{
		Count[checkCamSign[i]]  = samDlg->Count;
		Count_[checkCamSign[i]] = samDlg->Count;
	}
		
	hThread[0] = ::AfxBeginThread(Write2File ,head[0],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[1] = ::AfxBeginThread(Write2File1,head[1],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[2] = ::AfxBeginThread(Write2File2,head[2],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[3] = ::AfxBeginThread(Write2File3,head[3],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[4] = ::AfxBeginThread(Write2File4,head[4],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[5] = ::AfxBeginThread(Write2File5,head[5],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[6] = ::AfxBeginThread(Write2File6,head[6],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[7] = ::AfxBeginThread(Write2File7,head[7],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);

	SetTimer(SETTIMER_ID_COLLECTION, 1000, NULL);
}

void CFlowNavigatorDlg::OnSingleFrame()
{
	// TODO: �ڴ���������������
	b_isContinuous = FALSE;
	SetSingleFrameValueDlg* setSingleDlg = new SetSingleFrameValueDlg;
	if(IDCANCEL == setSingleDlg->DoModal())
	{
		return;
	}

	NODE_HANDLE hNode;
	double d_val;
	if(J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[0],(int8_t *)"AcquisitionFrameRate",&hNode))
	{
		if(J_ST_SUCCESS != J_Node_GetValueDouble(hNode,TRUE,&d_val))	
		{
			AfxMessageBox(_T("δ����ȷ��ȡ���֡�ʲ���!"),MB_ICONINFORMATION);
		}
	}
	else
	{
		AfxMessageBox(_T("δ����ȷ��ȡ���֡�ʲ���!"),MB_ICONINFORMATION);
	}

	Space_Frame[0] = Count_Space[0] * (int)d_val;

	//��ȡ��ǰѡ�е�������λ�õ�
	checkShow->setCheckCamSign(checkBt, MAX_CAMERAS);
	checkShow->getCheckNum(checkBt, MAX_CAMERAS, TRUE);

	int* checkCamSign = checkShow->getCheckCamSign();
	int num = checkShow->getNum();

	for (int i = 0; i < num; i++)
	{
		Space_Frame[checkCamSign[i]]      = Space_Frame[0];
		Copy_Space_Frame[checkCamSign[i]] = Space_Frame[0];
		Copy_Count_Once[checkCamSign[i]]  = Count_Once[0];
		Total_Frame[checkCamSign[i]]      = Count_Once[0] * Count_Single[0];
	}
	initCount = Count_Once[0] * Count_Single[0];
	b_isSingleFrame = TRUE;
	hThread[0] = ::AfxBeginThread(Write2File ,head[0],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[1] = ::AfxBeginThread(Write2File1,head[1],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[2] = ::AfxBeginThread(Write2File2,head[2],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[3] = ::AfxBeginThread(Write2File3,head[3],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[4] = ::AfxBeginThread(Write2File4,head[4],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[5] = ::AfxBeginThread(Write2File5,head[5],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[6] = ::AfxBeginThread(Write2File6,head[6],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[7] = ::AfxBeginThread(Write2File7,head[7],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);

	SetTimer(SETTIMER_ID_COLLECTION, 1000, NULL);
}

void CFlowNavigatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CTime   time;
	CString str_time;
	int I        = 0;     //�ɼ�����
	int maxCount = 0;
	CString text;

	int collect_state = 0;
	switch(nIDEvent)
	{
		case SETTIMER_ID_SYSTEM_TIME:
			time     = CTime::GetCurrentTime();
			str_time = time.Format(_T("%H:%M:%S"));

			m_statusBar.SetPaneText(INDEX_INDICATOR_TIME, str_time, TRUE);

			break;
		case SETTIMER_ID_COLLECTION:
			//δѡ���������ֹ��ʱ���Լ��������߳�
			if((checkShow->getNum() == 0) || (initCount == 0))
			{
				KillTimer(SETTIMER_ID_COLLECTION);
				b_isContinuous  = FALSE;
				b_isSingleFrame = FALSE;

				break;
			}

			if(b_isContinuous)
			{
				maxCount = FindMax_Int(Count_, MAX_CAMERAS);
				I        = (initCount - maxCount) * 100 / initCount;
				text.Format(_T("�ɼ�״̬: �Ѳɼ�%d%%"), I);
				m_statusBar.SetPaneText(INDEX_INDICATOR_SAVE_IMG, text, TRUE);
				
				for (int i = 0; i < m_CameraCount; i++)
				{
					if(Count_[i] == 0)
					{
						collect_state++;
					}
				}
			}
			else if(b_isSingleFrame)
			{
				maxCount = FindMax_Int(Total_Frame, MAX_CAMERAS);
				I        = (initCount - maxCount) * 100 / initCount;
				text.Format(_T("�ɼ�״̬: �Ѳɼ�%d%%"), I);
				m_statusBar.SetPaneText(INDEX_INDICATOR_SAVE_IMG, text, TRUE);
			
				for (int i = 0; i < MAX_CAMERAS; i++)
				{
					if(Total_Frame[i] == 0)
					{
						collect_state++;
					}
				}
			}
			if(collect_state == m_CameraCount)
			{
				KillTimer(SETTIMER_ID_COLLECTION);
				b_isContinuous  = FALSE;
				b_isSingleFrame = FALSE;

				for (int j = 0; j < MAX_CAMERAS; j++)
				{
					Count_[j] = 0;
					Count[j]  = 0;
				}
			}
			break;

		default:
			;
	}
	CDialog::OnTimer(nIDEvent);
}

void CFlowNavigatorDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//b_isContinuous = FALSE;//���� �洢�̲߳��������˳������е��ٽ������ֱ����ж�
	EndControl();
	CloseFactoryAndCamera();
	EnableControls();
	GdiplusShutdown(gdiplusToken);



	if (setDlg != NULL)
	{	
		delete setDlg;
	}

	CDialog::OnClose();
}

void CFlowNavigatorDlg::OnLinkFlow()
{
	// TODO: �ڴ���������������

// 	//��ȡѡ��button�����Լ���ʼλ��
// 	checkShow->getCheckNum(checkBt, MAX_CAMERAS, TRUE);
// 
// 	checkShow->ChangeButtonState(checkBt, MAX_CAMERAS, FALSE);
	if(!mAdjust) 
		return;
	setPivArg = SetPivArg::GetInstance();
	setPivArg->setShowView(SV_AdjustArg);
	setPivArg->copy_solveFlow = &solveFlow;
	setPivArg->Create(IDD_DIALOG_PIVSET, this);
	setPivArg->ShowWindow(SW_SHOW);
	setPivArg->UpdateWindow();
}


void CFlowNavigatorDlg::OnAdjust()
{
	// TODO: �ڴ���������������

	ShowCheckCamBt(this, Check_ID, FALSE);

	char currentPath[MAX_PATH] = {0};
	wcstombs(currentPath, m_CurrentProPath, m_CurrentProPath.GetLength());

	FILE* fp;
	char path[MAX_PATH]     = {0};	
	float m_xMin_s;
	float m_xMax_s;
	float m_yMin_s;
	float m_yMax_s;
	float Lx;
	float Ly;
	// ��ָ���
	float m_leftSplit;
	// �ҷָ���
	float m_RightSplit;

	for (int i = 0; i < m_CameraCount; i++)
	{
		sprintf(path, "%s\\�����ļ�\\Position_Cam%d.txt", currentPath, i);
		fp = fopen(path, "r");

		fscanf(fp, "%f %f %f %f %f %f %f %f ", &m_xMin_s, &m_xMax_s, &m_yMin_s,
			&m_yMax_s, &Lx, &Ly, &m_leftSplit, &m_RightSplit);

		adjustImage_C[i]->setSingleRange(m_xMin_s, m_xMax_s, m_yMin_s, m_yMax_s);
		adjustImage_C[i]->setL(Lx, Ly);
		adjustImage_C[i]->setLeftSplitLine(m_leftSplit);
		adjustImage_C[i]->setSplitLine(m_RightSplit);
		adjustImage_C[i]->setAdjustImageSize(); // ���Ҫ��setSingleRange()֮�����
		imgClb->UpdateRelatedArraySize(i);

		fclose(fp);

		imgClb->Load_RelatedArray_ToMemory(currentPath, i);

		adjustImage_C[i]->setRelatedArray_k(imgClb->getRelatedArray(i));
	}

	mAdjust = true;
	
	Invalidate(TRUE);
	for (int j = 0; j < MAX_CAMERAS; j++)
	{
		m_pAdjustCls->adjustImage_C[j] = adjustImage_C[j];
	}
	SV_AdjustArg = new ShowView(this, hinst);
	m_pAdjustCls->showView = SV_AdjustArg;//
	m_pAdjustCls->pWnd = this;

	m_pAdjustCls->linkImage = new LinkImage();
	m_pAdjustCls->pTwoImageInfo = (st_IMAGE_INFO *)malloc(sizeof(st_IMAGE_INFO));
	m_pAdjustCls->pTwoImageInfo->pOutImage = NULL;

	m_pAdjustCls->checkShow = checkShow;
	checkShow->setCheckCamSign(checkBt, MAX_CAMERAS);
	checkShow->getCheckNum(checkBt, MAX_CAMERAS, TRUE);
	m_pAdjustCls->Count = checkShow->getNum();

	checkShow->ChangeButtonState(checkBt, MAX_CAMERAS, FALSE);

	::AfxBeginThread(AdjustThreadFunc::AdjustIm,m_pAdjustCls,THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);

	//adjust�߳����ڼ��㣬���߼���δ���ʱ��ֹͣadjust�̣߳������ٴδ��߳�BUG_2016_05_10
	for (unsigned int j = 0; j < m_pAdjustCls->Count; j++)
	{
		Pair[j] = 2;
	}
}

void CFlowNavigatorDlg::OnLink()
{
	// TODO: �ڴ���������������
	//mAdjust = FALSE;
	mLink = TRUE;
}

void CFlowNavigatorDlg::OnTest()
{
	// TODO: �ڴ���������������
	ShowCheckCamBt(this, Check_ID, TRUE);
	checkShow->ChangeButtonState(checkBt, MAX_CAMERAS, TRUE);
	mLink           = FALSE;
	mAdjust         = FALSE;
	b_isContinuous  = FALSE;
	b_isSingleFrame = FALSE;

	solveFlow       = -1;    // ����ֹͣ��������ʱ�����moveʱ���ڻ�ȡ����ֵ
	//Invalidate(TRUE);
}
void CFlowNavigatorDlg::OnOpenproj()
{
	// TODO: �ڴ���������������
	CString dstDir = DEFAULT_PATH;

	CString SubDir[MAX_SUBDIR] = {_T("\\ԭʼ����"), _T("\\ͼ��ƴ��"), _T("\\����ƴ��"), _T("\\�����ļ�")};

	if(IDOK == SelectDir(dstDir,GetSafeHwnd(),FALSE))
	{
		if (dstDir == DEFAULT_PATH)
		{
			dstDir += _T("\\HawkSoftWorkSpace");
			CreatePathDir(dstDir);
			CTime time = CTime::GetCurrentTime();
			CString Time = time.Format(_T("%Y%m%d%H%M%S"));
			dstDir += _T("\\FlowNavigator") + Time;
			CreatePathDir(dstDir);
		}
		TRACE(_T("project path: %s\n"), dstDir);
		InitPathDir(dstDir, SubDir, MAX_SUBDIR);

		SetWindowText(dstDir);
		m_CurrentProPath = dstDir;
	}
}

void CFlowNavigatorDlg::OnCreateproj()
{
	// TODO: �ڴ���������������
	CString dstDir = DEFAULT_PATH;

	CString SubDir[MAX_SUBDIR] = {_T("\\ԭʼ����"), _T("\\ͼ��ƴ��"), _T("\\����ƴ��"), _T("\\�����ļ�")};

	if(IDOK == SelectDir(dstDir,GetSafeHwnd(), TRUE))
	{
		if (dstDir == DEFAULT_PATH)
		{
			dstDir += _T("\\HawkSoftWorkSpace");
			CreatePathDir(dstDir);
			CTime time = CTime::GetCurrentTime();
			CString Time = time.Format(_T("%Y%m%d%H%M%S"));
			dstDir += _T("\\FlowNavigator") + Time;
			CreatePathDir(dstDir);
		}
		TRACE(_T("project path: %s\n"), dstDir);
		InitPathDir(dstDir, SubDir, MAX_SUBDIR);

		SetWindowText(dstDir);
		m_CurrentProPath = dstDir;
	}
}

void CFlowNavigatorDlg::InitPathDir(CString dstDir, CString SubDir[], UINT_K length)
{
	//UINT_K length = sizeof(SubDir) / sizeof(CString);//�����ǵò������ȵģ�sizeof(SubDir) == 8 ��CString��ָ�����ͣ�
	for (UINT_K i = 0; i < length; i++)
	{
		CreatePathDir(dstDir + SubDir[i]);
	}
}

void CFlowNavigatorDlg::OnStartstream()
{
	// TODO: �ڴ���������������
	if (m_hCam[0] == NULL)
	{
		AfxMessageBox(_T("���ȴ�ϵͳ��ϵͳ->��ϵͳ��!"));
		return;
	}
	On8col1line();
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_StartStream,TRUE);
	ShowCheckCamBt(this, Check_ID, SW_SHOW);
}

void CFlowNavigatorDlg::OnCloseStream()
{
	// TODO: �ڴ���������������
	if (m_hCam[0] == NULL)
	{
		AfxMessageBox(_T("���ȴ�ϵͳ��ϵͳ->��ϵͳ��!"));
		return;
	}
	On8col1line();
	CMenu* pMenu =GetMenu();
	pMenu->EnableMenuItem(ID_StartStream,FALSE);
	ShowCheckCamBt(this,Check_ID,SW_HIDE);
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		RecvFrame[i] = 0;
	}
	Invalidate(TRUE);
	DoubleClk = DOUBLECLK_IN;
	CamSign = -1; // �÷Ŵ���ͼ��ִ��DOUBLECLK_INʱ��DOUBLECLK_OUTʱ���ָ���ʾ_BUG_20160518

	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		Count[i]  = 0;
		Count_[i] = 0;
	}

	WPARAM wParam;
	LPARAM lParam;
	int system_state = -1;
	int content      = -1;
	lParam           = (LPARAM)content;

	for (int i = COLLECTION_NOT_START; i < CAM_IS_CLOSED+1; i++)
	{
		system_state = i;
		wParam       = (WPARAM)system_state;
		SendMessage(WM_MY_MESSAGE_INIT, wParam, lParam);
	}
}

BOOL CFlowNavigatorDlg::getLBDownState(CPoint point)
{
	int startHeight = checkShow->getStartHeight(FALSE);
	int bottom      = checkShow->getHeight() + startHeight;
	int intY = point.y;
	if((intY < startHeight) || (intY > bottom))
	{
		b_LBDown = FALSE;
		return b_LBDown;
	}
	b_LBDown = TRUE;

	return b_LBDown;
}


void CFlowNavigatorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if(DoubleClk == DOUBLECLK_IN)
	{
		return;
	}

	if(startClb == 1)
	{
		int ClbNum = imgClb->getDlbNum();
		CalibrationPoint* ClbPoint = imgClb->getCBPoint();


		int CurrentCamSign = imgClb->getCurrentCamSign();

		if(What_NumOfClb < ClbNum)
		{
			ClbPoint[What_NumOfClb].x_i = point.x;
			ClbPoint[What_NumOfClb].y_i = point.y;
			What_NumOfClb++;
		}

		if(What_NumOfClb == ClbNum)
		{
			imgClb->GenerateRelated_(checkShow, CurrentCamSign);
			imgClb->GenerateRelatedArray_(adjustImage_C[CurrentCamSign], checkShow, CurrentCamSign);

			relateArr[CurrentCamSign] = imgClb->getRelatedArray(CurrentCamSign);

			char szStr[256] = {0};
/*
			CString tempPath;
			tempPath.Format(_T("%s\\�����ļ�"), m_CurrentProPath);
			wcstombs(szStr, tempPath, tempPath.GetLength());//Unicodeת��ΪASCII
*/
			
			wcstombs(szStr, m_CurrentProPath, m_CurrentProPath.GetLength());//Unicodeת��ΪASCII
			const char * p = szStr;
			imgClb->Save_RelatedArray_ToDisk(p, CurrentCamSign);

			CString tips;
			tips.Format(_T("���%d�궨���!"), CurrentCamSign);
			AfxMessageBox(tips, MB_ICONINFORMATION);
			What_NumOfClb++;
		}
	}

	getLBDownState(point);

	b_LBUp   = FALSE;
	float x = point.x;
	float y = point.y;

	PointStart.x = point.x;
	PointStart.y = point.y;
	GetCursorPos(&PointEnd);
// 	CClientDC dc(this);
// 
// 	m_RectTracker.Draw(&dc);
// 	m_RectTracker.Track(this,point,TRUE);
// 	Invalidate();
	TRACE(_T("x=%f, y=%f\n"), x, y);
	CDialog::OnLButtonDown(nFlags, point);
}

void CFlowNavigatorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//��Ч��������� || ��Ч���һ�̧�� || ��������������"��һ��"���µ�
	if(!b_LBDown || !getLBDownState(point) || PointStart.x == PointEnd.x || PointStart.y == PointEnd.y)
	{
		b_LBDown = FALSE;
		return;
	}
	PointEnd.x = point.x;
	PointEnd.y = point.y;

	zoomImage->RegularTwoPoint(PointStart, PointEnd);
    checkShow->ReturnSignByPosition(PointStart, &CamSign, &CamSignNow);
	checkShow->GenerateShowStartAndSize(PointStart,PointEnd);

	b_LBDown = FALSE;
	b_LBUp   = TRUE;


// 	CRect rect(PointStart.x, PointStart.y, PointEnd.x, PointEnd.y);
// 	CClientDC clientDc(this);
// 	HDC hdc = clientDc.GetSafeHdc(); 
// 	SetROP2(hdc,R2_MASKPEN);
// 	clientDc.DrawFocusRect(rect);

	CDialog::OnLButtonUp(nFlags, point);
}

void CFlowNavigatorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(startClb == 1)
	{
		return;
	}

	if(solveFlow != -1 && mAdjust==TRUE)
	{
		st_FlowRate flowRate = {0};
		flowRate = SV_AdjustArg->getFlowRate(point, checkShow->getCheckCamSign(), checkShow->getNum(), adjustImage_C);

		static int oldX, oldY; 
		int newX, newY; 

		if (!g_TrackingMouse) 
		{ 
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT)}; 
			tme.hwndTrack = m_hWnd; 
			tme.dwFlags = TME_LEAVE; 

			TrackMouseEvent(&tme); 

			::SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem); 

			g_TrackingMouse = TRUE; 
		} 

		newX = point.x; 
		newY = point.y; 

		if ((newX != oldX) || (newY != oldY)) 
		{ 
			oldX = newX; 
			oldY = newY; 

			WCHAR coords[256]; 
			swprintf_s(coords, ARRAYSIZE(coords), _T("u=%f, v=%f, uv=%f"), flowRate.u, flowRate.v, flowRate.uv); 

			g_toolItem.lpszText = coords; 
			::SendMessage(g_hwndTrackingTT, TTM_SETTOOLINFO, 0, (LPARAM)&g_toolItem); 

			POINT pt = {newX, newY}; 
			::ClientToScreen(m_hWnd, &pt); 
			::SendMessage(g_hwndTrackingTT, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + 10, pt.y - 20)); 
		} 

	}

	if(!b_LBDown || !getLBDownState(point))
	{
		return;
	}
	PointEnd = point;
	zoomImage->DrawRectangle(PointStart, PointEnd);

	CDialog::OnMouseMove(nFlags, point);
}

//�ͻ��������Ч
void CFlowNavigatorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnLButtonDblClk(nFlags, point);

	//��ֹͼ��ƴ��ʱ˫������
	if(mAdjust)
	{
		return;
	}

	TRACE("left bt double clk!\n");

	//�ܸı�DoubleClk == DOUBLECLK_OUT ��ֻ�� �����if
	if (DoubleClk == DOUBLECLK_OUT)
	{
		DoubleClk = DOUBLECLK_IN;
		checkShow->setDoubleClk(FALSE);
		CamSign = -1; // �÷Ŵ���ͼ��ִ��DOUBLECLK_INʱ��DOUBLECLK_OUTʱ���ָ���ʾ_BUG_20160518
		//On8col1line();
		//UpdateWindow();
		for(int i = 0; i < MAX_CAMERAS; i++)
		{
			GetDlgItem(Check_ID[i])->ShowWindow(SW_SHOW);
		}

		Invalidate(TRUE);
		return;
	}

	if(m_hCam[0] != NULL && (GetDlgItem(Check_ID[0]) != NULL))
	{
		if (DoubleClk == DOUBLECLK_IN)
		{
			DoubleClk = DOUBLECLK_OUT;
			//DestroyCheckCamBt(checkBt, Check_ID);
			checkShow->setDoubleClk(TRUE);
			ShowCheckCamBt(this, Check_ID, SW_HIDE);
		}
// 		else if (DoubleClk == DOUBLECLK_OUT)
// 		{
// 			DoubleClk = DOUBLECLK_IN;
// 		}
	}
	else
	{
		//��ʱ����û����� ��ϵͳ->�򿪲ɼ��ն� ����
		return;
	}

	checkShow->setCheckCamSign(checkBt, MAX_CAMERAS);
	checkShow->getCheckNum(checkBt, MAX_CAMERAS, TRUE);
	//checkShow->ChangeButtonState(checkBt, MAX_CAMERAS, FALSE);

	CRect rect;
	GetClientRect(&rect);
	checkShow->setRect(rect);
}

void CFlowNavigatorDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if(DoubleClk != DOUBLECLK_OUT)
	{
		return;
	}
	CamSign = -1;
	checkShow->setXLostPixel(0);
	checkShow->setYLostPixel(0);
	checkShow->setXRealPixel(2560);
	checkShow->setYRealPixel(2048);
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CFlowNavigatorDlg::OnReboot()
{
	// TODO: �ڴ���������������
	OnCloseCamera();
	OnOpenCamera();
}


void CFlowNavigatorDlg::OnMouseLeave()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	::SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&g_toolItem); 
	g_TrackingMouse = FALSE; 
	CDialog::OnMouseLeave();
}


void CFlowNavigatorDlg::OnImageCalibrate()
{
	// TODO: �ڴ���������������
	CLBdlg2 = new ImageCalibrationDlg2;

	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		CLBdlg2->adjustImage[i] = adjustImage_C[i];
	}
	CLBdlg2->imgClb = imgClb;
	CLBdlg2->CurrentPath = m_CurrentProPath;
	CLBdlg2->Create(IDD_CLB_CHILD8, this);
	CLBdlg2->ShowWindow(SW_SHOW);
	CLBdlg2->UpdateWindow();
}


void CFlowNavigatorDlg::OnImageCalibrate2()
{
	// TODO: �ڴ���������������
	CLBdlg = new ImageCalibrationDlg;

	CLBdlg->Create(IDD_CLB_PARENT, this);
	CLBdlg->ShowWindow(SW_SHOW);
	CLBdlg->UpdateWindow();
}

LRESULT CFlowNavigatorDlg::OnMyMessegeInit(WPARAM wParam, LPARAM lParam)
{
	int system_state = (int)wParam;
	int I            = 0;          // �򿪽���
	CString str_state;
	
	switch(system_state)
	{
	case SYSTEM_WILL_OPEN:
		m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬: ���ڴ�"), TRUE);

		break;
	case SYSTEM_OPEN_ING:
		I = openCamCount * 100 / m_CameraCount;
		str_state.Format(_T("ϵͳ״̬:�����%d%%"), I);
		m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, str_state, TRUE);

		break;
	case SYSTEM_IS_OPEN:
		m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬: �Ѿ���"), TRUE);

		break;
	case SYSTEM_NOT_OPEN:
		m_statusBar.SetPaneText(INDEX_INDICATOR_INIT, _T("ϵͳ״̬��     �ѹر�"), TRUE);

		break;
	case COLLECTION_NOT_START:
		m_statusBar.SetPaneText(INDEX_INDICATOR_SAVE_IMG, _T("�ɼ�״̬��     δ��ʼ"), TRUE);

		break;
	case CAM_IS_CLOSED:
		m_statusBar.SetPaneText(INDEX_INDICATOR_CURRENT_CAMSIGN, _T("�Ѵ�������: "), TRUE);

		break;
	default:
		;
	}
	return 0;
}

void CFlowNavigatorDlg::OnCheckBtClick(UINT uID)
{
	int b_Check = ((CButton*)GetDlgItem(uID))->GetCheck();

	//��ȡ��ǰѡ�е�������λ�õ�
	checkShow->setCheckCamSign(checkBt, MAX_CAMERAS);
	checkShow->getCheckNum(checkBt, MAX_CAMERAS, TRUE);

	int* checkCamSign = checkShow->getCheckCamSign();
	int num = checkShow->getNum();

	CString text;
	CString first;
	for(int i = 0; i < num; i++)
	{
		text.Format(_T("%s,cam%d"), first,  checkCamSign[i]);
		first = text;
	}

	text.Format(_T("�Ѵ�������: %s"), first);
	m_statusBar.SetPaneText(INDEX_INDICATOR_CURRENT_CAMSIGN, text, TRUE);

	switch(uID)
	{
	case IDC_CheckBT1:

		break;
	case IDC_CheckBT2:

		break;
	case IDC_CheckBT3:

		break;
	case IDC_CheckBT4:

		break;
	case IDC_CheckBT5:

		break;
	case IDC_CheckBT6:

		break;
	case IDC_CheckBT7:

		break;
	case IDC_CheckBT8:

		break;
	default:
		;
	}
}