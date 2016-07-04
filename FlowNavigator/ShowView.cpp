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
:Xstart(0),Ystart(0),Xreal(2048),Yreal(2560),
showNum(8), singleWidth(0), height(0), DbClk(FALSE),nStretchMode(HALFTONE),
base(BASE_ONWIDTH),I(1)
{
	initBmpInfo();
	initial();

	this->pWnd = pWnd;
}

ShowView::ShowView(CWnd* pWnd, HINSTANCE hinst)
:Xstart(0),Ystart(0),Xreal(2048),Yreal(2560),
showNum(8), singleWidth(0), height(0), DbClk(FALSE),nStretchMode(HALFTONE),
base(BASE_ONWIDTH),I(1)
{
	initBmpInfo();
	initial();
	this->pWnd = pWnd;

	this->hinst = hinst;
}

void ShowView::initial()
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

	for(int i = 0; i < MAX_CAMERAS; i++)
	{
		px[i]  = NULL;
		py[i]  = NULL;
		pue[i] = NULL;
		pve[i] = NULL;
		pu[i]  = NULL;
		pv[i]  = NULL;
		pc[i]  = NULL;
		uMax[i] = 0;
		vMax[i] = 0;
		MaxArrowLen[i] = 0;
	}

	paras[0] = 32; paras[1] = 32; paras[2] = 1;paras[3] = 30;
	paras[4] = -10;paras[5] = 10;paras[6] = -10;paras[7] = 10;

	AlgorithmSign = 0;
	number++;
	TRACE(_T("++ShowView has %d \n"), number);

	memset(&st_base, 0, sizeof(st_base));
}

ShowView::~ShowView(void)
{
	delete this->bmpInfo;
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		delete this->pBmpInfo[i];
	}

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
}

void ShowView::initBmpInfo()
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		pBmpInfo[i] = (BITMAPINFO*)new char[256*sizeof(RGBQUAD)+ sizeof(BITMAPINFOHEADER)];
		memset(pBmpInfo[i],0,sizeof(BITMAPINFO)); 
		pBmpInfo[i]->bmiHeader.biPlanes = 1;    
		pBmpInfo[i]->bmiHeader.biWidth=2560;
		pBmpInfo[i]->bmiHeader.biHeight=2048;
		pBmpInfo[i]->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);


		pBmpInfo[i]->bmiHeader.biCompression =BI_RGB;      
		pBmpInfo[i]->bmiHeader.biBitCount=8;
		pBmpInfo[i]->bmiHeader.biClrUsed=0;//为0就用2^biBitCount

		for( int j = 0; j < 256; j++ )
		{
			pBmpInfo[i]->bmiColors[j].rgbBlue =  (BYTE) (0xff & j);
			pBmpInfo[i]->bmiColors[j].rgbGreen = (BYTE)j;// bmpInfo->bmiColors[j].rgbBlue;
			pBmpInfo[i]->bmiColors[j].rgbRed =(BYTE)j;// bmpInfo->bmiColors[j].rgbBlue;
			pBmpInfo[i]->bmiColors[j].rgbReserved = 0;
		}

	}
}

BITMAPINFO* ShowView::getBmpInfo()
{
	return this->bmpInfo;
}

pBitmapInfo ShowView::getBmpInfo(int sign)
{
	ASSERT(sign < MAX_CAMERAS);
	return this->pBmpInfo[sign];
}

void ShowView::setWidth(long width)
{
	bmpInfo->bmiHeader.biWidth = width;
}

void ShowView::setWidth(long width, int sign)
{
	this->pBmpInfo[sign]->bmiHeader.biWidth = width;
}

void ShowView::setHeight(long Height)
{
	bmpInfo->bmiHeader.biHeight = Height;
}

void ShowView::setHeight(long Height, int sign)
{
	this->pBmpInfo[sign]->bmiHeader.biHeight = Height;
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
	st_StartPosition startPosition = adjustImage->getStartPosition(st_base);
	int diff = adjustImage->getLeftSplitLinePixel() * st_base.baseWidth / adjustImage->getXClientRange();
	this->Xstart = startPosition.xStart + diff;
	this->Ystart = /*0*/startPosition.yStart; //显示位置_20160605
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
	int weight        = checkShow->ReturnWeight(sign);

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

void ShowView::LiveViewByPhysical(unsigned char* targetImage, int sign, AdjustImage* adjustImage, int originSign)
{
	setStartPosition(adjustImage);

	int xRange = adjustImage->getXRange();
	int yRange = adjustImage->getYClientRange();  //这个范围是实际校正后的范围

	//设置bmpinfo
	setWidth(xRange, sign);
	setHeight(yRange, sign);

	int nWidth  = (adjustImage->getSplitLinePixel() - adjustImage->getLeftSplitLinePixel()) * (float)st_base.baseWidth / adjustImage->getXClientRange() /*+ 0.5*/;
	int nHeight = st_base.baseHeight;

	int xSrc    = adjustImage->getLeftSplitLinePixel();
	int ySrc    = pBmpInfo[sign]->bmiHeader.biHeight;
	int nSrcWidth  = adjustImage->getSplitLinePixel() - adjustImage->getLeftSplitLinePixel();
	int nSrcHeight = pBmpInfo[sign]->bmiHeader.biHeight;
/*
	//如果选中的只有一台相机，下面的判断 还得增加对FIRSTCHECK时nSrcWidth的情况 最好是把adjustImage->getSplitLinePixel() 抽象到AdjustImage里面用函数判断
	if(originSign == FIRSTCHECK)
	{
		xSrc = 0;
		nSrcWidth  = adjustImage->getSplitLinePixel();
	}
	else if(originSign == LASTCHECK)
	{
		nSrcWidth = adjustImage->getXRange() - adjustImage->getLeftSplitLinePixel();
	}
*/
	LiveView_CWJ(targetImage, pBmpInfo[sign], this->Xstart, this->Ystart, 
		nWidth, nHeight, xSrc, ySrc, nSrcWidth, nSrcHeight,getStretchMode());
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

void ShowView::LiveView_CWJ(unsigned char * pImageBuffer, BITMAPINFO* bmpInfo, int x, int y, 
	int scaleX, int scaleY, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, int nStretchMode)
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
	pDC->StretchBlt(x,y,scaleX,scaleY,&MemDC,xSrc,ySrc,/*first*/
		nSrcWidth,-nSrcHeight,/*second */ //两者都为0时和此时对比，图像上下翻转
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

	int result = GetLastError();

	if(result != 0)
	{
		CString txt;
		txt.Format(_T("load func GetLastError=%d"), result);
		AfxMessageBox(txt);
	}


	this->yLarge   = nBuffRow;
	this->ySmall   = 0;
	this->xLarge   = nBuffCol;
	this->xSmall   = 0;

	int nRow = (this->yLarge - this->ySmall) / step + 1;
	int nCol = (this->xLarge - this->xSmall) / step + 1;

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
	FindMaxArrowLen(sign);
#ifdef MYTEST_MAX_U_V
	FindMaxUV(sign);
#endif

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

			xEnd   = xStart + u[i*sizeY + j]  * ARROW_LEN / MaxArrowLen[sign];
			yEnd   = yStart - v[i*sizeY + j]  * ARROW_LEN / MaxArrowLen[sign];
			endPoint.x = (int)xEnd;
			endPoint.y = (int)yEnd;
			//endPoint = (xEnd, yEnd);
			DrawVectorArrow(startPoint, endPoint, this->pWnd);
		}
	}
}

void ShowView::FindMaxUV(int sign)
{
	uMax[sign] = *(pu[sign]);
	vMax[sign] = *(pv[sign]);
	for(int i = 1; i < (mSizeY[sign]*mSizeX[sign]); i++)
	{
		if(uMax[sign] < *(pu[sign] + i))
		{
			uMax[sign] = *(pu[sign] + i);
		}
		if (vMax[sign] < *(pv[sign] + i))
		{
			vMax[sign] = *(pv[sign] + i);
		}
	}
	TRACE(_T("uMax[%d] = %f, vMax[%d] = %f\n"), sign, uMax[sign],sign, vMax[sign]);
}

void ShowView::FindMaxArrowLen(int sign)
{
	float len = 0;
	for (int i = 0; i < (mSizeX[sign] * mSizeY[sign]); i++)
	{
		len = sqrt((*(pu[sign] + i)) * (*(pu[sign] + i))+ (*(pv[sign] + i)) * (*(pv[sign] + i)));
		if(MaxArrowLen[sign] < len)
		{
			MaxArrowLen[sign] = len;
		}
	}

	TRACE(_T("MaxArrowLen[%d] = %f\n"),sign, MaxArrowLen[sign]);
}

void ShowView::DrawArrowPoisitionBySign(AdjustImage* adjustImage, int sign, int originSign)
{
	int sizeX = this->mSizeX[sign];
	int sizeY = this->mSizeY[sign];

	int startX = 0;
	while(startX * step < adjustImage->getLeftSplitLinePixel())
	{
		startX++;
	}
	//startX--;

	int endX = 0;
	while(endX * step < adjustImage->getSplitLinePixel())
	{
		endX++;
	}
	endX--;

	TRACE(_T("DrawArrowPoisitionBySign: startX = %d, endX = %d\n"), startX, endX);
	/*
	if(originSign == FIRSTCHECK)
	{
		startX = 0;
	}
	else if(originSign == LASTCHECK)
	{
		endX = sizeY;
	}
	*/
	CPoint startPoint;
	CPoint endPoint;

	CRect rect;
	pWnd->GetClientRect(&rect);

	//getCurrentLen(rect);             
	st_StartPosition start= adjustImage->getStartPosition(st_base);// 拼成同高度时，原始图像在实际窗口中的起始位置
	
	for (int i = 0; i < sizeX; i++)
	{
		for(int j = startX; j < endX; j++)
		{
			startPoint.x = (*(px[sign]+i*sizeY + j) * st_base.baseWidth  / adjustImage->getXClientRange() +
				start.xStart + 0.5);
			startPoint.y = (*(py[sign]+i*sizeY + j) * st_base.baseHeight / adjustImage->getYClientRange() + 
				start.yStart + 0.5); 
			endPoint.x   = (startPoint.x + *(pu[sign] + i*sizeY + j) * ARROW_LEN / MaxArrowLen[sign] + 0.5);
			endPoint.y   = (startPoint.y - *(pv[sign] + i*sizeY + j) * ARROW_LEN / MaxArrowLen[sign] + 0.5);
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

	dc.SelectObject(pOldPen);
	::DeleteObject(pNewPen);
}

void ShowView::setCurrentBase(int base)
{
	this->base = base;
}

int ShowView::getCurrentBase()
{
	return this->base;
}

void ShowView::setSacle(float I)
{
	this->I = I;
}
st_Base ShowView::getCurrentLen(CRect rect)
{
	if (this->base == BASE_ONWIDTH)
	{
		st_base.baseWidth  = rect.Width();
		st_base.baseHeight = (int)(st_base.baseWidth * I);

		st_base.diffWidth  = 0;
		st_base.diffHeight = (rect.Height() - st_base.baseHeight) / 2;  //代表将第一步同高度的图放窗口中间了
	} 
	else if(this->base == BASE_ONHEIGHT)
	{
		st_base.baseHeight = rect.Height();
		st_base.baseWidth  = (int)(st_base.baseHeight / I);

		st_base.diffHeight = 0;
		st_base.diffWidth = (rect.Width() - st_base.baseWidth) / 2;
	}
	else if(this->base == BASE_SAME)
	{
		st_base.baseWidth  = rect.Width();
		st_base.baseHeight = rect.Height();

		st_base.diffWidth  = 0;
		st_base.diffHeight = 0;
	}
	else
	{
		ASSERT(1);
	}

	return st_base;
}

st_FlowRate ShowView::getFlowRate(CPoint point, int* CheckCamSign, int Num, AdjustImage* adjustImage[])
{
	st_FlowRate flowRate       = {0};

	if(point.x < st_base.diffWidth || point.x > (st_base.baseWidth + st_base.diffWidth) ||
		point.y < st_base.diffHeight || point.y > (st_base.diffHeight + st_base.baseHeight))
	{
		flowRate.u = 0;
		flowRate.v = 0;
		flowRate.uv = 0;

		return flowRate;
	}

	st_Range range             = {0};
	st_ClientRange clientRange = {0};
	float splitLine            =  0;
	int sign                   =  0;  //所选中的编号


	for(int i = 0; i < Num; i++)
	{
		//clientRange其实都一样
		clientRange = adjustImage[CheckCamSign[i]]->getClientRange();                                                                                 //(2)

		splitLine = (adjustImage[CheckCamSign[i]]->getSplitLine() - clientRange.xMin) * st_base.baseWidth / (clientRange.xMax - clientRange.xMin);    //(2)

		if((point.x - st_base.diffWidth) <= splitLine)
		{
			sign = i;
			break;
		}
	}
	range = adjustImage[CheckCamSign[sign]]->getSingleRange();
/*
	range = adjustImage[CheckCamSign[sign]]->getSingleRange();
	clientRange = adjustImage[CheckCamSign[sign]]->getClientRange();                                                                      //(1)
	splitLine = (adjustImage[CheckCamSign[sign]]->getSplitLine() - clientRange.xMin) * st_base.baseWidth / (clientRange.xMax - clientRange.xMin);  //(1)

	for(int i = 0; i < Num; i++)
	{
		if((point.x - st_base.diffWidth) <= splitLine)
		{
			sign = i;
			break;
		}
		else
		{
			//clientRange其实都一样
			clientRange = adjustImage[CheckCamSign[i]]->getClientRange();                                                                                 //(2)
			splitLine = (adjustImage[CheckCamSign[i]]->getSplitLine() - clientRange.xMin) * st_base.baseWidth / (clientRange.xMax - clientRange.xMin);    //(2)
		}
	}

	clientRange = adjustImage[CheckCamSign[sign]]->getClientRange();                                                                                     //(3)
	splitLine = (adjustImage[CheckCamSign[sign]]->getSplitLine() - clientRange.xMin) * st_base.baseWidth / (clientRange.xMax - clientRange.xMin);        //(3)
*/
	float pointX       = 0;
	int pointXByAdjust = 0;
	int pointYByAdjust = 0;
	st_StartPosition startPosition = adjustImage[CheckCamSign[sign]]->getStartPosition(st_base);
// 	float leftSplit = adjustImage[CheckCamSign[sign]]->getLeftSplitLine();
// 
// 	pointX = point.x - startPosition.xStart - (leftSplit - range.xMin) * st_base.baseWidth/ 
// 		(clientRange.xMax - clientRange.xMin);

	pointX = point.x - startPosition.xStart;
	pointXByAdjust = (int)(pointX * adjustImage[CheckCamSign[sign]]->getXClientRange() / st_base.baseWidth + 0.5);
	pointYByAdjust = (int)((point.y - startPosition.yStart) * 
		adjustImage[CheckCamSign[sign]]->getYClientRange() / st_base.baseHeight + 0.5);
	int startX = 0;
	while(startX * step <= pointXByAdjust)
	{
		startX++;
	}
	startX--;
/*
	//不在有效区域内
	st_StartPosition startPosition = adjustImage[CheckCamSign[sign]]->getStartPosition(st_base);
	int nHeight = (range.yMax - range.yMin) * st_base.baseHeight / (clientRange.yMax - clientRange.yMin);
	if (pointYByAdjust < (startPosition.yStart - st_base.diffHeight) || pointYByAdjust > (startPosition.yStart + nHeight - st_base.diffHeight))
	{
		flowRate.u = 0;
		flowRate.v = 0;
		flowRate.uv = 0;

		return flowRate;
	}
*/
	int startY = 0;
	while(startY * step <= pointYByAdjust)
	{
		startY++;
	}
	startY--;

	flowRate.u  = *(pu[sign] + startY * mSizeY[sign] + startX);
	flowRate.v  = *(pv[sign] + startY * mSizeY[sign] + startX);
	flowRate.uv = sqrt(flowRate.u*flowRate.u + flowRate.v*flowRate.v);

	TRACE(_T("u=%f, v=%f, uv=%f, maxUVLen=%f, firstU=%f, firstV=%f, startX=%d, startY=%d\n"), flowRate.u, flowRate.v, flowRate.uv,MaxArrowLen[CheckCamSign[sign]], 
		*(pu[sign] + 1), *(pv[sign] + 1), startX, startY);

	return flowRate;
}