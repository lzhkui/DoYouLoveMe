#pragma once
#include <Jai_Factory.h>

#define PAIR 2

class AdjustImage
{
public:
	AdjustImage(unsigned int imageSize, unsigned char* relatedArray);
	AdjustImage::AdjustImage(J_tIMAGE_INFO* pAqImageInfo, unsigned char* relatedArray);

	~AdjustImage(void);

private:
	J_tIMAGE_INFO* pAqImageInfo_k;
	J_tIMAGE_INFO* pAqImageInfo_pair[PAIR];//Ò»¶Ô

public:
	unsigned char* relatedArray_k;

public:
	bool setImageInfo(J_tIMAGE_INFO *pAqImageInfo);
	int setPairImageInfo(int* i, J_tIMAGE_INFO* pAqImageInfo);

	J_tIMAGE_INFO* getImageInfo();
	J_tIMAGE_INFO* getImageInfo(int i);

	unsigned int getImageSize(void);

	unsigned char* AdjustQX(J_tIMAGE_INFO* pAqImageInfo_k, unsigned const char* relatedArray,
		unsigned char* targetArray);

public:
	bool isImageInfoNULL();
	bool isMallocOk(J_tIMAGE_INFO* pAqImageInfo);


private:
	unsigned int imageSize_k;
};
