 // FlowNavigatorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "FlowNavigatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//PathIsDirectory()
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
//保存Image
J_tIMAGE_INFO m_CnvImageInfo,m_CnvImageInfo1,m_CnvImageInfo2,m_CnvImageInfo3,m_CnvImageInfo4,
m_CnvImageInfo5,m_CnvImageInfo6,m_CnvImageInfo7;

int Count,Count1,Count2,Count3,
Count4,Count5,Count6,Count7;
int Count_[MAX_CAMERAS];

BOOL b_isContinuous = FALSE;//true:连续模式 false:潮流
CCriticalSection g_cs,g_cs1,g_cs2,g_cs3,
g_cs4,g_cs5,g_cs6,g_cs7;

CWaitLoading *dlgWait;//进度条等待对话框
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


// CFlowNavigatorDlg 对话框
CFlowNavigatorDlg::CFlowNavigatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlowNavigatorDlg::IDD, pParent)
	//,b_isContinuous(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_hFactory = NULL;
	m_CameraCount = 0;
    test_count_getImage = 0;
	CtrLine = 42;
	setDlg = NULL;
	m_CnvImageInfo.pImageBuffer = NULL;
	m_CnvImageInfo1.pImageBuffer = NULL;
	m_CnvImageInfo2.pImageBuffer = NULL;
	m_CnvImageInfo3.pImageBuffer = NULL;
	m_CnvImageInfo4.pImageBuffer = NULL;
	m_CnvImageInfo5.pImageBuffer = NULL;
	m_CnvImageInfo6.pImageBuffer = NULL;
	m_CnvImageInfo7.pImageBuffer = NULL;
	Count=Count1=Count2=Count3=Count4=Count5=Count6=Count7=5;
	RecvFrame=RecvFrame1=0;

	for (int i=0; i< MAX_CAMERAS; i++)
	{
		m_hCam[i] = NULL;
		m_hView[i] = NULL;
		m_hThread[i] = NULL;
		head[i] = myImageInfo.CreateLink();//创建单链表，初始化头节点
		Count_[i] = 0;
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
	bmpinfo->bmiHeader.biClrUsed=0;//为0就用2^biBitCount
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
END_MESSAGE_MAP()


// CFlowNavigatorDlg 消息处理程序

BOOL CFlowNavigatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitializeControls();

	m_PixelValue.PixelValueUnion.Mono16Type.Value = 0;
	m_PixelType = J_GVSP_PIX_MONO8; 

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFlowNavigatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();//你永远也想不到这里注释掉，定时器只有在最小化时才运行吧，多么痛的领悟wtf！   _kui
	}
// 	CRect k_rect;
// 	GetClientRect(k_rect);
// 	CPaintDC k_dc(this);
// 	k_dc.SetMapMode(MM_ANISOTROPIC);
// 	k_dc.SetWindowExt(500,500);
// 	k_dc.SetViewportExt(k_rect.Width(),k_rect.Height());
// 	k_dc.Ellipse(0,0,500,500);
}
 
//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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

		if(0 == strncmp("TL=>GevTL , INT=>FD", (char*)m_sCameraId[iValidCamera], 19))//这里字符串中间逗号两侧都有空格‘\0’也参与比较
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

		//设置初始帧率值，否则默认22帧，8台同时开，卡死
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

		// Start Acquisition
		retval = J_Camera_ExecuteCommand(m_hCam[j], NODE_NAME_ACQSTART);
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


/*这个是实现一个相机的代码，是我尝试数字图像处理的开端
void CFlowNavigatorDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{

	TRACE("Line%d:get image%d\n",__LINE__,test_count_getImage++);
#if KDebug

	//J_Image_ShowImage(::GetDC(this->m_hWnd),pAqImageInfo);

#endif
	
	//CClientDC dcScreen(this);

// 	BYTE *pData;
// 	pData = new BYTE[2560*2048];
    
	//由于是赋值给位图数组，这么做没必要
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
	
	//bmpinfo 要分配空间
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

	//这句不加图片失真
 	pDC->SetStretchBltMode(HALFTONE);//COLORONCOLOR：直接將不要的点删掉.：
	                                  //HALFTONE將要删掉的像素跟周围的中和起來,可得到较好的画质 不过效率要牺牲点
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
int CFlowNavigatorDlg::SaveImage_CWJ(J_tIMAGE_INFO* pAqImageInfo, J_tIMAGE_INFO& m_CnvImageInfo, int Count,
									 const char* numCam,
									 const char* path, const char* fileName)
{
	//限制多线程访问，否则存出来的图会串掉（可能有的图的内容是几幅图片揉在一起的情况）//由于共享全局变量
	//EnterCriticalSection(&m_CriticalSection);

	CString filePath;
	char str[MAX_PATH];
	//J_tIMAGE_INFO **ppAqImageInfo = new J_tIMAGE_INFO *[MAX_IMG_COUNT];
	/*
	for (int i = 0; i < Count; i++)
	{
//		J_tIMAGE_INFO *ppAqImageInfo[i] = 
	}
	*/

	/*这里注掉的的理由同LiveView_CWJ中使用时的理由*/
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
				//得到exe的完整路径，包括exe的文件名
				GetModuleFileName(NULL,LPCH(str),MAX_PATH);
				CString pathDir(str);
				int pos = pathDir.ReverseFind('\\');
				pathDir = pathDir.Left(pos);

				CString strNumCam(numCam);
				pathDir += "\\"+ strNumCam;
				if(!PathIsDirectory(pathDir))
				{
					::CreateDirectory(pathDir,NULL);
				}

				filePath.Format("%s\\%d.tiff",pathDir,Count);
				filePath.Replace("\\","\\\\");

				J_Image_SaveFileA(&m_CnvImageInfo, (LPCSTR)(filePath)); // Stores RGB image to a TIFF file.
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
	//分配失败
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

void CFlowNavigatorDlg::StreamCBFunc(J_tIMAGE_INFO *pAqImageInfo)
{
// 	fRecvFrame++;
// 	TRACE("fRecvFrame = %d\n",fRecvFrame);
	CRect rect;
	GetClientRect(&rect);
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
			//接收的数据插链表头，从链表尾用其他线程读，读完删除节点
			if(-1 == InsertHead(pImage[0],pAqImageInfo,myImageInfo,head[0]))
			{
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count--;
			g_cs.Unlock();
			float end = clock();
			TRACE("InsertHead:%f\n",end - begin);//2毫秒
		}
	}
	RecvFrame++;
	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[0], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);
	iSize = sizeof(uint64_t);
	// get the number of frames which has been corrupted 
	J_Image_GetStreamInfo(m_hThread[0], STREAM_INFO_CMD_NUMBER_OF_FRAMES_CORRUPT_ON_DELIEVRY, &iCurruptedFrames, &iSize);
	TRACE("相机0：%d,LostFrames=%d,CurruptedFrames=%d,RecvFrame=%d\n",Count,iLostFrames,iCurruptedFrames,RecvFrame);

}
void CFlowNavigatorDlg::StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo)
{
	CRect rect;
	GetClientRect(&rect);
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count1--;
			g_cs1.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo1,Count1,"1");
		}
	}
	RecvFrame1++;

	uint64_t iCurruptedFrames = -1; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = -1; // received but thrown for lacking of buffers in acquisition engine
	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread[1], STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);
	iSize = sizeof(uint64_t);
	// get the number of frames which has been corrupted 
	J_Image_GetStreamInfo(m_hThread[1], STREAM_INFO_CMD_NUMBER_OF_FRAMES_CORRUPT_ON_DELIEVRY, &iCurruptedFrames, &iSize);

	TRACE("相机1：%d,LostFrames=%d,CurruptedFrames=%d,RecvFrame=%d\n",Count1,iLostFrames,iCurruptedFrames,RecvFrame);

}

void CFlowNavigatorDlg::StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo)
{	
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count2--;
			g_cs2.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo2,Count2,"2");
			TRACE("相机2：%d\n",Count2);
		}
	}

}
void CFlowNavigatorDlg::StreamCBFunc3(J_tIMAGE_INFO * pAqImageInfo)
{
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count3--;
			g_cs3.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo3,Count3,"3");
			TRACE("相机3：%d\n",Count3);
		}
	}

}
void CFlowNavigatorDlg::StreamCBFunc4(J_tIMAGE_INFO * pAqImageInfo)
{
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count4--;
			g_cs4.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo4,Count4,"4");
			TRACE("相机4：%d\n",Count4);
		}
	}

}
void CFlowNavigatorDlg::StreamCBFunc5(J_tIMAGE_INFO * pAqImageInfo)
{
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count5--;
			g_cs5.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo5,Count5,"5");
			TRACE("相机5：%d\n",Count5);
		}
	}

}
void CFlowNavigatorDlg::StreamCBFunc6(J_tIMAGE_INFO * pAqImageInfo)
{
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count6--;
			g_cs6.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo6,Count6,"6");
			TRACE("相机6：%d\n",Count6);
		}
	}

}
void CFlowNavigatorDlg::StreamCBFunc7(J_tIMAGE_INFO * pAqImageInfo)
{
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
				AfxMessageBox(_T("内存不足！"),MB_ICONINFORMATION);
				OnClose();
			}
			Count7--;
			g_cs7.Unlock();
			//SaveImage_CWJ(pAqImageInfo,m_CnvImageInfo7,Count7,"7");
			TRACE("相机7：%d\n",Count7);
		}
	}

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
	/*之所以这里注掉，我猜这个是开了一个线程在做翻转 并将旋转pAaImageInfo的值排列赋值给
	pAaImageInfo（这个赋值应该是在线程完成），导致，
	其他线程中要是再调用这个东西会使得pAImageInfo紊乱*/
	//J_Image_Flip(pAqImageInfo,J_FLIP_VERTICAL);//SetDIBitsToDevice这玩意，参数设置也能搞出上下翻转效果来
	//float end = clock();
	//TRACE("flip time: %f\n",end-begin);   //5到7毫秒之间

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
 
　　hdcDest：指向目标设备环境的句柄。 
　　nXDest：指定目标矩形区域左上角的X轴逻辑坐标。 
　　nYDest：指定目标矩形区域左上角的Y轴逻辑坐标。 
　　nWidth：指定源和目标矩形区域的逻辑宽度。 
　　nHeight：指定源和目标矩形区域的逻辑高度。 
　　hdcSrc：指向源设备环境的句柄。 
　　nXSrc：指定源矩形区域左上角的X轴逻辑坐标。 
　　nYSrc：指定源矩形区域左上角的Y轴逻辑坐标。 
　　nWidthSrc：指定源矩形的宽度，按逻辑单位表示宽度。 
　　nHeightSrc：指定源矩形的高度，按逻辑单位表示高度。
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
void CFlowNavigatorDlg::EnableControls()
{

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
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
	b_isContinuous = FALSE;
	
	CloseFactoryAndCamera();

	bt_on = FALSE;
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_OpenCamera,bt_on);
}

void CFlowNavigatorDlg::OnQuit()
{
	// TODO: 在此添加命令处理程序代码
	b_isContinuous = FALSE;

	OnOK();
	CloseFactoryAndCamera();
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
	CDialog::OnDestroy();
}

void CFlowNavigatorDlg::OnSetValues()
{
	// TODO: 在此添加命令处理程序代码
	TRACE("SetValues\n");
	int retval;
	int64_t int64Val;
	CSliderCtrl *pSCtrl;
	//SetTimer(1,1000,NULL);
	if(setDlg == NULL )
	{
		setDlg = new CSetValue;
	}
	setDlg->m_CameraCount = m_CameraCount;

	for (int i = 0; i<m_CameraCount; i++)
	{
		setDlg->m_hCam[i]= m_hCam[i];
	}

	setDlg->Create(IDD_DIALOG1,this);
	setDlg->ShowWindow(SW_SHOW);
	//setDlg->Start();
	setDlg->SetRangeMM();//顺序很重要必须窗体创建了，才能使用，否则Assert出错

	setDlg->UpdateWindow();//不加这句会导致 弹出非模态对话框时，里边控件一开始不显示（拖动一下对话框就显示）


/*
	retval = J_Camera_GetNodeByName(m_hCam[0], NODE_NAME_EXPOSURE, &m_hExposureNode);

	retval = J_Node_GetValueInt64(m_hExposureNode, TRUE, &int64Val);
    retval = J_Camera_GetNodeByName(m_hCam[1], NODE_NAME_EXPOSURE, &m_hExposureNode);

	//retval = J_Node_SetValueInt64(m_hExposureNode, TRUE, (int64_t)100);
	retval = J_Node_GetValueInt64(m_hExposureNode, TRUE, &int64Val);
	//retval = J_Node_SetValueInt64(m_hExposureNode, TRUE, (int64_t)29000);
*/
}

void CFlowNavigatorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_hCam[0] == NULL)
	{
		return;
	}

	CSliderCtrl *pSCtrl;
	int iPos;
	J_STATUS_TYPE retval;
	NODE_HANDLE hNode;
	int64_t int64Val;

	if (m_hCam[0] != NULL)
	{
		pSCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER1);
		retval = J_Camera_GetNodeByName(m_hCam[0],NODE_NAME_EXPOSURE,&m_hExposureNode);
		if (retval == J_ST_SUCCESS)
		{
			retval = J_Node_GetMinInt64(m_hExposureNode,&int64Val);
			SetDlgItemInt(IDC_MinExposure,(int)int64Val);
			pSCtrl->SetRangeMin((int)int64Val,TRUE);
			int minExposure = (int)int64Val;

			retval = J_Node_GetMaxInt64(m_hExposureNode,&int64Val);
			SetDlgItemInt(IDC_MaxExposure,(int)int64Val);
			pSCtrl->SetRangeMax((int)int64Val,TRUE);
			int maxExposure = (int)int64Val;

			retval = J_Node_GetValueInt64(m_hCam[0],TRUE,&int64Val);
			SetDlgItemInt(IDC_CurExposure,(int)int64Val);	

			pSCtrl->SetSelection(minExposure,maxExposure);
			pSCtrl->SetTicFreq((maxExposure-minExposure)/20);
		}
	}

	//pSCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER1);

	if (pSCtrl == (CSliderCtrl *)pScrollBar)
	{
		iPos = pSCtrl->GetPos();//current position

		retval = J_Camera_GetNodeByName(m_hCam,NODE_NAME_EXPOSURE,&m_hExposureNode);
		retval = J_Node_SetValueInt64(m_hExposureNode,TRUE,iPos);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFlowNavigatorDlg::On4Col2Line()
{
	// TODO: 在此添加命令处理程序代码
	CtrLine = 42;
}

void CFlowNavigatorDlg::On8col1line()
{
	// TODO: 在此添加命令处理程序代码
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

	while(b_isContinuous)
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

			if(Count_[0] > 0)
			{
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo,Count_[0],"0");

				g_cs.Lock();
				ImageInfo::DelTail(pHead);
				g_cs.Unlock();
				(Count_[0])--;
			}
		}

		float end = clock();
		TRACE("Write2File time %d: %lf\n",Count_[0],end-begin);
	}	
	return 0;
}

UINT Write2File1(LPVOID param)
{
	while(b_isContinuous)
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

			if(Count_[1] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo1,Count_[1],"1");

				g_cs1.Lock();
				ImageInfo::DelTail(pHead);
				g_cs1.Unlock();
				(Count_[1])--;
			}
		}
	}
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

			if(Count_[2] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo2,Count_[2],"2");

				g_cs2.Lock();
				ImageInfo::DelTail(pHead);
				g_cs2.Unlock();
				(Count_[2])--;
			}
		}
	}

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

			if(Count_[3] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo3,Count_[3],"1");

				g_cs3.Lock();
				ImageInfo::DelTail(pHead);
				g_cs3.Unlock();
				(Count_[3])--;
			}
		}
	}

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

			if(Count_[4] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo4,Count_[4],"1");

				g_cs4.Lock();
				ImageInfo::DelTail(pHead);
				g_cs4.Unlock();
				(Count_[4])--;
			}
		}
	}

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

			if(Count_[5] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo5,Count_[5],"1");

				g_cs5.Lock();
				ImageInfo::DelTail(pHead);
				g_cs5.Unlock();
				(Count_[5])--;
			}
		}
	}

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

			if(Count_[6] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo6,Count_[6],"1");

				g_cs6.Lock();
				ImageInfo::DelTail(pHead);
				g_cs6.Unlock();
				(Count_[6])--;
			}
		}
	}

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

			if(Count_[7] > 0){
				CFlowNavigatorDlg::SaveImage_CWJ(&(pTail->AqImageInfo),m_CnvImageInfo7,Count_[7],"1");

				g_cs7.Lock();
				ImageInfo::DelTail(pHead);
				g_cs7.Unlock();
				(Count_[7])--;
			}
		}
	}

    return 0;
} 
void CFlowNavigatorDlg::OnContinuous()
{
	// TODO: 在此添加命令处理程序代码
	//考虑到了 在 相机未开状态，以及开过一次但是关闭状态下，拒绝点击采集模式
	if(m_CameraCount <= 0)
	{
		AfxMessageBox(_T("请先进行参数设置!"),MB_ICONINFORMATION);
		return;
	}
	if(m_hCam[0] == NULL)
	{
		AfxMessageBox(_T("请打开相机!"),MB_ICONINFORMATION);
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
		
	hThread[0] = ::AfxBeginThread(Write2File,head[0],THREAD_PRIORITY_NORMAL/*,0,CREATE_SUSPENDED*/);
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
	// TODO: 在此添加命令处理程序代码
	b_isContinuous = FALSE;
}

void CFlowNavigatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
// 	Invalidate(TRUE);
	TRACE("这个在主窗口中\n");
	//::AfxMessageBox((LPCTSTR)"1",MB_ICONINFORMATION);

	CDialog::OnTimer(nIDEvent);
}

void CFlowNavigatorDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	b_isContinuous = FALSE;//否则 存储线程不能正常退出，其中的临界区出现报错，中断
	CloseFactoryAndCamera();
	if (setDlg != NULL)
	{	
		delete setDlg;
		setDlg = NULL;
	}

	CDialog::OnClose();
}


