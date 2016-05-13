#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"
#include "CheckToShow.h"


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
	Graphics* graph;
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

static int number = 0; //统计类有多少实例

typedef int (__cdecl *Load_PIV_Images_8bit_Proc)(unsigned char *, unsigned char *, int, int);
typedef int (__cdecl *HSPIV_MQD_MP_Proc)(int, float *, float *, int, int, 
										 float *, float *, int *, 
										 float *, float *, float *);
typedef int (__cdecl *HSPIV_MQD_Proc)(float *, float *, int, int, 
									  float *, float *, int *, 
									  float *, float *, float *);
typedef int (__cdecl *Unload_PIV_Images_Proc)();
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
	void setStartPosition(int Xstart, int Ystart);
	int getStartPositionX();
	int getStartPositionY();

	void DrawArrowPoisitionBySign(float* px, float* py, float* u, float* v, 
		int sizeX, int sizeY, unsigned int sign, CheckToShow* checkShow);
	void DrawVectorArrow(POINT startPoint, POINT endPoint, CWnd* pWnd = NULL);
	void GenerateVectorNum(unsigned char* pBuffFirst, unsigned char* pBuffSecond, int nBuffRow, int nBuffCol);

	void Get_float(int size);
	void setXRealPixel(int Xreal);
	void setYRealPixel(int Yreal);
private:
	BITMAPINFO *bmpInfo;
	CWnd *pWnd;
	int Xstart;
	int Ystart;
	int Xreal;  //x方向实际像素长度
	int Yreal;  //y方向实际像素长度

public:
	float* px;
	float* py;
	float* pue;
	float* pve;
	int paras[8];
	float* pu;
	float* pv;
	float* pc;
	int    mSizeX; //上述指针变量的"行" 
	int    mSizeY; //上述指针变量的"列"
};
extern void CreateMatrixByCol(float* px, float* py, int xSmall, int xLarge, int ySmall, int yLarge,
							  int step);
extern void CreateMatrixByRow(float* px, float* py, int xSmall, int xLarge, int ySmall, int yLarge,
							 int step);