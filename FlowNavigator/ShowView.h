#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"
#include "CheckToShow.h"
#include "AdjustImage.h"
#include "KGloabalVar.h"

#define BASE_ONWIDTH  0  //�Կ�Ϊ��׼����
#define BASE_ONHEIGHT 1  //�Ը�Ϊ��׼����
#define BASE_SAME     2  //��߱���rect��߱�����ͬ

#define FIRSTCHECK    0  //��һ��ѡ�е����
#define LASTCHECK     1  //���һ��ѡ�е����
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
typedef struct DrawText* pst_drawText;

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


typedef BITMAPINFO* pBitmapInfo;

class ShowView
{
public:
	ShowView(CWnd *pWnd);
	~ShowView(void);

	void initBmpInfo();

	BITMAPINFO* getBmpInfo();
	pBitmapInfo getBmpInfo(int sign);

	void setWidth(long width);
	void setWidth(long width, int sign);

    void setHeight(long Height);
	void setHeight(long Height, int sign);

	//����У�����ʵ������λ��չʾ
	void LiveViewByPhysical(unsigned char* targetImage, int sign, AdjustImage* adjustImage, int originSign = -1);

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

	void LiveView_CWJ(unsigned char * pImageBuffer, BITMAPINFO* bmpInfo, int x, int y, 
		int scaleX, int scaleY, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, int nStretchMode);


	void DrawText_k(st_DrawText *pDrawText);

	void setStartPosition(int Xstart, int Ystart);
	void setStartPosition(AdjustImage* adjustImage);
	void setStartPosition(CheckToShow* checkShow, int sign);
	int  getStartPositionX();
	int  getStartPositionY();

	//��u v��max
	void FindMaxUV(int sign);
	//����󳤶�ʸ����ͷ��ʹ��չʾ�����ٻ�
	void FindMaxArrowLen(int sign);

	void DrawArrowPoisitionBySign(float* px, float* py, float* u, float* v, 
		int sizeX, int sizeY, unsigned int sign, CheckToShow* checkShow);

	void DrawArrowPoisitionBySign(AdjustImage* adjustImage, int sign, int originSign = -1);

	void DrawVectorArrow(POINT startPoint, POINT endPoint, CWnd* pWnd = NULL);
	void GenerateVectorNum(unsigned char* pBuffFirst, unsigned char* pBuffSecond, int nBuffRow, int nBuffCol, int sign = 0);

	void Get_float(int size, int sign);
	void setXRealPixel(int Xreal);
	void setYRealPixel(int Yreal);
	void setStretchMode(int nStretchMode);
	int  getStretchMode();

	void setCalcRange(int* paras, int xSmall = 0, int xLarge = 2560, int ySmall = 0, int yLarge = 2048, int step = 30);
	void setChooseAlgorithm(int AlgorithmSign); // 0: HSPIV_MQD_MP  1: HSPIV_Cross_Correlation_MP

	//���õ�ǰ�Կ������Ը�Ϊ��׼
	void setCurrentBase(int base);//BASE_ONWIDTH BASE_ONHEIGHT BASE_SAME
	void setSacle(float I);
	int  getCurrentBase();
	st_Base getCurrentLen(CRect rect);

	st_FlowRate getFlowRate(CPoint point, int* CheckCamSign, int Num, AdjustImage* adjustImage[]);


private:
	BITMAPINFO *bmpInfo;

	pBitmapInfo pBmpInfo[MAX_CAMERAS];

	CWnd *pWnd;
	int Xstart;
	int Ystart;
	int Xreal;        //x����ʵ�����س���
	int Yreal;        //y����ʵ�����س���

	int AlgorithmSign;

	float I;            //У����ߣ��� ����
	int base;         //BASE_ONWIDTH BASE_ONHEIGHT BASE_SAME
	st_Base st_base;
// 	st_Base baseLen;
private:
	int showNum;      //��double_in����Ϊ8�� double_outʱΪѡ�и���
	int singleWidth;  //����showNum�õ��ĵ�����ʾ������
	int height;       //������ʾ����߶�
	BOOL DbClk;       //double_inʱ: false   double_outʱ��true
	int startHeight;  //��Կͻ������˵ĸ߶�
	int nStretchMode; //��ʾģʽ
private:
	float uMax[MAX_CAMERAS];
	float vMax[MAX_CAMERAS];
	float MaxArrowLen[MAX_CAMERAS];
public:
	float* px[MAX_CAMERAS];
	float* py[MAX_CAMERAS];
	float* pue[MAX_CAMERAS];
	float* pve[MAX_CAMERAS];
	int paras[8];
	float* pu[MAX_CAMERAS];
	float* pv[MAX_CAMERAS];
	float* pc[MAX_CAMERAS];
	int    mSizeX[MAX_CAMERAS]; //����ָ�������"��" 
	int    mSizeY[MAX_CAMERAS]; //����ָ�������"��"

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