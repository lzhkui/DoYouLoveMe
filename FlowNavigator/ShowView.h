#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"

/*显示函数传入结构体*/
struct LiveView
{
	unsigned char * pImageBuffer; //像素矩阵
	BITMAPINFO* bmpInfo;          //调色板
	int x;                        //显示区域起始位置x坐标
	int y;                        //显示区域起始位置y坐标
	int scaleX;                   //需要显示的宽
	int scaleY;                   //需要显示的高
	int nStretchMode;             //COLORONCOLOR   HALFTONE
	CWnd * pWnd;
};
typedef struct LiveView st_LiveView;
typedef struct LiveView* pst_LiveView;

/*绘制字体函数传入结构体*/
struct DrawText
{
	Graphics graph;
	TextRenderingHint Mode;
	BYTE a;
	BYTE r;
	BYTE g;
	BYTE b;
	CWnd* pWnd;
	int X;
	int Y;
	CString showStr;
};
typedef struct DrawText st_DrawText;
typedef struct DrawText pst_drawText;

class ShowView
{
public:
	ShowView(CWnd *pWnd);
	~ShowView(void);

	void setWidth(long width);
    void setHeight(long Height);

	/*可以重新指定CWnd 中显示*/
	void LiveViewWnd(st_LiveView *pLiveView);

	/*在类构造函数传入的CWnd中显示*/
	void LiveView_CWJ(unsigned char * pImageBuffer, BITMAPINFO* bmpInfo, //调色板
		int x, int y,                                                  //显示区域起始位置(左上角)
		int scaleX, int scaleY,                                        //需要显示的宽高
		int nStretchMode);                                             //COLORONCOLOR   HALFTONE
	void DrawText_k(st_DrawText *pDrawText);

	BITMAPINFO* getBmpInfo();

private:
	BITMAPINFO *bmpInfo;
	CWnd *pWnd;
};
