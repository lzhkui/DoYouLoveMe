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

	J_tIMAGE_INFO* pAqImageInfo_pair[PAIR];//һ��
	unsigned char* Generate_pair[PAIR];    //У��������һ��

	unsigned int imageSize_k;
    unsigned int mAdj_ImgSize;             //ƴ��ǰ��У�����С

	st_Range m_stRange;
	float    L[2];                         //L[0]: x����ֱ���   L[1]: y����ֱ���
	
	int mAdjustXRange;                     //ƴ�Ӻ�X����ĳ��ȣ���λ�����أ�
	int mAdjustYRange;                     //ƴ�Ӻ�Y����ĳ��ȣ���λ�����أ�

    st_ClientRange m_stClientRange;
	st_StartPosition m_stStartPosition;

public:
	void initialAdjust();

	bool setImageInfo(J_tIMAGE_INFO *pAqImageInfo);
	int  setPairImageInfo(int* i, J_tIMAGE_INFO* pAqImageInfo);

	J_tIMAGE_INFO* getImageInfo();
	J_tIMAGE_INFO* getImageInfo(int i);

	//�õ�ԭʼͼƬ��С����ʼ������
	unsigned int getImageSize(void);
	//����У�����ͼƬ��С��
	void setAdjustImageSize(unsigned int size);
	//���У�����ͼƬ��С
	unsigned int getAdjustImageSize();

	void Get_unCh();
	void FreeGenerate_pair();

	unsigned char* AdjustQX(J_tIMAGE_INFO* pAqImageInfo_k, unsigned const char* relatedArray,
		unsigned char* targetArray);
	//�������㴫��һ��ͼƬ
	void AdjustQX(J_tIMAGE_INFO* pAqImageInfo1, J_tIMAGE_INFO* pAqImageInfo2);
	//ͼ��ƴ��ÿ�����ֻ��һ��ͼƬ
	void AdjustQX(J_tIMAGE_INFO* pAqImageInfo);

	unsigned char* getGeneratePair(int sign);

	bool isImageInfoNULL();
	bool isMallocOk(J_tIMAGE_INFO* pAqImageInfo);

	//���������ԭʼʵ�ʷ�Χ
	void setSingleRange(float xMin, float xMax, float yMin, float yMax);
	st_Range getSingleRange();

	//��ν�ġ��ֱ��ʡ�����1���ض�Ӧ��ʵ���������
	void  setL(float* L);
	float* getL();
	//�õ�������Ϊ��λ�ķ�Χ
	int getXRange();
	int getYRange();

	//ƴ�Ӻ�ķ�Χ
	void setAdjustXRange(int AdjustXMin, int AdjustXMax);
	void setAdjustYRange(int AdjustYMin, int AdjustYMax);
	int getAdjustXRange();
	int getAdjustYRange();

	//ʵ��У����ķ�Χ
	void setClientRange(float xMin, float xMax, float yMin, float yMax);
	st_ClientRange getClientRange();

	//�õ�������Ϊ��λ��ʵ��У����Χ
	int getXClientRange();
	int getYClientRange();

	//�õ�У��ͼ����롰�ͻ�����top�ĳ��ȣ����أ�����ͻ����ǻ�����ѡ���cam���м��������С�õ���
	int getLenWithTop();

	st_StartPosition getStartPosition(CRect rect);
};
