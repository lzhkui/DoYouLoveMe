#include "StdAfx.h"
#include "LinkImage.h"

BOOL mLink = FALSE;

LinkImage::LinkImage(void)
{
}

LinkImage::~LinkImage(void)
{

}

//冒泡排序
void BubbleSortArray(int *RangeArray, unsigned int ArraySize)
{
	int tempIntNum;
	int tempHead, tempTail;
	int headDerection = 0;
	int TailDerection = ArraySize -1;

	while (headDerection < TailDerection)
	{
		tempTail = TailDerection - 1;
		TailDerection = 0;
		for(tempHead = headDerection; tempHead <= tempTail; tempHead++)
		{
			if(RangeArray[tempHead] > RangeArray[tempHead + 1])
			{
				tempIntNum = RangeArray[tempHead];
				RangeArray[tempHead] = RangeArray[tempHead + 1];
				RangeArray[tempHead + 1] = tempIntNum;
				TailDerection = tempHead; //总能选到最大的数字原来的位置也就是调换后最大数字前一位
			}
		}
		tempTail = headDerection + 1;
		headDerection = 0;
		for (tempHead = TailDerection; tempHead >= tempTail; tempHead--)//从右边最大数字前一位开始到 最小数字的后一位
		{
			if (RangeArray[tempHead-1] > RangeArray[tempHead])
			{
				tempIntNum = RangeArray[tempHead];
				RangeArray[tempHead] = RangeArray[tempHead - 1];
				RangeArray[tempHead - 1] = tempIntNum;
				headDerection = tempHead;
			}
		}
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
