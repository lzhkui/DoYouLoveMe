#pragma once

#include "afxwin.h"
#include "AdjustImage.h"
#include "ShowView.h"
#include "KGlobalDefine.h"

// #define SOLVEFLOW 1
// #define NO_SOLVEFLOW 0

//拼接两张灰度图片是否成功
enum LINK_TYPE_K
{
	INCOMING_IMAGE_NULL,
	//OUTGOING_IMAGE_NULL,

	ERROR_INPUT_RANGE,     //上下限输入有误
	IMAGE_NOTRELATED,      //两图无重合

	IMAGE2_INCLUDE_IMAGE1, //2包含1
	IMAGE1_INCLUDE_IMAGE2, //1包含2

	IMAGE1_LINK_IMAGE2,    //1左2右
	IMAGE2_LINK_IMAGE1,    //2左1右
    
	ERROR_MALLOC,
	SUCCESS_MALLOC,
	EXEC_FREE,
	NO_EXEC_FREE,

	UNDEFINED
};

//传入的两张待拼接灰度图片信息
struct st_IMAGE_INFO_k
{
	unsigned int imageWidth1;
	int RangeMin1;
	int RangeMax1;

	unsigned int imageWidth2;
	int RangeMin2;
	int RangeMax2;

	unsigned int imageHeight; //高度都一样

	unsigned int outWidth;
	unsigned int splitPoint;  //MallocOutImage时确定

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
	/*拼接两张灰度图*/
	LINK_TYPE_K SplitTwoMonoImage(st_IMAGE_INFO* pTwoImageInfo);

	//生成同高度图片
	void Get_unCh(int sign, int size);
	void Free_ImageHasSameHeight();
	void GenerateSameHeigth(AdjustImage* adjustImage, int sign);
	void StartLink(int* checkSign, int checkSignNum);

	//扩展pu pv 成与生成图片"同高度"，并不是说行列相等，只是基于图片高度，生成的另一个高度
	void Get_Float(int sign, int size);
	void GenerateSameHeigth(ShowView* showView, AdjustImage* adjustImage, int sign);
	void StartLink(ShowView* showView, int* checkSign, int checkSignNum);

	unsigned char* getImageHasSameHeight(int sign);

private:
	unsigned char* ImageHasSameHeight[MAX_CAMERAS];
	AdjustImage* adjustImage_C[MAX_CAMERAS];
	float* pu[MAX_CAMERAS];
	float* pv[MAX_CAMERAS];

	int nRow;                  //上述指针变量分配的个数
	int nCol;
};
