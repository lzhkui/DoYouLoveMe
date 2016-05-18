#pragma once

class ZoomImage
{
public:
	ZoomImage(CWnd* pWnd);
	~ZoomImage(void);

	void setHdc(CWnd* pwnd);
	HDC getHdc();
	void releaseHDC(CWnd* pWnd);

	void setPointStart(CPoint PointStart);
	void setPointEnd(CPoint PointEnd);

	void DrawRectangle(CPoint PointStart, CPoint PointEnd);
	void DrawRectangle(HDC hdc, CPoint PointStart, CPoint PointEnd);
	void RegularTwoPoint(CPoint& PointStart, CPoint& PointEnd);

private:
	CWnd* m_pWnd;
	HDC hdc;
	CPoint pointStart;
	CPoint pointEnd;
};
