#include "StdAfx.h"
#include "ZoomImage.h"

ZoomImage::ZoomImage(CWnd* pWnd)
{
	this->m_pWnd = pWnd;
	this->hdc  = pWnd->GetDC()->GetSafeHdc();
	pointStart.x = pointStart.y = 0;
	pointEnd.x   = pointEnd.y   = 0;
}

ZoomImage::~ZoomImage(void)
{
	releaseHDC(this->m_pWnd);
}


void ZoomImage::setHdc(CWnd* pWnd)
{
	if (pWnd == NULL)
	{
		return;
	}
	this->hdc = pWnd->GetDC()->GetSafeHdc();
}

HDC ZoomImage::getHdc()
{
	return this->hdc;
}

void ZoomImage::releaseHDC(CWnd* pWnd)
{
	ReleaseDC(pWnd->GetSafeHwnd(), this->hdc);
}

void ZoomImage::setPointStart(CPoint PointStart)
{
	this->pointStart = PointStart;
}

void ZoomImage::setPointEnd(CPoint PointEnd)
{
	this->pointEnd = PointEnd;
}
void ZoomImage::DrawRectangle(CPoint PointStart, CPoint PointEnd)
{
	DrawRectangle(this->hdc, PointStart, PointEnd);
}
void ZoomImage::DrawRectangle(HDC hdc, CPoint PointStart, CPoint PointEnd)
{
	SetROP2(hdc,R2_NOTMASKPEN);   

	HPEN hpen;   
	HGDIOBJ hOldPen;
	hpen=CreatePen(PS_SOLID,2,RGB(255,0,0));   
	hOldPen = SelectObject(hdc,hpen);   
	SelectObject(hdc,GetStockObject(NULL_BRUSH));   

	//::Rectangle(hdc,this->PointStart.x,this->PointStart.y,this->PointEnd.x,this->PointEnd.y);    
	//PointEnd = point;
	//::GetCursorPos(&PointEnd);   
	::Rectangle(hdc,PointStart.x,PointStart.y,PointEnd.x,PointEnd.y);  
	SelectObject(hdc, hOldPen);
	::DeleteObject(hpen);
}

void ZoomImage::RegularTwoPoint(CPoint& PointStart, CPoint& PointEnd)
{
	long Temp = -1;

	if(PointStart.x > PointEnd.x)
	{
		Temp         = PointStart.x;
		PointStart.x = PointEnd.x;
		PointEnd.x   = Temp;
	}

	if(PointStart.y > PointEnd.y)
	{
		Temp         = PointStart.y;
	    PointStart.y = PointEnd.y;
		PointEnd.y   = Temp;
	}

	setPointStart(PointStart);
	setPointEnd(PointEnd);
}