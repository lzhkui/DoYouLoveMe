#pragma once


#define INT_FPRINTF      0
#define UNSIGNED_FPRINTF 0
#define FLOAT_FPRINTF    1
#define CHAR_FPRINTF     2
#define PTR_CHAP_FPRINTF 3

extern bool readBmp(char *bmpName);
extern unsigned char* Matrix_T(unsigned char* pBuff1, int nRow, int nCol);
extern float* Matrix_T(float* pBuff, int nRow, int nCol);
extern void BubbleSortArray(int *RangeArray, unsigned int ArraySize);
extern int WriteMatrixToDisk(char* path, int nRow, int nCol, unsigned char*buff);
extern int WriteMatrixToDisk(char* path, int nRow, int nCol, unsigned int*buff);

extern int FindMax_Int(int *nArray, int arraySize);
