#include "StdAfx.h"
#include "ShowView.h"
#include <math.h>
#include "KGlobalDefine.h"
#include "KGloabalFunc.h"
#include "KGloabalVar.h"

void CreateMatrixByCol(float* px, float* py, int xSmall, int xLarge, int ySmall, int yLarge,
					   int step)
{

	int x_j  = 0;
	int y_j  = 0;
	int nRow = 0;
	int nCol = 0;

	nRow     = (yLarge - ySmall)/step + 1;
	nCol     = (xLarge - xSmall)/step + 1;
	int size = nRow * nCol;

	for(int i = 0; i < size; i++)
	{
		if(x_j < nRow)
		{
			*(px + i) = xSmall;
			x_j++;
		}
		else
		{
			xSmall+=step;
			x_j = 0;
			i--;
		}
	}

	for (int j = 0; j < size; j++)
	{
		if (y_j < nCol)
		{
			*(py + j) = ySmall;
			y_j++;
		}
		else
		{
			ySmall+=step;
			y_j = 0;
			j--;
		}		
	}
}

void CreateMatrixByRow(float* px, float* py, int xSmall, int xLarge, int ySmall, int yLarge,
					   int step)
{
	int nRow = 0;
	int nCol = 0;

	nRow     = (yLarge - ySmall)/step + 1;
	nCol     = (xLarge - xSmall)/step + 1;
	int size = nRow * nCol;

	for(int i = 0; i < nRow; i++)
	{
		for(int j = 0; j < nCol; j++)
		{
			*(px + i*nCol + j) = xSmall + j*step;
			*(py + i*nCol + j) = ySmall;
		}
		ySmall+=step;
	}
}

ShowView::ShowView(CWnd *pWnd)
:Xstart(0),Ystart(0),Xreal(2560),Yreal(2048),
showNum(8), singleWidth(0), height(0), DbClk(FALSE),nStretchMode(HALFTONE)
{
	bmpInfo = (BITMAPINFO*)new char[256*sizeof(RGBQUAD)+ sizeof(BITMAPINFOHEADER)];
	memset(bmpInfo,0,sizeof(BITMAPINFO)); 
	bmpInfo->bmiHeader.biPlanes = 1;    
	bmpInfo->bmiHeader.biWidth=2560;
	bmpInfo->bmiHeader.biHeight=2048;
	//bmpInfo->bmiHeader.biSizeImage=2560*2048*8;
	bmpInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);


	bmpInfo->bmiHeader.biCompression =BI_RGB;      
	bmpInfo->bmiHeader.biBitCount=8;
	bmpInfo->bmiHeader.biClrUsed=0;//为0就用2^biBitCount
	//bmpInfo->bmiHeader.biClrImportant=256;


	for( int j = 0; j < 256; j++ )
	{
		bmpInfo->bmiColors[j].rgbBlue =  (BYTE) (0xff & j);
		bmpInfo->bmiColors[j].rgbGreen = (BYTE)j;// bmpInfo->bmiColors[j].rgbBlue;
		bmpInfo->bmiColors[j].rgbRed =(BYTE)j;// bmpInfo->bmiColors[j].rgbBlue;
		bmpInfo->bmiColors[j].rgbReserved = 0;
	}

	this->pWnd = pWnd;

	for(int i = 0; i < MAX_CAMERAS; i++)
	{
		px[i]  = NULL;
		py[i]  = NULL;
		pue[i] = NULL;
		pve[i] = NULL;
		pu[i]  = NULL;
		pv[i]  = NULL;
		pc[i]  = NULL;
	}

	paras[0] = 32; paras[1] = 32; paras[2] = 1;paras[3] = 30;
	paras[4] = -10;paras[5] = 10;paras[6] = -10;paras[7] = 10;

	AlgorithmSign = 0;
	number++;
	TRACE(_T("++ShowView has %d \n"), number);

	hinst = LoadLibraryEx(_T("hs_piv64.dll") , NULL, LOAD_WITH_ALTERED_SEARCH_PATH);//第一个参数后缀可以不加...
	int result = GetLastError();
}

ShowView::~ShowView(void)
{
	delete this->bmpInfo;
	number--;
	TRACE(_T("--ShowView has %d \n"), number);

	for(int i = 0; i < MAX_CAMERAS; i++)
	{
		if(px[i]  != NULL){free(px[i]);  px[i]  = NULL;} 
		if(py[i]  != NULL){free(py[i]);  py[i]  = NULL;}
		if(pue[i] != NULL){free(pue[i]); pue[i] = NULL;}
		if(pve[i] != NULL){free(pve[i]); pve[i] = NULL;}
		if(pu[i]  != NULL){free(pu[i]);  pu[i]  = NULL;}
		if(pv[i]  != NULL){free(pv[i]);  pv[i]  = NULL;}
		if(pc[i]  != NULL){free(pc[i]);  pc[i]  = NULL;}

	}
	FreeLibrary(hinst);
}

BITMAPINFO* ShowView::getBmpInfo()
{
	return this->bmpInfo;
}
void ShowView::setWidth(long width)
{
	bmpInfo->bmiHeader.biWidth = width;
}

void ShowView::setHeight(long Height)
{
	bmpInfo->bmiHeader.biHeight = Height;
}

void ShowView::setStartPosition(int Xstart, int Ystart)
{
	this->Xstart = Xstart;
	this->Ystart = Ystart;
}

void ShowView::setStartPosition(AdjustImage* adjustImage)
{
	CRect rect;
	pWnd->GetClientRect(&rect);
	st_StartPosition startPosition = adjustImage->getStartPosition(rect);
	this->Xstart = startPosition.xStart;
	this->Ystart = startPosition.yStart;
}
void ShowView::setStartPosition(CheckToShow* checkShow, int sign)
{
	CRect rect;
	pWnd->GetClientRect(&rect);

	this->showNum     = checkShow->getNumBySign();              //相机数量
	this->singleWidth = rect.Width() / showNum;                 //单个相机在rect中的宽度 单位:像素
	this->height      = checkShow->getHeight(rect);             //单个相机在rect中的高度 单位:像素
	this->DbClk       = checkShow->getDoubleClk();
	this->startHeight = checkShow->getStartHeight(rect, !DbClk);//判断距顶部的像素距离
	int weight  = checkShow->ReturnWeight(sign);

	this->Xstart = weight * singleWidth;
	this->Ystart = startHeight;
}

int ShowView::getStartPositionX()
{
	return this->Xstart;
}

int ShowView::getStartPositionY()
{
	return this->Ystart;
}

void ShowView::setStretchMode(int nStretchMode)
{
	this->nStretchMode = nStretchMode;
}

int ShowView::getStretchMode()
{
	return this->nStretchMode;
}

void ShowView::LiveViewByPhysical(unsigned char* targetImage, int sign, AdjustImage* adjustImage)
{
	setStartPosition(adjustImage);

	int xRange = adjustImage->getXRange();
	int yRange = adjustImage->getYClientRange();  //这个范围是实际校正后的范围

	//设置bmpinfo
	setWidth(xRange);
	setHeight(yRange);

	LiveView_CWJ(targetImage, bmpInfo, this->Xstart, this->Ystart, 
		xRange, yRange, getStretchMode());
}

void ShowView::LiveViewBySign(unsigned char* targetImage, int sign, CheckToShow* checkShow)
{
	setStartPosition(checkShow, sign);
	LiveView_CWJ(targetImage, bmpInfo, this->Xstart, this->Ystart, 
		singleWidth, height, getStretchMode());
}
void ShowView::LiveViewWnd(CWnd* pWnd, CheckToShow* checkShow, int sign,
				 unsigned char* pImageBuff,BITMAPINFO *bmpinfo,int nStretchMode)
{
	CRect rect;
	pWnd->GetClientRect(&rect);

	int Weight      = checkShow->ReturnWeight(sign);
	int startHeight = checkShow->getStartHeight(rect);
	int SrcWidth    = checkShow->getWidth(rect);
	int SrcHeight   = checkShow->getHeight(rect, 2048 , 2560); // 这里不传2560，后面默认参数莫名其妙被改成了2056_Bug_20160517

	LiveView_CWJ(pImageBuff, bmpinfo, Weight*SrcWidth, startHeight, SrcWidth, SrcHeight, nStretchMode);
}
void ShowView::LiveViewWnd(st_LiveView *pLiveView)
{
	unsigned char* pImageBuffer = pLiveView->pImageBuffer;
	BITMAPINFO* bmpInfo = pLiveView->bmpInfo;
	int x = pLiveView->x;
	int y = pLiveView->y;
	int scaleX = pLiveView->scaleX;
	int scaleY = pLiveView->scaleY;
	int nStretchMode = pLiveView->nStretchMode;
	CWnd *m_pWnd = pLiveView->pWnd;

	CDC * pDC = m_pWnd->GetDC();
	CDC MemDC;

	if(!MemDC.CreateCompatibleDC(pDC))
	{
		m_pWnd->ReleaseDC(pDC);
		return;
	}

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,bmpInfo->bmiHeader.biWidth,bmpInfo->bmiHeader.biHeight);
	CBitmap* pOldBmp = MemDC.SelectObject(&bmp);
	::SetDIBitsToDevice(MemDC.GetSafeHdc(),
		0,0,bmpInfo->bmiHeader.biWidth,bmpInfo->bmiHeader.biHeight,
		0,0,
		0,(bmpInfo->bmiHeader.biHeight),
		pImageBuffer,
		bmpInfo,
		DIB_RGB_COLORS);

	pDC->SetStretchBltMode(COLORONCOLOR);//COLORONCOLOR   HALFTONE
	pDC->StretchBlt(x,y,scaleX,scaleY,&MemDC,0,bmpInfo->bmiHeader.biHeight,/*first*/
		bmpInfo->bmiHeader.biWidth,-(bmpInfo->bmiHeader.biHeight),/*second */ //两者都为0时和此时对比，图像上下翻转
		SRCCOPY);
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
	bmp.DeleteObject();
	pWnd->ReleaseDC(pDC);

}

void ShowView::LiveView_CWJ(unsigned char * pImageBuffer, BITMAPINFO* bmpInfo, int x, int y, 
							int scaleX, int scaleY, int nStretchMode)
{
	CDC * pDC = pWnd->GetDC();
	CDC MemDC;

	if(!MemDC.CreateCompatibleDC(pDC))
	{
		pWnd->ReleaseDC(pDC);
		return;
	}

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,bmpInfo->bmiHeader.biWidth,bmpInfo->bmiHeader.biHeight);
	CBitmap* pOldBmp = MemDC.SelectObject(&bmp);
	::SetDIBitsToDevice(MemDC.GetSafeHdc(),
		0,0,bmpInfo->bmiHeader.biWidth,bmpInfo->bmiHeader.biHeight,
		0,0,
		0,(bmpInfo->bmiHeader.biHeight),
		pImageBuffer,
		bmpInfo,
		DIB_RGB_COLORS);

	pDC->SetStretchBltMode(COLORONCOLOR);//COLORONCOLOR   HALFTONE
	pDC->StretchBlt(x,y,scaleX,scaleY,&MemDC,0,bmpInfo->bmiHeader.biHeight,/*first*/
		bmpInfo->bmiHeader.biWidth,-(bmpInfo->bmiHeader.biHeight),/*second */ //两者都为0时和此时对比，图像上下翻转
		SRCCOPY);
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
	bmp.DeleteObject();
	pWnd->ReleaseDC(pDC);
}


void ShowView::DrawText_k(st_DrawText *pDrawText)
{
	// Set the text rendering for Cleartype
	pDrawText->graph->SetTextRenderingHint(pDrawText->Mode);

	// Create a red Solid Brush Color(A,R,G,B)
	SolidBrush brush(Color(pDrawText->a,pDrawText->r,pDrawText->g,pDrawText->b));

	// Create a font
	FontFamily fontFamily(L"Arial");
	Font font(&fontFamily, 10, FontStyleRegular, UnitPoint);


	CClientDC clientDC(pDrawText->pWnd);
	CPoint point(pDrawText->X, pDrawText->Y);
	clientDC.LPtoDP(&point);
	PointF pointF(point.x,point.y);

	//g.TranslateTransform(0,font.GetHeight(0.0f));/af/平移坐标系  
	pDrawText->graph->DrawString(pDrawText->showStr, -1, &font, pointF, &brush);
}
void ShowView::Get_float(int size, int sign)
{
	if (px[sign] == NULL)
	{
		px[sign] = (float*)malloc(size);
		//px = new float[size];   //不知为何new float的size 难道还要加sizeof（float）？？
		memset(px[sign], 0, size);
	}
	if (py[sign] == NULL)
	{
		py[sign] = (float*)malloc(size);
		//py = new float[size];
		memset(py[sign], 0, size);
	}
	if (pue[sign] == NULL)
	{
		pue[sign] = (float*)malloc(size);
		//pue = new float[size];
		memset(pue[sign], 0, size);
	}
	if (pve[sign] == NULL)
	{
		pve[sign] = (float*)malloc(size);
		//pve = new float[size];
		memset(pve[sign], 0, size);
	}
	if (pu[sign] == NULL)
	{
		pu[sign] = (float*)malloc(size);
		//pu = new float[size];
		memset(pu[sign], 0, size);
	}
	if (pv[sign] == NULL)
	{
		pv[sign] = (float*)malloc(size);
		//pv = new float[size];
		memset(pv[sign], 0, size);
	}
	if (pc[sign] == NULL)
	{
		pc[sign] = (float*)malloc(size);
		//pc = new float[size];
		memset(pc[sign], 0, size);
	}

}

void ShowView::setChooseAlgorithm(int AlgorithmSign)
{

	this->AlgorithmSign = AlgorithmSign;
}

void ShowView::setCalcRange(int* paras, int xSmall, int xLarge, int ySmall, int yLarge, int step)
{
	this->xSmall = xSmall;
	this->xLarge = xLarge;
	this->ySmall = ySmall;
	this->yLarge = yLarge;
	this->step   = step;

	/*ASSERT(paras == NULL);*/
	for (int i = 0; i < 8; i++)
	{
		this->paras[i] = paras[i];
	}
}

void ShowView::GenerateVectorNum(unsigned char* pBuffFirst, unsigned char* pBuffSecond, int nBuffRow, int nBuffCol, int sign)
{
	//若没指定绝对路径，则关联动态库放在.h .cpp 位置
// 	HINSTANCE hinst;
// 	hinst = LoadLibraryEx(_T("hs_piv64.dll") , NULL, LOAD_WITH_ALTERED_SEARCH_PATH);//第一个参数后缀可以不加...
// 	int result = GetLastError();

	Load_PIV_Images_8bit_Proc Load_PIV_Images_8bit = (Load_PIV_Images_8bit_Proc)GetProcAddress(hinst, "Load_PIV_Images_8bit");
	Unload_PIV_Images_Proc    Unload_PIV_Images    = (Unload_PIV_Images_Proc)GetProcAddress(hinst, "Unload_PIV_Images");
	HSPIV_MQD_MP_Proc         HSPIV_MQD_MP         = (HSPIV_MQD_MP_Proc)GetProcAddress(hinst, "HSPIV_MQD_MP");
	HSPIV_Cross_Correlation_MP_Proc HSPIV_Cross_Correlation_MP  = (HSPIV_Cross_Correlation_MP_Proc)GetProcAddress(hinst, "HSPIV_Cross_Correlation_MP");
	//HSPIV_MQD_Proc            HSPIV_MQD            = (HSPIV_MQD_Proc)GetProcAddress(hinst,"HSPIV_MQD");

	yLarge   = nBuffRow;
	ySmall   = 0;
	xLarge   = nBuffCol;
	xSmall   = 0;

	int nRow = (yLarge - ySmall) / step + 1;
	int nCol = (xLarge - xSmall) / step + 1;

	this->mSizeX[sign] = nRow;
	this->mSizeY[sign] = nCol;

	unsigned char* copy_pBuffFirst  = Matrix_T(pBuffFirst, nBuffRow, nBuffCol);
	unsigned char* copy_pBuffSecond = Matrix_T(pBuffSecond, nBuffRow, nBuffCol);

	Load_PIV_Images_8bit(copy_pBuffFirst, copy_pBuffSecond, nBuffRow, nBuffCol);
	Get_float(nRow * nCol * sizeof(float), sign);
	CreateMatrixByRow(px[sign], py[sign], xSmall, xLarge, ySmall, yLarge, step);

	if(AlgorithmSign == 0)
	{
		HSPIV_MQD_MP(8, px[sign], py[sign], nRow, nCol, pue[sign], pve[sign], paras, pu[sign], pv[sign], pc[sign]);
	}
	else if(AlgorithmSign == 1)
	{
		HSPIV_Cross_Correlation_MP(8, px[sign], py[sign], nRow, nCol, pue[sign], pve[sign], paras, pu[sign], pv[sign], pc[sign]);
	}
	Unload_PIV_Images();
	/*FreeLibrary(hinst);*/

	free(copy_pBuffFirst);
	copy_pBuffFirst = NULL;
	free(copy_pBuffSecond);
	copy_pBuffSecond = NULL;
}

void ShowView::setXRealPixel(int Xreal)
{
	this->Xreal = Xreal;
}

void ShowView::setYRealPixel(int Yreal)
{
	this->Yreal = Yreal;
}



void ShowView::DrawArrowPoisitionBySign(float* px, float* py, float* u, float* v, 
				int sizeX, int sizeY, unsigned int sign, CheckToShow* checkShow)
{
	float xI     = 0;
	float yI     = 0;

	float xStart = 0; //矢量箭头像素坐标
	float yStart = 0;
	float xEnd   = 0;
	float yEnd   = 0;

	CPoint startPoint;
	CPoint endPoint;
	CRect rect;
	pWnd->GetClientRect(&rect);

	int showNum = checkShow->getNumBySign();                  //相机数量
	int singleWidth = rect.Width() / showNum;                 //单个相机在rect中的宽度 单位:像素
	int weight = checkShow->ReturnWeight(sign);               //权重...
	int height = checkShow->getHeight(rect);                  //单个相机在rect中的高度 单位:像素
	BOOL DbClk = checkShow->getDoubleClk();
	int startHeight = checkShow->getStartHeight(rect, !DbClk);//判断距顶部的像素距离
	for (int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			if(u[i*sizeY + j] == 0 && v[i*sizeY + j] == 0)
			{
				continue;
			}
			xI = px[i*sizeY + j] / this->Xreal;
			yI = py[i*sizeY + j] / this->Yreal;
			xStart = (weight + xI) * singleWidth;
			yStart = startHeight + yI*height;
		
			startPoint.x = (int)xStart;
			startPoint.y = (int)yStart;
			//startPoint = (xStart, yStart);

			xEnd   = xStart + /*8 / showNum * */u[i*sizeY + j];
			yEnd   = yStart + /*8 / showNum * */v[i*sizeY + j];
			endPoint.x = (int)xEnd;
			endPoint.y = (int)yEnd;
			//endPoint = (xEnd, yEnd);
			DrawVectorArrow(startPoint, endPoint, this->pWnd);
		}
	}
}
void ShowView::DrawVectorArrow(POINT startPoint, POINT endPoint, CWnd* pWnd)
{
	if(pWnd == NULL)
	{
		pWnd = this->pWnd;
	}
	CClientDC dc(pWnd);
	CPen pNewPen;
	CPen* pOldPen;
	pNewPen.CreatePen(PS_SOLID, 1, RGB(138, 43, 226));
	pOldPen = dc.SelectObject(&pNewPen);
	dc.MoveTo(startPoint);
	dc.LineTo(endPoint);

	double PI = 3.1415926;
	double t=PI/4; //箭头与直线夹角
	double l=0.4; //箭头边长度占直线长度的百分比

	POINT arrowPoint;

	arrowPoint.x = (int)(l*((startPoint.x-endPoint.x)*cos(t)-(startPoint.y-endPoint.y)*sin(t))+endPoint.x);
	arrowPoint.y = (int)(l*((startPoint.x-endPoint.x)*sin(t)+(startPoint.y-endPoint.y)*cos(t))+endPoint.y);
	dc.MoveTo(endPoint);
	dc.LineTo(arrowPoint);

	arrowPoint.x = (int)(l*((startPoint.x-endPoint.x)*cos(-t)-(startPoint.y-endPoint.y)*sin(-t))+endPoint.x);
	arrowPoint.y = (int)(l*((startPoint.x-endPoint.x)*sin(-t)+(startPoint.y-endPoint.y)*cos(-t))+endPoint.y);
	dc.MoveTo(endPoint);
	dc.LineTo(arrowPoint);

	dc.SelectObject(&pOldPen);
	::DeleteObject(pNewPen);
}