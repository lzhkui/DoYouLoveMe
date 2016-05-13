#include "StdAfx.h"
#include "AdjustThreadFunc.h"
#include "FNGlobal.h"
#include "hs_piv.h"
#include "KGloabalFunc.h"

BOOL mAdjust = FALSE; // true代表点击了图像校正
BOOL AdjustING = FALSE;

unsigned char* targetArray[MAX_CAMERAS] = {NULL};
unsigned char* pair_targetArray[MAX_CAMERAS][2] = {{NULL,NULL}};


AdjustThreadFunc::AdjustThreadFunc(void)
{
}

AdjustThreadFunc::~AdjustThreadFunc(void)
{

}

unsigned char** GetMalloc_unChar(int size)
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		if (targetArray[i] == NULL)
		{
			targetArray[i] = (unsigned char*)malloc(size*sizeof(unsigned char));
		}
	}

	return targetArray;
}

UINT AdjustThreadFunc::AdjustIm(LPVOID param)
{
	pAdajustCLs m_pAdjustCls = (pAdajustCLs)param;

	//J_tIMAGE_INFO * pAqImageInfo[pAdajustCLs->Count];

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < m_pAdjustCls->Count; j++)
		{
			if (pair_targetArray[j][i] == NULL)
			{
				pair_targetArray[j][i] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[j])->getImageSize());
			}
		}
	}
	for (int m = 0; m < m_pAdjustCls->Count; m++)
	{
		GetMalloc_unChar(((m_pAdjustCls->adjustImage_C[m])->getImageSize()) * sizeof(unsigned char));
		//targetArray[m] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[m])->getImageSize());
		//pair_targetArray[m][0] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[m])->getImageSize());
		//pair_targetArray[m][1] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[m])->getImageSize());
	}
	for (int i = 0 ; i < m_pAdjustCls->Count; i++)
	{
		while((m_pAdjustCls->adjustImage_C[i])->isImageInfoNULL())
		{
			Sleep(1000);
		}
	}
	while(mAdjust)
	{
		TRACE("begin adjust!\n");
		TRACE("iBlockId = %d,pImage = %d\n",(m_pAdjustCls->adjustImage_C[0])->getImageInfo()->iBlockId,
			*((m_pAdjustCls->adjustImage_C[0])->getImageInfo()->pImageBuffer));
		//
		AdjustING = TRUE;
		for (int j = 0; j < m_pAdjustCls->Count; j++)
		{
			float begin = clock();
			(m_pAdjustCls->adjustImage_C[j])->AdjustQX((m_pAdjustCls->adjustImage_C[j])->getImageInfo(),
				(m_pAdjustCls->adjustImage_C[j])->relatedArray_k,targetArray[j]);
			float end = clock();
			TRACE("AdjustQx time[%d] = %f\n",j , end - begin); 
		}
		AdjustING = FALSE;

		float beginGetTwo = clock();
		for (int i = 0; i < m_pAdjustCls->Count; i++)
		{
			if (Pair[i] == 0)
			{
				for (int n = 0; n < (m_pAdjustCls->adjustImage_C[i])->getImageSize(); n++)
				{
					*(pair_targetArray[i][0] + n) = *((m_pAdjustCls->adjustImage_C[i])->getImageInfo(0)
						->pImageBuffer + n);
					*(pair_targetArray[i][1] + n) = *((m_pAdjustCls->adjustImage_C[i])->getImageInfo(1)
						->pImageBuffer + n);
				}
			}
			else
			{
                Sleep(10);
				i--;
			}
		}
		m_pAdjustCls->showView->GenerateVectorNum(pair_targetArray[0][0], pair_targetArray[0][1], 2048, 2560);
		m_pAdjustCls->showView->DrawArrowPoisitionBySign(m_pAdjustCls->showView->px,m_pAdjustCls->showView->py, 
			m_pAdjustCls->showView->pu,m_pAdjustCls->showView->pv,
			m_pAdjustCls->showView->mSizeX,m_pAdjustCls->showView->mSizeY, 0, m_pAdjustCls->checkShow);
		float endGetTwo = clock();
		TRACE("Get Two Image coast = %f\n",endGetTwo - beginGetTwo); 

		for (int j = 0; j < m_pAdjustCls->Count; j++) 
		{
			Pair[j] = 2;
		}

		CRect rect;
		m_pAdjustCls->pWnd->GetClientRect(&rect);

		if(mLink)
		{
			m_pAdjustCls->pTwoImageInfo->imageHeight = 2048;
			m_pAdjustCls->pTwoImageInfo->imageWidth1 = 2560;
			m_pAdjustCls->pTwoImageInfo->imageWidth2 =2560;
			m_pAdjustCls->pTwoImageInfo->RangeMin1 = 0;
			m_pAdjustCls->pTwoImageInfo->RangeMax1 = 2560;
			m_pAdjustCls->pTwoImageInfo->RangeMin2 = 2000;
			m_pAdjustCls->pTwoImageInfo->RangeMax2 = 4560;
			m_pAdjustCls->pTwoImageInfo->pImageBuff1 = targetArray[0];
			m_pAdjustCls->pTwoImageInfo->pImageBuff2 = targetArray[1];
			m_pAdjustCls->linkImage->SplitTwoMonoImage(m_pAdjustCls->pTwoImageInfo);
			
			m_pAdjustCls->showView->setWidth(m_pAdjustCls->pTwoImageInfo->outWidth);

			m_pAdjustCls->showView->LiveView_CWJ(m_pAdjustCls->pTwoImageInfo->pOutImage,m_pAdjustCls->showView->getBmpInfo(),
				0,0,rect.Width()/2,rect.Height()/2, HALFTONE);
		}
		else
		{

// 			m_pAdjustCls->showView->LiveView_CWJ(targetArray[0],m_pAdjustCls->showView->getBmpInfo(),
// 				0,0,rect.Width()/4,rect.Height()/2,HALFTONE);
			m_pAdjustCls->showView->LiveView_CWJ(targetArray[1],m_pAdjustCls->showView->getBmpInfo(),
				rect.Width()/4,0,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
	}

	free((m_pAdjustCls->adjustImage_C[0])->relatedArray_k);
	(m_pAdjustCls->adjustImage_C[0])->relatedArray_k = NULL;

	for(int k = 0; k < m_pAdjustCls->Count; k++)
	{
		free (targetArray[k]);
		targetArray[k] = NULL;

		delete m_pAdjustCls->adjustImage_C[k];
		m_pAdjustCls->adjustImage_C[k] = NULL;

		delete m_pAdjustCls->showView;
		m_pAdjustCls->showView = NULL;
	}
	return 0;
}

/*
FILE *file = fopen(("C:\\Users\\Hawksoft\\Desktop\\test.txt"),"w+");
fprintf(file,"%s", "px:");
for (int i = 0; i <nRow; i++)
{
	for(int j = 0; j < nCol; j++)
	{
		fprintf(file,"%f " ,*(px+i*nCol + j));
	}
	fprintf(file,"\n");
}

fprintf(file,"\n%s", "py:");
for (int i = 0; i <nRow; i++)
{
	for(int j = 0; j < nCol; j++)
	{
		fprintf(file,"%f " ,*(py+i*nCol + j));
	}
	fprintf(file,"\n");
}


fprintf(file,"%s", "pue:");

fprintf(file,"%f", pue);
fprintf(file,"%s", "pve:");

fprintf(file,"%f", pve);
fprintf(file,"%s", "paras:");

for (int i = 0; i < 8; i++)
{
	fprintf(file,"%d", paras[i]);
}

fflush(file);
fclose(file);
*/