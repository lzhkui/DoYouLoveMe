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

// 输入原图信息，行，列
unsigned char* Matrix_T(unsigned char* pBuff, int nRow, int nCol)
{
	unsigned char* copy_pBuff = (unsigned char*)malloc(nRow*nCol*sizeof(unsigned char));

	for(int i =0; i < nCol; i++)
	{
		for (int j =0; j < nRow; j++)
		{
			*(copy_pBuff + i*nRow +j) = *(pBuff + j*nCol + i);
		}
	}

	return copy_pBuff;
}

float* Matrix_T(float* pBuff, int nRow, int nCol)
{
	float* copy_pBuff = (float*)malloc(nRow*nCol*sizeof(float));

	for(int i =0; i < nCol; i++)
	{
		for (int j =0; j < nRow; j++)
		{
			*(copy_pBuff + i*nRow +j) = *(pBuff + j*nCol + i);
		}
	}

	return copy_pBuff;

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

int WriteMatrixToDisk(char* path, int nRow, int nCol, unsigned char*buff)
{
	FILE* fp;
	fp = fopen(path, "w");
	if(fp == NULL)
	{
		return -1;
	}

	for(int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			fprintf(fp, "%d ", *(buff+i*nCol+nRow));
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	return 1;
}
int WriteMatrixToDisk(char* path, int nRow, int nCol, unsigned int*buff)
{
	FILE* fp;
	fp = fopen(path, "w");
	if(fp == NULL)
	{
		return -1;
	}

	for(int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			fprintf(fp, "%d ", *(buff+i*nCol+nRow));
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	return 1;
}

int FindMax_Int(int *nArray, int arraySize)
{
	if((nArray == NULL) && (arraySize <1))
	{
		return -1;
	}

	int max = nArray[0];
	for (int i = 1; i < arraySize; i++)
	{
		max = (max > nArray[i]) ? max : nArray[i];
	}

	return max;
}