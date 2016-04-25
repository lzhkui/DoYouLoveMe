#pragma once
#include <Jai_Factory.h>

class AdjustImage
{
public:
	AdjustImage(unsigned int imageSize, unsigned char* relatedArray);
	AdjustImage::AdjustImage(J_tIMAGE_INFO* pAqImageInfo, unsigned char* relatedArray);

	~AdjustImage(void);

private:
	J_tIMAGE_INFO* pAqImageInfo_k;

public:
	unsigned char* relatedArray_k;

public:
	bool setImageInfo(J_tIMAGE_INFO *pAqImageInfo);
	J_tIMAGE_INFO* getImageInfo();
	unsigned int getImageSize(void);

	unsigned char* AdjustQX(J_tIMAGE_INFO* pAqImageInfo_k, unsigned const char* relatedArray,
		unsigned char* targetArray);

public:
	bool isImageInfoNULL();
	bool isMallocOk(J_tIMAGE_INFO* pAqImageInfo);


private:
	unsigned int imageSize_k;
};
