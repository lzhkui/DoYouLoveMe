#pragma once
#include "AdjustImage.h"
#include "ShowView.h"
#include "LinkImage.h"
#include "CheckToShow.h"
#define MAX_CAMERAS 8

//ͼ��У��
struct st_ValidCls
{
	AdjustImage *adjustImage_C[MAX_CAMERAS];
	ShowView *showView;
	CWnd *pWnd;

	LinkImage *linkImage;
	st_IMAGE_INFO *pTwoImageInfo;

	unsigned int Count;

	CheckToShow* checkShow;
};
typedef struct st_ValidCls AdajustCLs;
typedef struct st_ValidCls* pAdajustCLs;

extern BOOL mAdjust;  // true��������ͼ��У����ť
extern BOOL AdjustING;
extern int solveFlow; //1:��ʼ�������� 0��ֹͣ��������

class AdjustThreadFunc
{
public:
	AdjustThreadFunc(void);
	~AdjustThreadFunc(void);
	static void GetNew_float(int size);

	static UINT AdjustIm(LPVOID param);
	
};
