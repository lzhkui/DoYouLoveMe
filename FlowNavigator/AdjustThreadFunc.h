#pragma once
#include "AdjustImage.h"
#include "ShowView.h"
#include "LinkImage.h"

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
};
typedef struct st_ValidCls AdajustCLs;
typedef struct st_ValidCls* pAdajustCLs;

extern BOOL mAdjust;// true��������ͼ��У����ť
extern BOOL AdjustING;

class AdjustThreadFunc
{
public:
	AdjustThreadFunc(void);
	~AdjustThreadFunc(void);

	static UINT AdjustIm(LPVOID param);
};
