#pragma once
#include <Jai_Factory.h>
#include "KGloabalVar.h"

#define PAIR 2

class AdjustImage
{
public:
	AdjustImage(unsigned int imageSize, unsigned char* relatedArray);
	AdjustImage::AdjustImage(J_tIMAGE_INFO* pAqImageInfo, unsigned char* relatedArray);

	~AdjustImage(void);	

public:
	unsigned char* relatedArray_k;

private:
	J_tIMAGE_INFO* pAqImageInfo_k;

	J_tIMAGE_INFO* pAqImageInfo_pair[PAIR];//一对
	unsigned char* Generate_pair[PAIR];    //校正后生成一对

	unsigned int imageSize_k;
    unsigned int mAdj_ImgSize;             //拼接前，校正后大小

	st_Range m_stRange;
	float    L[2];                         //L[0]: x方向分辨率   L[1]: y方向分辨率
	
	int mAdjustXRange;                     //拼接后X方向的长度（单位：像素）
	int mAdjustYRange;                     //拼接后Y方向的长度（单位：像素）

    st_ClientRange m_stClientRange;
	st_StartPosition m_stStartPosition;

public:
	void initialAdjust();

	bool setImageInfo(J_tIMAGE_INFO *pAqImageInfo);
	int  setPairImageInfo(int* i, J_tIMAGE_INFO* pAqImageInfo);

	J_tIMAGE_INFO* getImageInfo();
	J_tIMAGE_INFO* getImageInfo(int i);

	//得到原始图片大小，初始化传入
	unsigned int getImageSize(void);
	//设置校正后的图片大小，
	void setAdjustImageSize(unsigned int size);
	//获得校正后的图片大小
	unsigned int getAdjustImageSize();

	void Get_unCh();
	void FreeGenerate_pair();

	unsigned char* AdjustQX(J_tIMAGE_INFO* pAqImageInfo_k, unsigned const char* relatedArray,
		unsigned char* targetArray);
	//流场计算传入一对图片
	void AdjustQX(J_tIMAGE_INFO* pAqImageInfo1, J_tIMAGE_INFO* pAqImageInfo2);
	//图像拼接每个相机只需一张图片
	void AdjustQX(J_tIMAGE_INFO* pAqImageInfo);

	unsigned char* getGeneratePair(int sign);

	bool isImageInfoNULL();
	bool isMallocOk(J_tIMAGE_INFO* pAqImageInfo);

	//单个相机的原始实际范围
	void setSingleRange(float xMin, float xMax, float yMin, float yMax);
	st_Range getSingleRange();

	//所谓的“分辨率”，即1像素对应的实际物理距离
	void  setL(float* L);
	float* getL();
	//得到以像素为单位的范围
	int getXRange();
	int getYRange();

	//拼接后的范围
	void setAdjustXRange(int AdjustXMin, int AdjustXMax);
	void setAdjustYRange(int AdjustYMin, int AdjustYMax);
	int getAdjustXRange();
	int getAdjustYRange();

	//实际校正后的范围
	void setClientRange(float xMin, float xMax, float yMin, float yMax);
	st_ClientRange getClientRange();

	//得到以像素为单位的实际校正后范围
	int getXClientRange();
	int getYClientRange();

	//得到校正图像距离“客户区”top的长度（像素）这个客户区是基于所选择的cam进行计算最大最小得到的
	int getLenWithTop();

	st_StartPosition getStartPosition(CRect rect);
};
