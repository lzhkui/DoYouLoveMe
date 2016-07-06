#pragma once


#define INT_FPRINTF      0
#define UNSIGNED_FPRINTF 0
#define FLOAT_FPRINTF    1
#define CHAR_FPRINTF     2
#define PTR_CHAP_FPRINTF 3

extern bool readBmp(char *bmpName);
extern unsigned char* Matrix_T(unsigned char* pBuff1, int nRow, int nCol);
extern float* Matrix_T(float* pBuff, int nRow, int nCol);

template <typename T>
T* Matrix_T(T* pBuff, int nRow, int nCol)
{
	T* copy_pBuff = (T*)malloc(nRow*nCol*sizeof(T));

	for(int i =0; i < nCol; i++)
	{
		for (int j =0; j < nRow; j++)
		{
			*(copy_pBuff + i*nRow +j) = *(pBuff + j*nCol + i);
		}
	}

	return copy_pBuff;
};

//T_NUM:pBuf的元素个数
template <typename T>
double* SwitchType_From_T_to_Double(T* pBuf, int T_NUM)
{
	double* pD = (double*)malloc(sizeof(double) * T_NUM);

	for (int i = 0; i < T_NUM; i++)
	{
		*(pD + i) = (double)(*(pBuf + i));
	}

	return pD;
};
extern void BubbleSortArray(int *RangeArray, unsigned int ArraySize);
extern int WriteMatrixToDisk(char* path, int nRow, int nCol, unsigned char*buff);
extern int WriteMatrixToDisk(char* path, int nRow, int nCol, unsigned int*buff);

extern int FindMax_Int(int *nArray, int arraySize);
