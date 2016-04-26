#include "StdAfx.h"
#include "ShowView.h"

ShowView::ShowView(CWnd *pWnd)
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
}

ShowView::~ShowView(void)
{
	delete this->bmpInfo;
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