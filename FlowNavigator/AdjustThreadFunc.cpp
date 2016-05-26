#include "StdAfx.h"
#include "AdjustThreadFunc.h"
#include "KGloabalVar.h"
#include "hs_piv.h"
#include "KGloabalFunc.h"

#define KDEBUG_ADJUST 0

BOOL mAdjust = FALSE; // true代表点击了图像校正
BOOL AdjustING = FALSE;
int solveFlow = -1;   //1:开始流场计算 0：停止流场计算

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

	int* CheckCamSign; 
	CheckCamSign = m_pAdjustCls->checkShow->getCheckCamSign();

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < m_pAdjustCls->Count; j++)
		{
			if (pair_targetArray[CheckCamSign[j]][i] == NULL)
			{
				pair_targetArray[CheckCamSign[j]][i] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[CheckCamSign[j]])->getImageSize());
			}
		}
	}
	for (int m = 0; m < m_pAdjustCls->Count; m++)
	{
		GetMalloc_unChar(((m_pAdjustCls->adjustImage_C[CheckCamSign[m]])->getImageSize()) * sizeof(unsigned char));
		//targetArray[m] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[m])->getImageSize());
		//pair_targetArray[m][0] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[m])->getImageSize());
		//pair_targetArray[m][1] = (unsigned char*)malloc((m_pAdjustCls->adjustImage_C[m])->getImageSize());
	}
	for (int i = 0 ; i < m_pAdjustCls->Count; i++)
	{
		while((m_pAdjustCls->adjustImage_C[CheckCamSign[i]])->isImageInfoNULL())
		{
			Sleep(1000);
		}
	}
	while(mAdjust)
	{
		TRACE("begin adjust!\n");

		st_Range m_stRange = {0};
		float tempYmax = 0;
		float tempYmin = 0;
		float tempXmax = 0;
		float tempXmin = 0;
		for(int i = 0; i< m_pAdjustCls->Count; i++)
		{
			m_stRange = m_pAdjustCls->adjustImage_C[CheckCamSign[i]]->getSingleRange();
			tempYmax  = (tempYmax < m_stRange.yMax) ? m_stRange.yMax : tempYmax;
			tempYmin  = (tempYmin > m_stRange.yMin) ? m_stRange.yMin : tempYmin;
			tempXmax  = (tempXmax < m_stRange.xMax) ? m_stRange.xMax : tempXmax;
			tempXmin  = (tempXmin > m_stRange.xMin) ? m_stRange.xMin : tempXmin;
		}
		for(int i =0; i < m_pAdjustCls->Count; i++)
		{
			m_pAdjustCls->adjustImage_C[CheckCamSign[i]]->setClientRange(tempXmin, tempXmax, tempYmin, tempYmax);
		}

		CRect rect;
		m_pAdjustCls->pWnd->GetClientRect(&rect);

		if(solveFlow == 1)
		{
			float beginGetTwo = clock();
			for (int i = 0; i < m_pAdjustCls->Count; i++)
			{
				int imageSize = (m_pAdjustCls->adjustImage_C[CheckCamSign[i]])->getImageSize();
				if (Pair[CheckCamSign[i]] == 0)
				{
#if KDEBUG_ADJUST
					for (int n = 0; n < imageSize; n++)
					{
						*(pair_targetArray[CheckCamSign[i]][0] + n) = *((m_pAdjustCls->adjustImage_C[CheckCamSign[i]])->getImageInfo(0)
							->pImageBuffer + n);
						*(pair_targetArray[CheckCamSign[i]][1] + n) = *((m_pAdjustCls->adjustImage_C[CheckCamSign[i]])->getImageInfo(1)
							->pImageBuffer + n);
					}
#endif
#if KDEBUG_ADJUST != 1
					m_pAdjustCls->adjustImage_C[CheckCamSign[i]]->AdjustQX((m_pAdjustCls->adjustImage_C[CheckCamSign[i]])->getImageInfo(0),
						(m_pAdjustCls->adjustImage_C[CheckCamSign[i]])->getImageInfo(1));
#endif
				}
				else
				{
					Sleep(10);
					i--;
				}
			}

			InvalidateRect(m_pAdjustCls->pWnd->GetSafeHwnd(), &rect, TRUE);
			for (int j = 0; j < m_pAdjustCls->Count; j++)
			{
				int nRow = m_pAdjustCls->adjustImage_C[CheckCamSign[j]]->getYRange();
				int nCol = m_pAdjustCls->adjustImage_C[CheckCamSign[j]]->getXRange();

#if KDEBUG_ADJUST
				m_pAdjustCls->showView->GenerateVectorNum(pair_targetArray[CheckCamSign[j]][0], 
					pair_targetArray[CheckCamSign[j]][1], nRow, nCol, CheckCamSign[j]);

				m_pAdjustCls->showView->LiveViewBySign(pair_targetArray[CheckCamSign[j]][0], CheckCamSign[j],
					m_pAdjustCls->checkShow);

				m_pAdjustCls->showView->DrawArrowPoisitionBySign(m_pAdjustCls->showView->px[CheckCamSign[j]],m_pAdjustCls->showView->py[CheckCamSign[j]], 
					m_pAdjustCls->showView->pu[CheckCamSign[j]],m_pAdjustCls->showView->pv[CheckCamSign[j]],
					m_pAdjustCls->showView->mSizeX[CheckCamSign[j]],m_pAdjustCls->showView->mSizeY[CheckCamSign[j]], j, m_pAdjustCls->checkShow);
#endif
#if KDEBUG_ADJUST != 1
				unsigned char* pair[2];
				pair[0] = m_pAdjustCls->adjustImage_C[CheckCamSign[j]]->getGeneratePair(0);
				pair[1] = m_pAdjustCls->adjustImage_C[CheckCamSign[j]]->getGeneratePair(1);

				m_pAdjustCls->showView->GenerateVectorNum(pair[0], pair[1], nRow, nCol, CheckCamSign[j]);
				m_pAdjustCls->linkImage->GenerateSameHeigth(m_pAdjustCls->adjustImage_C[CheckCamSign[j]],CheckCamSign[j]);
				m_pAdjustCls->showView->LiveViewBySign(pair[0], CheckCamSign[j],
					m_pAdjustCls->checkShow);

#endif
			}

			m_pAdjustCls->linkImage->StartLink(CheckCamSign, m_pAdjustCls->Count);
			float endGetTwo = clock();
			TRACE("Get Two Image coast = %f\n",endGetTwo - beginGetTwo); 

			for (int j = 0; j < m_pAdjustCls->Count; j++) 
			{
				Pair[CheckCamSign[j]] = 2;
			}
		}//end of if(solveFlow == 1)
		else 
		{			
			AdjustING = TRUE;
			for (int j = 0; j < m_pAdjustCls->Count; j++)
			{
				float begin = clock();
				(m_pAdjustCls->adjustImage_C[CheckCamSign[j]])->AdjustQX((m_pAdjustCls->adjustImage_C[CheckCamSign[j]])->getImageInfo(),
					(m_pAdjustCls->adjustImage_C[CheckCamSign[j]])->relatedArray_k,targetArray[CheckCamSign[j]]);
				float end = clock();
				TRACE("AdjustQx time[%d] = %f\n",j , end - begin); 
			}
			AdjustING = FALSE;

// 		if(mLink)
// 		{
			m_pAdjustCls->pTwoImageInfo->imageHeight = 2048;
			m_pAdjustCls->pTwoImageInfo->imageWidth1 = 2560;
			m_pAdjustCls->pTwoImageInfo->imageWidth2 = 2560;
			m_pAdjustCls->pTwoImageInfo->RangeMin1   = 0;
			m_pAdjustCls->pTwoImageInfo->RangeMax1   = 2560;
			m_pAdjustCls->pTwoImageInfo->RangeMin2   = 2000;
			m_pAdjustCls->pTwoImageInfo->RangeMax2   = 4560;
			m_pAdjustCls->pTwoImageInfo->pImageBuff1 = targetArray[0];
			m_pAdjustCls->pTwoImageInfo->pImageBuff2 = targetArray[1];
			m_pAdjustCls->linkImage->SplitTwoMonoImage(m_pAdjustCls->pTwoImageInfo);
			
			m_pAdjustCls->showView->setWidth(m_pAdjustCls->pTwoImageInfo->outWidth);

			m_pAdjustCls->showView->LiveView_CWJ(m_pAdjustCls->pTwoImageInfo->pOutImage,m_pAdjustCls->showView->getBmpInfo(),
				0,0,rect.Width()/2,rect.Height()/2, HALFTONE);
//		}
/*		else
		{

// 			m_pAdjustCls->showView->LiveView_CWJ(targetArray[0],m_pAdjustCls->showView->getBmpInfo(),
// 				0,0,rect.Width()/4,rect.Height()/2,HALFTONE);
// 			m_pAdjustCls->showView->LiveView_CWJ(targetArray[1],m_pAdjustCls->showView->getBmpInfo(),
// 				rect.Width()/4,0,rect.Width()/4,rect.Height()/2,HALFTONE);
		}
*/
		}
	}

	free((m_pAdjustCls->adjustImage_C[0])->relatedArray_k);
	(m_pAdjustCls->adjustImage_C[0])->relatedArray_k = NULL;

	delete m_pAdjustCls->showView;
	m_pAdjustCls->showView = NULL;

	for(int k = 0; k < m_pAdjustCls->Count; k++)
	{
		free (targetArray[CheckCamSign[k]]);
		targetArray[CheckCamSign[k]] = NULL;

		delete m_pAdjustCls->adjustImage_C[CheckCamSign[k]];
		m_pAdjustCls->adjustImage_C[CheckCamSign[k]] = NULL;
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