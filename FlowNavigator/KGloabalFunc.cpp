#include "StdAfx.h"
#include "KGloabalFunc.h"

long bmpWidth;
long bmpHeight;
WORD kbiBitCount;
RGBQUAD *pColorTable=NULL;
unsigned char *pBmpBuf = NULL;
bool readBmp(char *bmpName)
{
	FILE *fp = fopen(bmpName,"rb");
	if (fp == NULL)
	{
		return FALSE;
	}
	fseek(fp,sizeof(BITMAPFILEHEADER),0);
	BITMAPINFOHEADER infoHead;

	fread(&infoHead,sizeof(BITMAPINFOHEADER),1,fp);
	bmpWidth = infoHead.biWidth;
	bmpHeight = infoHead.biHeight;
	kbiBitCount = infoHead.biBitCount;

	int lineByte = (bmpWidth*kbiBitCount/8 + 3)/4*4;
	if(kbiBitCount == 8)
	{
		pColorTable = new RGBQUAD[256];
		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}

	pBmpBuf = new unsigned char[lineByte*bmpHeight];
	fread(pBmpBuf,1,lineByte*bmpHeight,fp);
	fclose(fp);

	return true;
}


KGloabalFunc::KGloabalFunc(void)
{
}

KGloabalFunc::~KGloabalFunc(void)
{
}
