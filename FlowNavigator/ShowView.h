#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"
#include "CheckToShow.h"


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

static int number = 0; //ͳ�����ж���ʵ��

typedef int (__cdecl *Load_PIV_Images_8bit_Proc)(unsigned char *, unsigned char *, int, int);
typedef int (__cdecl *HSPIV_MQD_MP_Proc)(int, float *, float *, int, int, 
										 float *, float *, int *, 
										 float *, float *, float *);
typedef int (__cdecl *HSPIV_MQD_Proc)(float *, float *, int, int, 
									  float *, float *, int *, 
									  float *, float *, float *);
typedef int (__cdecl *HSPIV_Cross_Correlation_MP_Proc)(int, float *, float *, int, int, 
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

	//����signչʾͼƬλ�ã���Ҫ�ȵ���setStartPosition
	void LiveViewBySign(unsigned char* targetImage, int sign, CheckToShow* checkShow);

	void LiveViewWnd(CWnd* pWnd, CheckToShow* checkShow, int sign,
		unsigned char* pImageBuff,BITMAPINFO *bmpinfo,int nStretchMode);

	/*��������ָ��CWnd ����ʾ*/
	void LiveViewWnd(st_LiveView *pLiveView);

	/*���๹�캯�������CWnd����ʾ*/
	void LiveView_CWJ(unsigned char * pImageBuffer, BITMAPINFO* bmpInfo, //��ɫ��
		int x, int y,                                                    //��ʾ������ʼλ��(���Ͻ�)
		int scaleX, int scaleY,                                          //��Ҫ��ʾ�Ŀ��
		int nStretchMode);                                               //COLORONCOLOR   HALFTONE
	
	void DrawText_k(st_DrawText *pDrawText);

	BITMAPINFO* getBmpInfo();
	void setStartPosition(int Xstart, int Ystart);
	void setStartPosition(CheckToShow* checkShow, int sign);
	int  getStartPositionX();
	int  getStartPositionY();

	void DrawArrowPoisitionBySign(float* px, float* py, float* u, float* v, 
		int sizeX, int sizeY, unsigned int sign, CheckToShow* checkShow);
	void DrawVectorArrow(POINT startPoint, POINT endPoint, CWnd* pWnd = NULL);
	void GenerateVectorNum(unsigned char* pBuffFirst, unsigned char* pBuffSecond, int nBuffRow, int nBuffCol);

	void Get_float(int size);
	void setXRealPixel(int Xreal);
	void setYRealPixel(int Yreal);
	void setStretchMode(int nStretchMode);
	int  getStretchMode();

	void setCalcRange(int* paras, int xSmall = 0, int xLarge = 2560, int ySmall = 0, int yLarge = 2048, int step = 30);
	void setChooseAlgorithm(int AlgorithmSign); // 0: HSPIV_MQD_MP  1: HSPIV_Cross_Correlation_MP

private:
	BITMAPINFO *bmpInfo;
	CWnd *pWnd;
	int Xstart;
	int Ystart;
	int Xreal;        //x����ʵ�����س���
	int Yreal;        //y����ʵ�����س���

	int AlgorithmSign;
private:
	int showNum;      //��double_in����Ϊ8�� double_outʱΪѡ�и���
	int singleWidth;  //����showNum�õ��ĵ�����ʾ������
	int height;       //������ʾ����߶�
	BOOL DbClk;       //double_inʱ: false   double_outʱ��true
	int startHeight;  //��Կͻ������˵ĸ߶�
	int nStretchMode; //��ʾģʽ
public:
	float* px;
	float* py;
	float* pue;
	float* pve;
	int paras[8];
	float* pu;
	float* pv;
	float* pc;
	int    mSizeX; //����ָ�������"��" 
	int    mSizeY; //����ָ�������"��"

	int    xSmall;
	int    xLarge;
	int    ySmall;
	int    yLarge;
	int    step;
	HINSTANCE hinst;
};
extern void CreateMatrixByCol(float* px, float* py, int xSmall, int xLarge, int ySmall, int yLarge,
							  int step);
extern void CreateMatrixByRow(float* px, float* py, int xSmall, int xLarge, int ySmall, int yLarge,
							 int step);