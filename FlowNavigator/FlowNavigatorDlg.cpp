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
int Count,Count1,Count2,Count3,
Count4,Count5,Count6,Count7;
int Count_[MAX_CAMERAS];

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

CCriticalSection g_cs,g_cs1,g_cs2,g_cs3,
g_cs4,g_cs5,g_cs6,g_cs7;

CWaitLoading *dlgWait       ;//�������ȴ��Ի���
UINT CreateWaitDlg(LPVOID pParam);
//UINT Write2File(LPVOID param);

#define  KDebug 0

#if KDebug
long bmpWidth;
long bmpHeight;
WORD kbiBitCount;
RGBQUAD *pColorTable=NULL;
unsigned char *pBmpBuf = NULL;
bool readBmp(char *bmpName)
{
	FILE *fp = fopen(bmpName,"rb");
	if (fp == NULL)
	{
		return FALSE;
	}
	fseek(fp,sizeof(BITMAPFILEHEADER),0);
	BITMAPINFOHEADER infoHead;
	
	fread(&infoHead,sizeof(BITMAPINFOHEADER),1,fp);
	bmpWidth = infoHead.biWidth;
	bmpHeight = infoHead.biHeight;
	kbiBitCount = infoHead.biBitCount;

	int lineByte = (bmpWidth*kbiBitCount/8 + 3)/4*4;
	if(kbiBitCount == 8)
	{
		pColorTable = new RGBQUAD[256];
		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}

	pBmpBuf = new unsigned char[lineByte*bmpHeight];
	fread(pBmpBuf,1,lineByte*bmpHeight,fp);
	fclose(fp);

	return true;
}

#endif

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


	m_hFactory = NULL;
	m_CameraCount = 0;
	CtrLine = 42;
	setDlg = NULL;
	
	Count=Count1=Count2=Count3=Count4=Count5=Count6=Count7=5;
	m_pAdjustCls = (pAdajustCLs)malloc(sizeof(AdajustCLs));//У���̴߳������
	showView = new ShowView(this); 
	m_CurrentProPath = DEFAULT_PATH;

	// Initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	for (int i=0; i< MAX_CAMERAS; i++)
	{
		m_hCam[i] = NULL;
		m_hView[i] = NULL;
		m_hThread[i] = NULL;
		head[i] = myImageInfo.CreateLink();//������������ʼ��ͷ�ڵ�
		Count_[i] = 0;
		test_count_getImage[i] = 20;
		RecvFrame[i] = 0;
		(m_CnvImageInfo[i]).pImageBuffer = NULL;// �����ÿգ�������治����ռ�
	}
	bmpinfo = (BITMAPINFO*)new char[256*sizeof(RGBQUAD)+ sizeof(BITMAPINFOHEADER)];
	memset(bmpinfo,0,sizeof(BITMAPINFO)); 
	bmpinfo->bmiHeader.biPlanes = 1;    
	bmpinfo->bmiHeader.biWidth=2560;
	bmpinfo->bmiHeader.biHeight=2048;
	//bmpinfo->bmiHeader.biSizeImage=2560*2048*8;
	bmpinfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);


	bmpinfo->bmiHeader.biCompression =BI_RGB;      
	bmpinfo->bmiHeader.biBitCount=8;
	bmpinfo->bmiHeader.biClrUsed=0;//Ϊ0����2^biBitCount
	//bmpinfo->bmiHeader.biClrImportant=256;


	for( int j = 0; j < 256; j++ )
	{
		bmpinfo->bmiColors[j].rgbBlue =  (BYTE) (0xff & j);
		bmpinfo->bmiColors[j].rgbGreen = (BYTE)j;// bmpinfo->bmiColors[j].rgbBlue;
		bmpinfo->bmiColors[j].rgbRed =(BYTE)j;// bmpinfo->bmiColors[j].rgbBlue;
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

}

CFlowNavigatorDlg::~CFlowNavigatorDlg()
{
	delete bmpinfo;
	delete showView;
	if (m_pAdjustCls != NULL)
	{
		free(m_pAdjustCls);
		m_pAdjustCls = NULL;
	}
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		free(head[i]);
		head[i] = NULL;
	}
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
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
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

	NODE_HANDLE hNode;
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
	    J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[j],"AcquisitionFrameRate",&hNode);
		J_Node_SetValueDouble(hNode,TRUE,10.0);
		

        
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
		}

		CFlowNavigatorDlg::SaveImage_CWJ(pAqImageInfo, m_CnvImageInfo, *Count, numCam , path);
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
			AfxMessageBox(_T("����ģʽ�ɼ����"), MB_ICONINFORMATION);
			return -1;
		}
	}
	else if (bSingleFrame)
	{
		SaveImage(m_CurrentProPath, pAqImageInfo, m_CnvImageInfo, Count_Single, numCamSingle);

		if (*Count_Single == 0)
		{
			AfxMessageBox(_T("����ģʽ�ɼ����"), MB_ICONINFORMATION);
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
void CFlowNavigatorDlg::StreamCBFunc(J_tIMAGE_INFO *pAqImageInfo)
{
	float begin = clock();

	(RecvFrame[0])++;
	CRect rect;
	GetClientRect(&rect);

	if (mAdjust)
	{ 
		if (!AdjustING)
		{
			adjustImage_C[0]->setImageInfo(pAqImageInfo);
		}
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
			LiveView_Cwj(pAqImageInfo,bmpinfo,0,0,rect.Width()/8,rect.Height(),HALFTONE);
		}	
		if(b_isContinuous)
		{
			if (Count > 0)
			{
				float begin = clock();
				g_cs.Lock();
				//���յ����ݲ�����ͷ��������β�������̶߳�������ɾ���ڵ�
				if(-1 == InsertHead(pImage[0],pAqImageInfo,myImageInfo,head[0]))
				{
					AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
					OnClose();
				}
				Count--;
				g_cs.Unlock();
				float end = clock();
				TRACE("InsertHead:%f\n",end - begin);//2����
			}
		}
		else if (b_isSingleFrame)
		{
			g_cs.Lock();
			insertSingle(&(Count_Single[0]),&(Count_Once[0]), &(Space_Frame[0]),
				Copy_Space_Frame[0],Copy_Count_Once[0],
				&(pImage[0]), pAqImageInfo, myImageInfo, &(head[0]));
			g_cs.Unlock();
		}
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

	DrawTextByCamSign(this, showView, RecvFrame[0], iLostFrames, 0, 0);
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

	float end = clock();
	TRACE("LiveView coast time = %f\n",end - begin);
}

void CFlowNavigatorDlg::StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo)
{
	(RecvFrame[1])++;

	CRect rect;
	GetClientRect(&rect);
	if (mAdjust)
	{
		if (!AdjustING)
		{
			adjustImage_C[1]->setImageInfo(pAqImageInfo);
		}
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
			LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8,0,rect.Width()/8,rect.Height(),HALFTONE);
		}
		if(b_isContinuous)
		{
			if(Count1 > 0){
				g_cs1.Lock();
				if(-1 == InsertHead(pImage[1],pAqImageInfo,myImageInfo,head[1]))
				{
					AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
					OnClose();
				}
				Count1--;
				g_cs1.Unlock();
				//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo1,Count1,"1");
			}
		}
	}

	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[1], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[1], iLostFrames, rect.Width() / 4, 0);

}

void CFlowNavigatorDlg::StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo)
{	
	(RecvFrame[2])++;
	CRect rect;
	GetClientRect(&rect);
	if (CtrLine == 42)
	{
	    LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/2,0,rect.Width()/4,rect.Height()/2,HALFTONE);
	}
	else if (CtrLine == 81)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*2,0,rect.Width()/8,rect.Height(),HALFTONE);
	}
	if(b_isContinuous)
	{
		if(Count2 > 0){
			g_cs2.Lock();
			if(-1 == InsertHead(pImage[2],pAqImageInfo,myImageInfo,head[2]))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			Count2--;
			g_cs2.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo2,Count2,"2");
			TRACE("���2��%d\n",Count2);
		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[2], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[2], iLostFrames, rect.Width()/ 2, 0);
}
void CFlowNavigatorDlg::StreamCBFunc3(J_tIMAGE_INFO * pAqImageInfo)
{
	(RecvFrame[3])++;
	CRect rect;
	GetClientRect(&rect);
	if(CtrLine == 42)
	{
        LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4*3,0,rect.Width()/4,rect.Height()/2,HALFTONE);
	}
	else if (CtrLine == 81)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*3,0,rect.Width()/8,rect.Height(),HALFTONE);
	}
	if(b_isContinuous)
	{
		if(Count3 > 0){
			g_cs3.Lock();
			if(-1 == InsertHead(pImage[3],pAqImageInfo,myImageInfo,head[3]))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			Count3--;
			g_cs3.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo3,Count3,"3");
			TRACE("���3��%d\n",Count3);
		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[3], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[3], iLostFrames, rect.Width() / 4 * 3, 0);

}
void CFlowNavigatorDlg::StreamCBFunc4(J_tIMAGE_INFO * pAqImageInfo)
{
	(RecvFrame[4])++;
	CRect rect;
	GetClientRect(&rect);
	if(CtrLine ==42)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,0,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
	}
	else if (CtrLine == 81)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*4,0,rect.Width()/8,rect.Height(),HALFTONE);
	}
	if(b_isContinuous)
	{
		if(Count4 > 0){
			g_cs4.Lock();
			if(-1 == InsertHead(pImage[4],pAqImageInfo,myImageInfo,head[4]))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			Count4--;
			g_cs4.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo4,Count4,"4");
			TRACE("���4��%d\n",Count4);
		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[4], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[4], iLostFrames, 0, rect.Height() / 2);

}
void CFlowNavigatorDlg::StreamCBFunc5(J_tIMAGE_INFO * pAqImageInfo)
{
	(RecvFrame[5])++;
	CRect rect;
	GetClientRect(&rect);
	if (CtrLine == 42)
	{	
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
	}
	else if (CtrLine == 81)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*5,0,rect.Width()/8,rect.Height(),HALFTONE);
	}
	if(b_isContinuous)
	{
		if(Count5 > 0){
			g_cs5.Lock();
			if(-1 == InsertHead(pImage[5],pAqImageInfo,myImageInfo,head[5]))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			Count5--;
			g_cs5.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo5,Count5,"5");
			TRACE("���5��%d\n",Count5);
		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[5], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[5], iLostFrames, rect.Width() / 4, rect.Height() / 2);

}
void CFlowNavigatorDlg::StreamCBFunc6(J_tIMAGE_INFO * pAqImageInfo)
{
	(RecvFrame[6])++;
	CRect rect;
	GetClientRect(&rect);
	if (CtrLine == 42)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/2,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
	}
	else if (CtrLine == 81) 
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*6,0,rect.Width()/8,rect.Height(),HALFTONE);
	}
	if(b_isContinuous)
	{
		if(Count6 > 0){
			g_cs6.Lock();
			if(-1 == InsertHead(pImage[6],pAqImageInfo,myImageInfo,head[6]))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			Count6--;
			g_cs6.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo6,Count6,"6");
			TRACE("���6��%d\n",Count6);
		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	J_Image_GetStreamInfo(m_hThread[6], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[6], iLostFrames, rect.Width() / 2, rect.Height() / 2);

}
void CFlowNavigatorDlg::StreamCBFunc7(J_tIMAGE_INFO * pAqImageInfo)
{
	(RecvFrame[7])++;
	CRect rect;
	GetClientRect(&rect);
	if (CtrLine ==42)
	{	
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/4*3,rect.Height()/2,rect.Width()/4,rect.Height()/2,HALFTONE);
	}
	else if (CtrLine == 81)
	{
		LiveView_Cwj(pAqImageInfo,bmpinfo,rect.Width()/8*7,0,rect.Width()/8,rect.Height(),HALFTONE);
	}
	if(b_isContinuous)
	{
		if(Count7 > 0){
			g_cs7.Lock();
			if(-1 == InsertHead(pImage[7],pAqImageInfo,myImageInfo,head[7]))
			{
				AfxMessageBox(_T("�ڴ治�㣡"),MB_ICONINFORMATION);
				OnClose();
			}
			Count7--;
			g_cs7.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo7,Count7,"7");
			TRACE("���7��%d\n",Count7);
		}
	}
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	J_Image_GetStreamInfo(m_hThread[7], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);

	DrawTextByCamSign(this, showView, RecvFrame[7], iLostFrames, rect.Width() / 4 * 3, rect.Height() / 2);

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
����nXDest��ָ��Ŀ������������Ͻǵ�X���߼����ꡣ 
����nYDest��ָ��Ŀ������������Ͻǵ�Y���߼����ꡣ 
����nWidth��ָ��Դ��Ŀ�����������߼���ȡ� 
����nHeight��ָ��Դ��Ŀ�����������߼��߶ȡ� 
����hdcSrc��ָ��Դ�豸�����ľ���� 
����nXSrc��ָ��Դ�����������Ͻǵ�X���߼����ꡣ 
����nYSrc��ָ��Դ�����������Ͻǵ�Y���߼����ꡣ 
����nWidthSrc��ָ��Դ���εĿ�ȣ����߼���λ��ʾ��ȡ� 
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
	b_isContinuous = FALSE;
	mAdjust = FALSE;
}

void CFlowNavigatorDlg::EnableControls()
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		RecvFrame[i] = 0;
	}
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

UINT CreateWaitDlg(LPVOID pParam)
{
	dlgWait = new CWaitLoading;
	dlgWait->Create(IDD_DIALOG2,AfxGetMainWnd());
	dlgWait->ShowWindow(SW_SHOW);
	dlgWait->UpdateWindow();
	dlgWait->StartTimes();
	Sleep(5000);

	return 0;
}
void CFlowNavigatorDlg::OnOpenCamera()
{
	// TODO: �ڴ���������������
	TRACE("OpenCamera\n");

	//AfxBeginThread(CreateWaitDlg,this);

	dlgWait = new CWaitLoading;
	dlgWait->Create(IDD_DIALOG2,AfxGetMainWnd());
	dlgWait->ShowWindow(SW_SHOW);
	dlgWait->UpdateWindow();
	dlgWait->StartTimes();

	OpenFactoryAndCamera();

	bt_on = TRUE;
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_OpenCamera,bt_on);
	//dlgWait->KillTimes();
}

void CFlowNavigatorDlg::OnCloseCamera()
{
	// TODO: �ڴ���������������
	EnableControls();
	CloseFactoryAndCamera();

	bt_on = FALSE;
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_OpenCamera,bt_on);
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
}

void CFlowNavigatorDlg::On8col1line()
{
	// TODO: �ڴ���������������
	CtrLine = 81;
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

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[0]), &(Total_Frame[0]),
				"0", "0_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[0]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

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

				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();
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
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[1]), &(Total_Frame[1]),
				"1", "1_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[1]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
			
		}
	}
End:
    return 0;
} 
UINT Write2File2(LPVOID param)
{
	while(b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs2.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs2.Unlock();

		if (pTail == NULL)
		{
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[2]), &(Total_Frame[2]),
				"2", "2_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[2]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File3(LPVOID param)
{
	while(b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs3.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs3.Unlock();

		if (pTail == NULL)
		{
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[3]), &(Total_Frame[3]),
				"3", "3_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[3]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File4(LPVOID param)
{
	while(b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs4.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs4.Unlock();

		if (pTail == NULL)
		{
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[4]), &(Total_Frame[4]),
				"4", "4_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[4]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File5(LPVOID param)
{
	while(b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs5.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs5.Unlock();

		if (pTail == NULL)
		{
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[5]), &(Total_Frame[5]),
				"5", "5_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[5]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File6(LPVOID param)
{
	while(b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs6.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs6.Unlock();

		if (pTail == NULL)
		{
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[6]), &(Total_Frame[6]),
				"6", "6_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[6]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
		}
	}
End:
    return 0;
} 
UINT Write2File7(LPVOID param)
{
	while(b_isContinuous)
	{
		pImageNode pHead = (pImageNode)param;
		g_cs7.Lock();
		pImageNode pTail = ImageInfo::getTailNode(pHead);
		g_cs7.Unlock();

		if (pTail == NULL)
		{
			Sleep(10);
		}
		else
		{
			(pHead->AqImageInfo).pImageBuffer = pHead->pImageBuffer;

			if(-1 == SaveImageBySign(b_isContinuous, b_isSingleFrame, &(Count_[7]), &(Total_Frame[7]),
				"7", "7_", m_CurrentProPath, &(pTail->AqImageInfo), m_CnvImageInfo[7]))
			{
				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();

				b_isContinuous = b_isSingleFrame = FALSE;
				goto End;
			}

			g_cs1.Lock();
			ImageInfo::DelTail(pHead);
			g_cs1.Unlock();
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
	CSetSampleDlg *samDlg;
	samDlg = new CSetSampleDlg;
	samDlg->m_CameraCount = m_CameraCount;
	for (int i = 0; i < m_CameraCount; i++)
	{
		samDlg->m_hCam[i] = m_hCam[i];
	}
	samDlg->DoModal();
	b_isContinuous = TRUE;
	Count=Count1=Count2=Count3=Count4=Count5=Count6=Count7=samDlg->Count;
	for (int i = 0; i < m_CameraCount; i++)
	{
		Count_[i] = samDlg->Count;
	}
		
	hThread[0] = ::AfxBeginThread(Write2File ,head[0],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[1] = ::AfxBeginThread(Write2File1,head[1],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[2] = ::AfxBeginThread(Write2File2,head[2],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[3] = ::AfxBeginThread(Write2File3,head[3],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[4] = ::AfxBeginThread(Write2File4,head[4],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[5] = ::AfxBeginThread(Write2File5,head[5],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[6] = ::AfxBeginThread(Write2File6,head[6],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[7] = ::AfxBeginThread(Write2File7,head[7],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);

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
	if(J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[0],"AcquisitionFrameRate",&hNode))
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
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		Copy_Space_Frame[i] = Space_Frame[0];
		Copy_Count_Once[i]  = Count_Once[0];
		Total_Frame[i]      = Count_Once[0] * Count_Single[0];
	}
	b_isSingleFrame = TRUE;
	hThread[0] = ::AfxBeginThread(Write2File ,head[0],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[1] = ::AfxBeginThread(Write2File1,head[1],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[2] = ::AfxBeginThread(Write2File2,head[2],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[3] = ::AfxBeginThread(Write2File3,head[3],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[4] = ::AfxBeginThread(Write2File4,head[4],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[5] = ::AfxBeginThread(Write2File5,head[5],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[6] = ::AfxBeginThread(Write2File6,head[6],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
	hThread[7] = ::AfxBeginThread(Write2File7,head[7],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);

}

void CFlowNavigatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
// 	Invalidate(TRUE);
	TRACE("�������������\n");
	//::AfxMessageBox((LPCTSTR)"1",MB_ICONINFORMATION);

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

void CFlowNavigatorDlg::OnAdjust()
{
	// TODO: �ڴ���������������
	relateArr = (unsigned char *)malloc(2560*2048*sizeof(char));
	//�����Ժ�Ӹ�if(ͼƬ������.ͼƬ����==NULL) return��
	for (int i = 0; i < m_CameraCount; i++)
	{
		adjustImage_C[i] = new AdjustImage(2560*2048,relateArr);
	}
	mAdjust = true;
	
	for (int j = 0; j < m_CameraCount; j ++)
	{
		m_pAdjustCls->adjustImage_C[j] = adjustImage_C[j];
	}
	m_pAdjustCls->showView = new ShowView(this);
	m_pAdjustCls->pWnd = this;
	m_pAdjustCls->Count = m_CameraCount;

	m_pAdjustCls->linkImage = new LinkImage();
	m_pAdjustCls->pTwoImageInfo = (st_IMAGE_INFO *)malloc(sizeof(st_IMAGE_INFO));
	m_pAdjustCls->pTwoImageInfo->pOutImage = NULL;

	::AfxBeginThread(AdjustThreadFunc::AdjustIm,m_pAdjustCls,THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);	
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
	mLink = FALSE;
	mAdjust = FALSE;
	b_isContinuous = FALSE;
	b_isSingleFrame = FALSE;
}

void CFlowNavigatorDlg::OnCreateproj()
{
	// TODO: �ڴ���������������
	CString dstDir = DEFAULT_PATH;

	CString SubDir[MAX_SUBDIR] = {_T("\\ԭʼ����"), _T("\\ͼ��ƴ��"), _T("\\����ƴ��"), _T("\\�����ļ�")};

	if(IDOK == SelectDir(dstDir,GetSafeHwnd()))
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
