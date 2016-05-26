#include "StdAfx.h"
#include "AdjustImage.h"

AdjustImage::AdjustImage(unsigned int imageSize, unsigned char* relatedArray)
{
	//pAqImageInfo_k = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + imageSize);
	relatedArray_k       = relatedArray;
	imageSize_k          = imageSize;
	pAqImageInfo_k       = NULL;

	initialAdjust();
}

AdjustImage::AdjustImage(J_tIMAGE_INFO* pAqImageInfo, unsigned char* relatedArray)
{
	pAqImageInfo_k = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + pAqImageInfo->iImageSize);
	if(isMallocOk(pAqImageInfo_k))
	{
		*pAqImageInfo_k = *pAqImageInfo;
	}
	relatedArray_k       = relatedArray;
	imageSize_k          = pAqImageInfo->iImageSize;

	initialAdjust();
}

void AdjustImage::initialAdjust()
{
	pAqImageInfo_pair[0] = NULL;
	pAqImageInfo_pair[1] = NULL;
	mAdj_ImgSize         = 0;

	Generate_pair[0]     = NULL;
	Generate_pair[1]     = NULL;

	for(int i = 0; i < 2; i++)
	{
		L[i] = 0;
	}

	splitLine = 0;
}

AdjustImage::~AdjustImage(void)
{
	if(pAqImageInfo_k != NULL)
	{
		free(pAqImageInfo_k);
		pAqImageInfo_k =NULL;
	}
	FreeGenerate_pair();
}

void AdjustImage::FreeGenerate_pair()
{
	for (int i = 0; i < PAIR; i++)
	{
		if(Generate_pair[i] != NULL)
		{
			free(Generate_pair[i]);
			Generate_pair[i] = NULL;
		}
	}
}

bool AdjustImage::isMallocOk(J_tIMAGE_INFO* pAqImageInfo)
{
	if(pAqImageInfo == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool AdjustImage::setImageInfo(J_tIMAGE_INFO *pAqImageInfo)
{
	if(pAqImageInfo_k == NULL)
	{
		pAqImageInfo_k = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + pAqImageInfo->iImageSize);
	}
	
	if (isMallocOk(pAqImageInfo_k))
	{
		*(this->pAqImageInfo_k) = *pAqImageInfo;
			return true;
	}
	else
	{
		return false;
	}
}

int AdjustImage::setPairImageInfo(int* i, J_tIMAGE_INFO* pAqImageInfo)
{
	if (*i == 2)
	{
		if (pAqImageInfo_pair[0] == NULL)
		{
			pAqImageInfo_pair[0] = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + pAqImageInfo->iImageSize);
		}
		*(pAqImageInfo_pair[0]) = *pAqImageInfo;
		return 1;
	}
	else if (*i == 1)
	{
		if (pAqImageInfo_pair[1] == NULL)
		{
			pAqImageInfo_pair[1] = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + pAqImageInfo->iImageSize);
		}
		*(pAqImageInfo_pair[1]) = *pAqImageInfo;

		return 2;
	}
	else
	{
		return -1;
	}
}
bool AdjustImage::isImageInfoNULL()
{
	if (this->pAqImageInfo_k == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

J_tIMAGE_INFO* AdjustImage::getImageInfo()
{
	if (isImageInfoNULL())
	{
		return NULL;
	}
	else
	{
		return this->pAqImageInfo_k;
	}
}

J_tIMAGE_INFO* AdjustImage::getImageInfo(int i)
{
	if (i > PAIR)
	{
		return NULL;
	}
	return pAqImageInfo_pair[i];
}
unsigned char* AdjustImage::AdjustQX(J_tIMAGE_INFO* pAqImageInfo_k, unsigned const char* relatedArray,
							unsigned char* targetArray)
{
	if(pAqImageInfo_k == NULL || relatedArray == NULL || targetArray == NULL)
	{
		return NULL;
	}
	for (unsigned int i = 0; i < this->imageSize_k; i++)
	{
		*(targetArray + i) = *(pAqImageInfo_k->pImageBuffer + *(relatedArray + i));
	}

	return targetArray;
}

void AdjustImage::Get_unCh()
{
	for (int i = 0; i < PAIR; i++)
	{
		if(Generate_pair[i] == NULL)
		{
			Generate_pair[i] = (unsigned char*)malloc(this->mAdj_ImgSize * sizeof(unsigned char));
			ASSERT(Generate_pair[i] != NULL);

			memset(Generate_pair[i], 0, mAdj_ImgSize * sizeof(unsigned char));
		}
	}
}

void AdjustImage::AdjustQX(J_tIMAGE_INFO* pAqImageInfo)
{
	Get_unCh();
	for (unsigned int i = 0; i < this->mAdj_ImgSize; i++)
	{
		*(Generate_pair[0] + i) = *(pAqImageInfo->pImageBuffer + *(this->relatedArray_k + i));
	}
}

void AdjustImage::AdjustQX(J_tIMAGE_INFO* pAqImageInfo1, J_tIMAGE_INFO* pAqImageInfo2)
{
	Get_unCh();
	for (unsigned int i = 0; i < this->mAdj_ImgSize; i++)
	{
		*(Generate_pair[0] + i) = *(pAqImageInfo1->pImageBuffer + *(this->relatedArray_k + i));
		*(Generate_pair[1] + i) = *(pAqImageInfo2->pImageBuffer + *(this->relatedArray_k + i));
	}
}

unsigned char* AdjustImage::getGeneratePair(int sign)
{
	ASSERT(sign < 2);

	return this->Generate_pair[sign];
}

unsigned int AdjustImage::getImageSize(void)
{
	return this->imageSize_k;
}

void AdjustImage::setAdjustImageSize(unsigned int size)
{
	this->mAdj_ImgSize = size;
}

unsigned int AdjustImage::getAdjustImageSize()
{
	return this->mAdj_ImgSize;
}

void AdjustImage::setSingleRange(float xMin, float xMax, float yMin, float yMax)
{
	this->m_stRange.xMin = xMin;
	this->m_stRange.xMax = xMax;
	this->m_stRange.yMin = yMin;
	this->m_stRange.yMax = yMax;
}

st_Range AdjustImage::getSingleRange()
{	
	return this->m_stRange;
}

void AdjustImage::setL(float* L)
{
	ASSERT(L != NULL);
	for (int i = 0; i < 2; i++)
	{
		this->L[i] = L[i];
	}
}

float* AdjustImage::getL()
{
	return this->L;
}

int AdjustImage::getXRange()
{
	return (int)((this->m_stRange.xMax - this->m_stRange.xMin) / (this->L[0]));
}

int AdjustImage::getYRange()
{
	return (int)((this->m_stRange.yMax - this->m_stRange.yMin) / (this->L[1]));
}

void AdjustImage::setAdjustXRange(int AdjustXMin, int AdjustXMax)
{
	//this->mAdjustXRange = 
}

void AdjustImage::setAdjustYRange(int AdjustYMin, int AdjustYMax)
{

}

int AdjustImage::getAdjustXRange()
{
    return 0;
}

int AdjustImage::getAdjustYRange()
{
	return 0;
}

void AdjustImage::setClientRange(float xMin, float xMax, float yMin, float yMax)
{
	this->m_stClientRange.xMin = xMin;
	this->m_stClientRange.xMax = xMax;
	this->m_stClientRange.yMin = yMin;
	this->m_stClientRange.yMax = yMax;
}

st_ClientRange AdjustImage::getClientRange()
{
	return this->m_stClientRange;
}

int AdjustImage::getXClientRange()
{
	return (int)((this->m_stClientRange.xMax - this->m_stClientRange.xMin) / (this->L[0]));
}

int AdjustImage::getYClientRange()
{
	return (int)((this->m_stClientRange.yMax - this->m_stClientRange.yMin) / (this->L[1]));
}

int AdjustImage::getLenWithTop()
{
	return (int)((this->m_stClientRange.yMax - this->m_stRange.yMax) / (this->L[1]));
}

st_StartPosition AdjustImage::getStartPosition(CRect rect)
{
	m_stStartPosition.xStart = (int)((this->m_stRange.xMin - this->m_stClientRange.xMin) * 
		rect.Width() / (this->m_stClientRange.xMax - this->m_stClientRange.xMin));

	m_stStartPosition.yStart = (int)((this->m_stClientRange.xMax - this->m_stRange.xMax) *
		rect.Height() / (this->m_stClientRange.yMax - this->m_stClientRange.yMin));

	return this->m_stStartPosition;
}

void AdjustImage::setSplitLine(float splitLine)
{
	this->splitLine = splitLine;
}

float AdjustImage::getSplitLine()
{
	return (this->splitLine - this->m_stRange.xMin);
}

int AdjustImage::getSplitLinePixel()
{
	return (int)(this->splitLine / this->L[0]);
}