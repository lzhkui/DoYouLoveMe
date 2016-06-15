#pragma once

#include "afxwin.h"
#include "AdjustImage.h"
#include "ShowView.h"
#include "KGlobalDefine.h"

// #define SOLVEFLOW 1
// #define NO_SOLVEFLOW 0

//ƴ�����ŻҶ�ͼƬ�Ƿ�ɹ�
enum LINK_TYPE_K
{
	INCOMING_IMAGE_NULL,
	//OUTGOING_IMAGE_NULL,

	ERROR_INPUT_RANGE,     //��������������
	IMAGE_NOTRELATED,      //��ͼ���غ�

	IMAGE2_INCLUDE_IMAGE1, //2����1
	IMAGE1_INCLUDE_IMAGE2, //1����2

	IMAGE1_LINK_IMAGE2,    //1��2��
	IMAGE2_LINK_IMAGE1,    //2��1��
    
	ERROR_MALLOC,
	SUCCESS_MALLOC,
	EXEC_FREE,
	NO_EXEC_FREE,

	UNDEFINED
};

//��������Ŵ�ƴ�ӻҶ�ͼƬ��Ϣ
struct st_IMAGE_INFO_k
{
	unsigned int imageWidth1;
	int RangeMin1;
	int RangeMax1;

	unsigned int imageWidth2;
	int RangeMin2;
	int RangeMax2;

	unsigned int imageHeight; //�߶ȶ�һ��

	unsigned int outWidth;
	unsigned int splitPoint;  //MallocOutImageʱȷ��

	unsigned char *pImageBuff1;
	unsigned char *pImageBuff2;

	unsigned char *pOutImage;
};
typedef struct st_IMAGE_INFO_k st_IMAGE_INFO;
typedef struct st_IMAGE_INFO_k* pst_IMAGE_INFO;

struct st_ImageInfo_k
{
	unsigned int imageHeight1;
	int RangeMin1;
	int RangeMax1;

	unsigned int imageHeight12;
	int RangeMin2;
	int RangeMax2;

	unsigned int outHeight;

	unsigned char *pImageBuff1;
	unsigned char *pImageBuff2;

	unsigned char *pOutImage;
};

extern BOOL mLink;

class LinkImage
{
public:
	LinkImage(void);
	~LinkImage(void);
	void initial();

	LINK_TYPE_K isTwoImageRelated(int RangeMin1,int RangeMax1,int RangeMin2,int RangeMax2);

	LINK_TYPE_K MallocOutImage(st_IMAGE_INFO* pTwoImageInfo, LINK_TYPE_K type);
	LINK_TYPE_K MallocOutImageBeInclude(unsigned char* outImage, unsigned char* origImage,
		unsigned int height, unsigned int width);
	LINK_TYPE_K FreeOutImage(st_IMAGE_INFO* pTwoImageInfo);


	void swapTwoBuff(unsigned char **buf1, unsigned char **buf2);

	void Split(st_IMAGE_INFO *pTwoImageInfo);
	LINK_TYPE_K SplitRelated(st_IMAGE_INFO* pTwoImageInfo, LINK_TYPE_K type);
	/*ƴ�����ŻҶ�ͼ*/
	LINK_TYPE_K SplitTwoMonoImage(st_IMAGE_INFO* pTwoImageInfo);

	//����ͬ�߶�ͼƬ
	void Get_unCh(int sign, int size);
	void Free_ImageHasSameHeight();
	void GenerateSameHeigth(AdjustImage* adjustImage, int sign);
	void StartLink(int* checkSign, int checkSignNum);

	//��չpu pv ��������ͼƬ"ͬ�߶�"��������˵������ȣ�ֻ�ǻ���ͼƬ�߶ȣ����ɵ���һ���߶�
	void Get_Float(int sign, int size);
	void GenerateSameHeigth(ShowView* showView, AdjustImage* adjustImage, int sign);
	void StartLink(ShowView* showView, int* checkSign, int checkSignNum);

	unsigned char* getImageHasSameHeight(int sign);

private:
	unsigned char* ImageHasSameHeight[MAX_CAMERAS];
	AdjustImage* adjustImage_C[MAX_CAMERAS];
	float* pu[MAX_CAMERAS];
	float* pv[MAX_CAMERAS];

	int nRow;                  //����ָ���������ĸ���
	int nCol;
};
