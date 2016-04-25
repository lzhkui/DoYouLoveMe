#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"

/*��ʾ��������ṹ��*/
struct LiveView
{
	unsigned char * pImageBuffer; //���ؾ���
	BITMAPINFO* bmpInfo;          //��ɫ��
	int x;                        //��ʾ������ʼλ��x����
	int y;                        //��ʾ������ʼλ��y����
	int scaleX;                   //��Ҫ��ʾ�Ŀ�
	int scaleY;                   //��Ҫ��ʾ�ĸ�
	int nStretchMode;             //COLORONCOLOR   HALFTONE
	CWnd * pWnd;
};
typedef struct LiveView st_LiveView;
typedef struct LiveView* pst_LiveView;

/*�������庯������ṹ��*/
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

	/*��������ָ��CWnd ����ʾ*/
	void LiveViewWnd(st_LiveView *pLiveView);

	/*���๹�캯�������CWnd����ʾ*/
	void LiveView_CWJ(unsigned char * pImageBuffer, BITMAPINFO* bmpInfo, //��ɫ��
		int x, int y,                                                  //��ʾ������ʼλ��(���Ͻ�)
		int scaleX, int scaleY,                                        //��Ҫ��ʾ�Ŀ��
		int nStretchMode);                                             //COLORONCOLOR   HALFTONE
	void DrawText_k(st_DrawText *pDrawText);

	BITMAPINFO* getBmpInfo();

private:
	BITMAPINFO *bmpInfo;
	CWnd *pWnd;
};
