#include "StdAfx.h"
#include "AdjustImage.h"

AdjustImage::AdjustImage(unsigned int imageSize, unsigned char* relatedArray)
{
	//pAqImageInfo_k = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + imageSize);
	relatedArray_k = relatedArray;
	imageSize_k = imageSize;
	pAqImageInfo_k = NULL;
	pAqImageInfo_pair[0] = NULL;
	pAqImageInfo_pair[1] = NULL;
}

AdjustImage::AdjustImage(J_tIMAGE_INFO* pAqImageInfo, unsigned char* relatedArray)
{
	pAqImageInfo_k = (J_tIMAGE_INFO*)malloc(sizeof(J_tIMAGE_INFO) + pAqImageInfo->iImageSize);
	if(isMallocOk(pAqImageInfo_k))
	{
		*pAqImageInfo_k = *pAqImageInfo;
	}
	relatedArray_k = relatedArray;
	imageSize_k = pAqImageInfo->iImageSize;
	pAqImageInfo_pair[0] = NULL;
	pAqImageInfo_pair[1] = NULL;
}

AdjustImage::~AdjustImage(void)
{
	if(pAqImageInfo_k != NULL)
	{
		free(pAqImageInfo_k);
		pAqImageInfo_k =NULL;
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
	for (int i = 0; i < pAqImageInfo_k->iImageSize; i++)
	{
		*(targetArray + i) = *(pAqImageInfo_k->pImageBuffer + *(relatedArray + i));
	}

	return targetArray;
}


unsigned int AdjustImage::getImageSize(void)
{
	return this->imageSize_k;
}
