#include "StdAfx.h"
#include "LinkImage.h"

BOOL mLink = FALSE;

LinkImage::LinkImage(void)
{
	initial();
}

LinkImage::~LinkImage(void)
{
    Free_ImageHasSameHeight();
}

void LinkImage::initial()
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		ImageHasSameHeight[i] = NULL;
		adjustImage_C[i]      = NULL;
		pu[i]                 = NULL;
		pv[i]                 = NULL;
	}
}

LINK_TYPE_K LinkImage::isTwoImageRelated(int RangeMin1,int RangeMax1,int RangeMin2,int RangeMax2)
{
	if(RangeMin1 > RangeMax1 || RangeMin2 > RangeMax2)
	{
		return ERROR_INPUT_RANGE;
	}

	if(RangeMax1 < RangeMin2 || RangeMax2 < RangeMin1 )
	{
		return IMAGE_NOTRELATED;
	}

	//1包含2
	if(RangeMin1 > RangeMin2 && RangeMax1 < RangeMax2)
	{
		return IMAGE2_INCLUDE_IMAGE1;
	}

	//2包含1
	if (RangeMin2 > RangeMin1 && RangeMax2 < RangeMax1)
	{
		return IMAGE1_INCLUDE_IMAGE2;
	}
	//保证只相交一部分而不包含全部
	if (RangeMax1 > RangeMin2 && RangeMin1 < RangeMin2 && RangeMax1 < RangeMax2)     //1左2右
	{
		return IMAGE1_LINK_IMAGE2;
	}
	else if(RangeMax2 > RangeMin1 && RangeMin2 < RangeMin1 && RangeMax2 < RangeMax1) //2左1右
	{
		return IMAGE2_LINK_IMAGE1;
	}

	return UNDEFINED;
}

LINK_TYPE_K LinkImage::FreeOutImage(st_IMAGE_INFO* pTwoImageInfo)
{
	if(pTwoImageInfo != NULL)
	{
		free(pTwoImageInfo->pOutImage);
		pTwoImageInfo->pOutImage = NULL;
		return EXEC_FREE;
	}
	return NO_EXEC_FREE;
}

LINK_TYPE_K LinkImage::MallocOutImage(st_IMAGE_INFO* pTwoImageInfo, LINK_TYPE_K type)
{
	int relatedRange;
	int splitLine;
	switch(type)
	{
	case IMAGE1_LINK_IMAGE2:
		relatedRange = pTwoImageInfo->RangeMax1 - pTwoImageInfo->RangeMin2;
		splitLine = relatedRange / 2;// /(除号)用于两个int类型时，结果只取整数部分
		pTwoImageInfo->splitPoint = pTwoImageInfo->imageWidth1 - relatedRange + splitLine;

		break;
	case IMAGE2_LINK_IMAGE1:
		relatedRange = pTwoImageInfo->RangeMax2 - pTwoImageInfo->RangeMin1;
		splitLine = relatedRange / 2;// /(除号)用于两个int类型时，结果只取整数部分
		pTwoImageInfo->splitPoint = pTwoImageInfo->imageWidth2 - relatedRange + splitLine;

		break;
	default:
		{
			return ERROR_MALLOC;
		}
	}

	pTwoImageInfo->outWidth = pTwoImageInfo->imageWidth1 + pTwoImageInfo->imageWidth2 - 2 * relatedRange;
	TRACE("pTwoImageInfo->splitPoin t= %d,pTwoImageInfo->outWidth=%d\n",pTwoImageInfo->splitPoint,pTwoImageInfo->outWidth);
	
	//要求传入时一定要置空
	if(pTwoImageInfo->pOutImage == NULL)
	{
		pTwoImageInfo->pOutImage = (unsigned char*)malloc((pTwoImageInfo->imageHeight) * (pTwoImageInfo->outWidth) * sizeof(char));
	}
	if (pTwoImageInfo->pOutImage == NULL)
	{
		return ERROR_MALLOC;
	}

	return type;
}

LINK_TYPE_K LinkImage::MallocOutImageBeInclude(unsigned char* outImage, unsigned char* origImage,
						   unsigned int height, unsigned int width)
{
	unsigned int imageSize = width * height;
	outImage = (unsigned char *)malloc(imageSize * sizeof(char));
	if (outImage == NULL)
	{
		return ERROR_MALLOC;
	}
	for (unsigned int i = 0; i < imageSize; i++)
	{
		*(outImage + i) = *(origImage + i);
	}

	return SUCCESS_MALLOC;
}

void LinkImage::swapTwoBuff(unsigned char **buf1, unsigned char **buf2)
{
	unsigned char *tempBuf = *buf1;
	*buf1 = *buf2;
	*buf2 = tempBuf;
}
void LinkImage::Split(st_IMAGE_INFO *pTwoImageInfo)
{
	for (unsigned int i = 0; i < pTwoImageInfo->imageHeight; i++)
	{
		for (unsigned int j = 0; j < pTwoImageInfo->outWidth; j++)
		{
			if (j < pTwoImageInfo->splitPoint)//MallocOutImage时splitPoint、outWidth被确定
			{
				*(pTwoImageInfo->pOutImage + j + i*(pTwoImageInfo->outWidth)) = *(pTwoImageInfo->pImageBuff1 + j
					+ i*(pTwoImageInfo->imageWidth1));
			}
			else
			{
				*(pTwoImageInfo->pOutImage + j + i*(pTwoImageInfo->outWidth)) = *(pTwoImageInfo->pImageBuff2 + j - pTwoImageInfo->splitPoint 
					+ i*(pTwoImageInfo->imageWidth2));
			}
		}
	}
}

LINK_TYPE_K LinkImage::SplitRelated(st_IMAGE_INFO* pTwoImageInfo, LINK_TYPE_K type)
{
	//入口参数检查...

	if(ERROR_MALLOC == MallocOutImage(pTwoImageInfo, type))
	{
		return ERROR_MALLOC;
	}

	if(type == IMAGE1_LINK_IMAGE2)
	{
		Split(pTwoImageInfo);

		return IMAGE1_LINK_IMAGE2;
	}
	else if(type == IMAGE2_LINK_IMAGE1)
	{
		swapTwoBuff(&(pTwoImageInfo->pImageBuff1),&(pTwoImageInfo->pImageBuff2));
		Split(pTwoImageInfo);

		return IMAGE2_LINK_IMAGE1;
	}

	return UNDEFINED;
}

LINK_TYPE_K LinkImage::SplitTwoMonoImage(st_IMAGE_INFO* pTwoImageInfo)
{
	if (pTwoImageInfo->pImageBuff1 == NULL || pTwoImageInfo->pImageBuff2 == NULL)
	{
		return INCOMING_IMAGE_NULL;
	}

	LINK_TYPE_K type = isTwoImageRelated(pTwoImageInfo->RangeMin1,pTwoImageInfo->RangeMax1,
		pTwoImageInfo->RangeMin2,pTwoImageInfo->RangeMax2);
	switch (type)
	{
		case IMAGE1_LINK_IMAGE2: 
			if(ERROR_MALLOC == SplitRelated(pTwoImageInfo, IMAGE1_LINK_IMAGE2))
			{
				return ERROR_MALLOC;
			}
			else
			{
				return IMAGE1_LINK_IMAGE2;
			}
		case IMAGE2_LINK_IMAGE1: 
			if (ERROR_MALLOC == SplitRelated(pTwoImageInfo, IMAGE2_LINK_IMAGE1))
			{
				return ERROR_MALLOC;
			}
			else
			{
				return IMAGE2_LINK_IMAGE1;
			}

		case IMAGE1_INCLUDE_IMAGE2: 
			MallocOutImageBeInclude(pTwoImageInfo->pOutImage, pTwoImageInfo->pImageBuff2,
				pTwoImageInfo->imageHeight, pTwoImageInfo->imageWidth2);
			return IMAGE1_INCLUDE_IMAGE2;

		case IMAGE2_INCLUDE_IMAGE1: 
			MallocOutImageBeInclude(pTwoImageInfo->pOutImage, pTwoImageInfo->pImageBuff1, 
				pTwoImageInfo->imageHeight, pTwoImageInfo->imageWidth1);
				return IMAGE2_INCLUDE_IMAGE1;
		default:
			{
				return UNDEFINED;
			}
	}
}

void LinkImage::Get_unCh(int sign, int size)
{
	if (ImageHasSameHeight[sign] == NULL)
	{
		ImageHasSameHeight[sign] = (unsigned char*)malloc(size);
		ASSERT(ImageHasSameHeight[sign] != NULL);

		memset(ImageHasSameHeight[sign], 0, size);
	}
}

void LinkImage::Free_ImageHasSameHeight()
{
	for(int i = 0; i < MAX_CAMERAS; i++)
	{
		if (ImageHasSameHeight[i] != NULL)
		{
			free(ImageHasSameHeight[i]);
			ImageHasSameHeight[i] = NULL;
		}
	}
}

void LinkImage::GenerateSameHeigth(AdjustImage* adjustImage, int sign)
{
	this->adjustImage_C[sign] = adjustImage;

	int ImageSize      = adjustImage->getXRange() * adjustImage->getYRange();
	int DestImageSize  = adjustImage->getYClientRange() * adjustImage->getXRange();

	if(DestImageSize < ImageSize)
	{
		DestImageSize = ImageSize;
	}

	int topZero        = adjustImage->getLenWithTop() * adjustImage->getXRange(); //BugFix_20160608_ 忽略了乘以列数，导致显示位置出错

	unsigned char* imageInfo = adjustImage->getGeneratePair(0); //总是取第一张图进行拼接

	Get_unCh(sign, DestImageSize*sizeof(unsigned char));
	for (int i = topZero; i < (topZero + ImageSize); i++)
	{
		*(ImageHasSameHeight[sign] + i) = *(imageInfo + i - topZero);
	}
}

void LinkImage::StartLink(int* checkSign, int checkSignNum)
{
	ASSERT(checkSign != NULL);

	st_Range rangeF = {0};
	st_Range rangeN = {0};
	
	int xRightLen= -1; 
	int ColF     = 0;
	int ColN     = 0;

	//计算未重合区域
	int Row      = -1;
	int RowStart = 0;
	int ColStart = 0;

	//计算重合区域
	int RowCover = -1;
	int RowStartCover = 0;
	int ColStartCover = 0;
	int rightColStartCover = 0; //拼接线，在右侧图中的起始列数

	//到(checkSignNum - 2)即可，最后一个右边不需要重新调整
	for (int i = 0; i < (checkSignNum - 2); i++)
	{
		rangeF = adjustImage_C[checkSign[i]]->getSingleRange();
		rangeN = adjustImage_C[checkSign[i+1]]->getSingleRange();
		//x方向，右半重合区
		xRightLen = (int)((rangeF.xMax - rangeN.xMin) / 
			*(adjustImage_C[checkSign[i]]->getL()));

		ColF      = adjustImage_C[checkSign[i]]->getXRange();
		ColN      = adjustImage_C[checkSign[i+1]]->getXRange();


		//未重合部分右边图赋值到左边图
		if(rangeF.yMax < rangeN.yMax)
		{
			RowStart = adjustImage_C[checkSign[i+1]]->getLenWithTop();
			Row      = adjustImage_C[checkSign[i]]->getLenWithTop();

			RowStartCover = adjustImage_C[checkSign[i]]->getLenWithTop();
			RowCover      = adjustImage_C[checkSign[i+1]]->getLenWithTop() + adjustImage_C[checkSign[i+1]]->getYRange();
		}
		else if(rangeF.yMin > rangeN.yMin)
		{
			RowStart = adjustImage_C[checkSign[i]]->getLenWithTop() + adjustImage_C[checkSign[i]]->getYRange();
			Row      = adjustImage_C[checkSign[i+1]]->getLenWithTop() + adjustImage_C[checkSign[i+1]]->getYRange();

			RowStartCover = adjustImage_C[checkSign[i+1]]->getLenWithTop();
			RowCover      = adjustImage_C[checkSign[i]]->getLenWithTop() + adjustImage_C[checkSign[i]]->getYRange();
		}

		ColStart  = ColF - xRightLen;

		ColStartCover = adjustImage_C[checkSign[i]]->getSplitLinePixel();

		rightColStartCover = ColStartCover - (rangeN.xMin - rangeF.xMin) / *(adjustImage_C[checkSign[i]]->getL());
		for(int nRow = RowStart; nRow < Row; nRow++)
		{
			for(int nCol = ColStart; nCol < ColF; nCol++)
			{
				*(ImageHasSameHeight[checkSign[i]] + nCol + nRow*ColF) = *(ImageHasSameHeight[checkSign[i+1]]
				+ nCol - ColStart + nRow * ColN);
			}
		}

		for (int nRow = RowStartCover; nRow < RowCover; nRow++)
		{
			for (int nCol = ColStartCover; nCol < ColF; nCol++)
			{
				*(ImageHasSameHeight[checkSign[i]] + nCol + nRow*ColF) = *(ImageHasSameHeight[checkSign[i+1]] +
					rightColStartCover + nCol - ColStartCover + nRow*ColN);
			}
		}
	}
}

void LinkImage::Get_Float(int sign, int size)
{
	if (pu[sign] == NULL)
	{
		pu[sign] = (float*)malloc(size);
		//pu = new float[size];
		memset(pu[sign], 0, size);
	}
	if (pv[sign] == NULL)
	{
		pv[sign] = (float*)malloc(size);
		//pv = new float[size];
		memset(pv[sign], 0, size);
	}
}

void LinkImage::GenerateSameHeigth(ShowView* showView, AdjustImage* adjustImage, int sign)
{
	this->adjustImage_C[sign] = adjustImage;
	 int DestImageSize  = adjustImage->getYClientRange() * adjustImage->getXRange();

	this->nRow = (int)(adjustImage->getYClientRange() / showView->step + 1);
	this->nCol = (int)(adjustImage->getXRange() / showView->step + 1);

	int topZero = (int)(adjustImage->getLenWithTop() / showView->step + 1);
	int srcSize = showView->mSizeX[sign] * showView->mSizeY[sign];

	Get_Float(sign, nRow * nCol * sizeof(float));

	for (int i = topZero; i < topZero + srcSize; i++)
	{
		*(pu[sign] + i) = *(showView->pu[sign] + i - topZero);
		*(pv[sign] + i) = *(showView->pv[sign] + i - topZero);
	}
}

void LinkImage::StartLink(ShowView* showView, int* checkSign, int checkSignNum)
{
	int ColStartCover = 0;

	for (int i = 0; i < checkSignNum - 2; i++)
	{
		ColStartCover = adjustImage_C[checkSign[i]]->getSplitLinePixel();
	}

}

unsigned char* LinkImage::getImageHasSameHeight(int sign)
{
	return this->ImageHasSameHeight[sign];
}